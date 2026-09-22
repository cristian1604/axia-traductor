#include "wxActualizaciones.h"
#include <SFML/Network.hpp>
#include <string>
#include "json.hpp"
#include "Version.h"

// Resultado de la consulta al servidor. Se calcula en un hilo aparte, sin
// tocar wxWidgets, y se muestra en el hilo principal con CallAfter.
struct UpdateCheck {
	bool ok = false;
	std::string error;      // descripción del problema si !ok
	int version = 0;
	std::string date;
};

static UpdateCheck query_latest_version() {
	UpdateCheck r;
	sf::Http http("http://erp.axia.com.ar");
	sf::Http::Request request("/version.json");
	// Con timeout: si el servidor no responde, la consulta termina igual
	sf::Http::Response response = http.sendRequest(request, sf::seconds(5));
	if (response.getStatus() == sf::Http::Response::ConnectionFailed) {
		r.error = "No se pudo conectar con el servidor de actualizaciones.";
		return r;
	}
	if (response.getStatus() != sf::Http::Response::Ok) {
		r.error = "El servidor respondió con el código HTTP " + std::to_string((int) response.getStatus()) + ".";
		return r;
	}
	// El JSON puede faltar o cambiar de formato: sin este try la aplicación se cerraba sola
	try {
		nlohmann::json data = nlohmann::json::parse(response.getBody());
		r.version = data.at("traductor").at("version").get<int>();
		r.date = data.at("traductor").value("fecha", std::string());
		r.ok = true;
	} catch (const std::exception &e) {
		r.error = std::string("Respuesta inesperada del servidor: ") + e.what();
	}
	return r;
}

wxActualizaciones::wxActualizaciones(wxWindow *parent) : ComprobarActualizaciones(parent) {
	// Tamaño fijo del diseño en píxeles lógicos: se escala al DPI del monitor
	SetSize(FromDIP(GetSize()));
	Centre(wxBOTH);
	m_gauge->SetValue(10);
	m_staticText21->SetLabel(wxT("Consultando la última versión..."));
	// La consulta va en un hilo para no congelar la interfaz mientras espera
	worker = std::thread([this]() {
		UpdateCheck r = query_latest_version();
		CallAfter([this, r]() { showResult(r); });
	});
}

wxActualizaciones::~wxActualizaciones() {
	// Si se cierra antes de que termine la consulta se espera (5 s como mucho);
	// el evento encolado por CallAfter lo descarta wxEvtHandler al destruirse.
	if (worker.joinable()) worker.join();
}

void wxActualizaciones::showResult(const UpdateCheck &r) {
	if (!r.ok) {
		m_gauge->SetValue(0);
		m_staticText21->SetLabel(wxString::FromUTF8(r.error.c_str()));
		Layout();
		return;
	}
	m_gauge->SetValue(100);
	wxString fechaLocal = APP_VERSION_DATE;
	wxString fechaRemota = wxString::FromUTF8(r.date.c_str());
	if (r.version == APP_VERSION) {
		m_staticText21->SetLabel(wxString::Format(
			wxT("Está usando la última versión\n\nSu versión: %d    %s\n\nÚltima versión: %d    %s."),
			APP_VERSION, fechaLocal, r.version, fechaRemota));
	} else if (r.version > APP_VERSION) {
		m_staticText21->SetLabel(wxString::Format(wxT("HAY UNA NUEVA VERSIÓN DISPONIBLE: %d    %s"), r.version, fechaRemota));
	} else {
		m_staticText21->SetLabel(wxString::Format(wxT("ESTÁS ADELANTADO!!!: %d"), r.version));
	}
	Layout();
}

void wxActualizaciones::close( wxCommandEvent& event )  {
	this->Destroy();
}
