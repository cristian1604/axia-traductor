#include "wxActualizaciones.h"
#include <SFML/Network.hpp>
#include <string>
#include "json.hpp"

wxActualizaciones::wxActualizaciones(wxWindow *parent) : ComprobarActualizaciones(parent) {
	float version = 1.7;
	wxString fecha = "09-05-2023";	
	
	m_gauge->SetValue(10);
	// Crear una solicitud HTTP
	sf::Http http("http://erp.axia.com.ar");
	sf::Http::Request request("/version.json");
	
	// Enviar la solicitud y recibir la respuesta
	sf::Http::Response response = http.sendRequest(request);
	
	// Verificar el código de estado de la respuesta
	if (response.getStatus() == sf::Http::Response::Ok) {
		m_gauge->SetValue(50);
		
		// Analizar el contenido JSON
		using json = nlohmann::json;		
		json data = json::parse(response.getBody());

		if (data["traductor"]["version"] > version) {
			m_staticText21->SetLabel("HAY UNA NUEVA VERSIÓN DISPONIBLE");
		} else {
			m_staticText21->SetLabel("Esta es la última versión");
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

