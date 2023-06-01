#include "wxAbout.h"

wxAbout::wxAbout(wxWindow *parent) : about(parent) {
	// Tambien se debe cambiar de wxActualizaciones!!!
	m_staticText_fechaUltimaActualizacion->SetLabel("02-06-2023");
	m_staticText_version->SetLabel("Versión 1.9");
}

wxAbout::~wxAbout() {
	
}

void wxAbout::evt_key_up( wxKeyEvent& event )  {
	if (event.GetKeyCode() == WXK_ESCAPE) {
		this->Close();
	} else {
		event.Skip();
	}
}

