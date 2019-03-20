#include "wxOptions.h"

wxOptions::wxOptions(wxWindow *parent) : wxParameters(parent) {
	
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

