#include "wxOptions.h"
#include "FileManager.h"

wxOptions::wxOptions(wxWindow *parent) : wxParameters(parent) {
	FileManager F;
	if (!F.loadSettings(settings)) {
		settings.colour_textCtrl = wxColour( 0, 30, 60);
		settings.colour_line_number = *wxGREEN;
		settings.colour_command_m = *wxRED;
		settings.colour_command_tool = *wxRED;
		settings.colour_text = *wxYELLOW;
		settings.colour_comments = *wxBLUE;
		settings.maximize_on_startup = false;
	}

	m_colour_textCtrl->SetColour(settings.colour_textCtrl);
	m_colour_text->SetColour(settings.colour_text);
	m_colour_m->SetColour(settings.colour_command_m);
	m_colour_line->SetColour(settings.colour_line_number);
	m_colour_tool->SetColour(settings.colour_command_tool);
	m_colour_comments->SetColour(settings.colour_comments);
	m_maximize->SetValue(settings.maximize_on_startup);
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
	FileManager F;
	F.saveSettings(settings);
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
	settings.colour_textCtrl = wxColour( 0, 30, 60);
	settings.colour_line_number = *wxGREEN;
	settings.colour_command_m = *wxRED;
	settings.colour_command_tool = *wxRED;
	settings.colour_text = *wxYELLOW;
	settings.colour_comments = *wxBLUE;
	save(event);
}

