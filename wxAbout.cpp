#include "wxAbout.h"
#include "Version.h"

wxAbout::wxAbout(wxWindow *parent) : about(parent) {
	m_staticText_fechaUltimaActualizacion->SetLabel(APP_VERSION_DATE);
	m_staticText_version->SetLabel(wxString::Format(wxT("Versión %d"), APP_VERSION));
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

