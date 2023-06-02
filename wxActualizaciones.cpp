#include "wxActualizaciones.h"
#include <SFML/Network.hpp>
#include <string>
#include "json.hpp"

wxActualizaciones::wxActualizaciones(wxWindow *parent) : ComprobarActualizaciones(parent) {
	using json = nlohmann::json;
	json jsonVersion = json::parse(R"({"fecha": "02-06-2023", "version": 1.9})");
	
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
		wxString webVersion = data["traductor"]["version"].dump();
		wxString thisVersion = jsonVersion["version"].dump();

		if (data["traductor"]["version"] > thisVersion) {
			m_staticText21->SetLabel("HAY UNA NUEVA VERSIÓN DISPONIBLE: " + webVersion);
		} else {
			m_staticText21->SetLabel("Está usando la última versión\n\nSu version: "+ thisVersion +"    "+ jsonVersion["fecha"].dump() +"\n\nÚltima versión: " + webVersion + "    "+ data["traductor"]["fecha"].dump() +".");
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

