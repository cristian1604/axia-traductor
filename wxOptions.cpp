#include "wxOptions.h"
#include "FileManager.h"
#include <wx/msgdlg.h>
using namespace std;

s_Settings default_settings() {
	s_Settings s;
	s.colour_textCtrl = wxColour( 0, 30, 60);
	s.colour_line_number = *wxGREEN;
	s.colour_command_m = *wxRED;
	s.colour_command_tool = *wxRED;
	s.colour_text = *wxYELLOW;
	s.colour_comments = wxColour(200, 200, 200);
	s.maximize_on_startup = false;
	s.remove_m08 = false;
	s.replace_from = "";
	s.replace_to = "";
	s.last_machine = "";
	s.last_filename = "";
	s.close_after_transfer = true;
	return s;
}

wxOptions::wxOptions(wxWindow *parent) : wxOptionsBase(parent) {
	// Tamaño fijo del diseño en píxeles lógicos: se escala al DPI del monitor
	SetSize(FromDIP(GetSize()));
	Centre(wxBOTH);
	FileManager F;
	F.loadSettings(settings);   // si no hay archivo, deja los valores por defecto
	m_replace_from->SetMaxLength(32);
	m_replace_to->SetMaxLength(32);
	// ESC cierra el diálogo (wxFormBuilder no conecta evt_key_up y no hay botón Cancelar)
	Bind(wxEVT_CHAR_HOOK, &wxOptions::evt_key_up, this);
	showSettings();
}

void wxOptions::showSettings() {
	m_replace_from->SetValue(settings.replace_from);
	m_replace_to->SetValue(settings.replace_to);
	m_colour_textCtrl->SetColour(settings.colour_textCtrl);
	m_colour_text->SetColour(settings.colour_text);
	m_colour_m->SetColour(settings.colour_command_m);
	m_colour_line->SetColour(settings.colour_line_number);
	m_colour_tool->SetColour(settings.colour_command_tool);
	m_colour_comments->SetColour(settings.colour_comments);
	m_maximize->SetValue(settings.maximize_on_startup);
	m_remove_m08->SetValue(settings.remove_m08);
}

wxOptions::~wxOptions() {
	
}

void wxOptions::evt_key_up( wxKeyEvent& event )  {
	if (event.GetKeyCode() == WXK_ESCAPE) {
		this->Close();
	} else {
		event.Skip();
	}
}

void wxOptions::save( wxCommandEvent& event )  {
	settings.replace_from = m_replace_from->GetValue();
	settings.replace_to = m_replace_to->GetValue();
	FileManager F;
	if (!F.saveSettings(settings)) {
		wxMessageBox( wxT("No se pudo guardar la configuración"), "Error", wxICON_ERROR);
	}
	this->Close();
}

void wxOptions::colour_textCtrl( wxColourPickerEvent& event )  {
	settings.colour_textCtrl = m_colour_textCtrl->GetColour();
}

void wxOptions::colour_text( wxColourPickerEvent& event )  {
	settings.colour_text = m_colour_text->GetColour();
}

void wxOptions::colour_m( wxColourPickerEvent& event )  {
	settings.colour_command_m = m_colour_m->GetColour();
}

void wxOptions::colour_tool( wxColourPickerEvent& event )  {
	settings.colour_command_tool = m_colour_tool->GetColour();
}

void wxOptions::colour_comments( wxColourPickerEvent& event )  {
	settings.colour_comments = m_colour_comments->GetColour();
}

void wxOptions::colour_line( wxColourPickerEvent& event )  {
	settings.colour_line_number = m_colour_line->GetColour();
}

void wxOptions::maximize( wxCommandEvent& event )  {
	settings.maximize_on_startup = m_maximize->GetValue();
}

void wxOptions::reset_defaults( wxCommandEvent& event )  {
	settings = default_settings();
	showSettings();
	save(event);
}

void wxOptions::remove_m08( wxCommandEvent& event )  {
	settings.remove_m08 = m_remove_m08->GetValue();
}

