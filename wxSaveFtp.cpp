#include "wxSaveFtp.h"

wxSaveFtp::wxSaveFtp(wxWindow *parent) : saveFtp(parent) {
	
}

void wxSaveFtp::evt_key_up( wxKeyEvent& event )  {
	event.Skip();
}

void wxSaveFtp::search( wxCommandEvent& event )  {
	event.Skip();
}

wxSaveFtp::~wxSaveFtp() {
	
}

void wxSaveFtp::saveFtp( wxCommandEvent& event )  {
	event.Skip();
}

void wxSaveFtp::saveFtpCommand( wxCommandEvent& event )  {
	event.Skip();
}

