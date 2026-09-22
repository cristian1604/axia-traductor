#include "wxAbout.h"
#include "Version.h"

wxAbout::wxAbout(wxWindow *parent) : wxAboutBase(parent) {
	// Tamaño fijo del diseño en píxeles lógicos: se escala al DPI del monitor
	SetSize(FromDIP(GetSize()));
	Centre(wxBOTH);
	m_staticText_fechaUltimaActualizacion->SetLabel(APP_VERSION_DATE);
	m_staticText_version->SetLabel(wxString::Format(wxT("Versión %d"), APP_VERSION));
	// ESC cierra el diálogo. El wxEVT_KEY_UP que conecta wxFormBuilder solo llega
	// si el propio diálogo tiene el foco, cosa que nunca ocurre (no tiene
	// controles enfocables), y sin botón Cancelar wxDialog tampoco lo cierra solo.
	Bind(wxEVT_CHAR_HOOK, &wxAbout::evt_key_up, this);
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

