#include "wxActualizaciones.h"
#include <SFML/Network.hpp>
#include <string>
#include "json.hpp"
#include "Version.h"

wxActualizaciones::wxActualizaciones(wxWindow *parent) : ComprobarActualizaciones(parent) {
	using json = nlohmann::json;
	
	m_gauge->SetValue(10);
	// Crear una solicitud HTTP
	sf::Http http("http://erp.axia.com.ar");
	sf::Http::Request request("/version.json");
	
	// Enviar la solicitud y recibir la respuesta
	// Con timeout: sin el, si el servidor no responde la interfaz queda congelada
	sf::Http::Response response = http.sendRequest(request, sf::seconds(5));
	
	// Verificar el código de estado de la respuesta
	if (response.getStatus() == sf::Http::Response::Ok) {
		m_gauge->SetValue(50);
		
		json data = json::parse(response.getBody());
		int webVersion  = data["traductor"]["version"].get<int>();
		int thisVersion = APP_VERSION;
		
		wxString fechaLocal  = APP_VERSION_DATE;
		wxString fechaRemota = wxString::FromUTF8(data["traductor"]["fecha"].get<std::string>().c_str());
		

		if (data["traductor"]["version"] == thisVersion) {
			
			m_staticText21->SetLabel(
									 wxString::Format(
													  wxT("Está usando la última versión\n\n")
													  wxT("Su versión: %d    %s\n\n")
													  wxT("Última versión: %d    %s."),
													  thisVersion,     // %d
													  fechaLocal,      // %s
													  webVersion,      // %d
													  fechaRemota      // %s
													  )
									 );
		} else {
			if (data["traductor"]["version"] > thisVersion) {
				m_staticText21->SetLabel(wxString::Format(wxT("HAY UNA NUEVA VERSIÓN DISPONIBLE: %d"), webVersion));
			} else {
				m_staticText21->SetLabel(wxString::Format(wxT("ESTÁS ADELANTADO!!!: %d"), webVersion));
			}
		}
		
		m_gauge->SetValue(100);
	} else {
		m_gauge->SetValue(0);
		m_staticText21->SetLabel("Error al realizar la solicitud HTTP.");
	}
	
}

wxActualizaciones::~wxActualizaciones() {
	
}

void wxActualizaciones::close( wxCommandEvent& event )  {
	this->Destroy();
}

