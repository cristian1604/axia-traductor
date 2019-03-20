#include "wxAbout.h"

wxAbout::wxAbout(wxWindow *parent) : about(parent) {
	
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

