#include "wxActualizaciones.h"
#include <SFML/Network.hpp>
#include <string>
#include "json.hpp"

wxActualizaciones::wxActualizaciones(wxWindow *parent) : ComprobarActualizaciones(parent) {
	using json = nlohmann::json;
	json jsonVersion = json::parse(R"({"fecha": "30-07-2025", "version": 20250730})");
	
	m_gauge->SetValue(10);
	// Crear una solicitud HTTP
	sf::Http http("http://erp.axia.com.ar");
	sf::Http::Request request("/version.json");
	
	// Enviar la solicitud y recibir la respuesta
	sf::Http::Response response = http.sendRequest(request);
	
	// Verificar el código de estado de la respuesta
	if (response.getStatus() == sf::Http::Response::Ok) {
		m_gauge->SetValue(50);
		
		json data = json::parse(response.getBody());
		int webVersion  = data["traductor"]["version"].get<int>();
		int thisVersion = jsonVersion["version"].get<int>();
		
		wxString fechaLocal  = wxString::FromUTF8(jsonVersion["fecha"].get<std::string>().c_str());
		wxString fechaRemota = wxString::FromUTF8(data["traductor"]["fecha"].get<std::string>().c_str());
		

		if (data["traductor"]["version"] == thisVersion) {
			
			m_staticText21->SetLabel(
									 wxString::Format(
													  "Está usando la última versión\n\n"
													  "Su versión: %d    %s\n\n"
													  "Última versión: %d    %s.",
													  thisVersion,     // %d
													  fechaLocal,      // %s
													  webVersion,      // %d
													  fechaRemota      // %s
													  )
									 );
		} else {
			m_staticText21->SetLabel(wxString::Format("HAY UNA NUEVA VERSIÓN DISPONIBLE: %d", webVersion));
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

