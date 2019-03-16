#include "wxSearch.h"

wxSearch::wxSearch(wxWindow *parent) : searchDialog(parent) {
	search_term->SetFocus();
}

void wxSearch::search( wxCommandEvent& event )  {
	wxString text = textCtrl->GetValue();
	long pos = text.Find(search_term->GetValue());
	if (pos > 0) {
		this->Close();
		textCtrl->SetFocus();
		textCtrl->SetInsertionPoint(pos);
		textCtrl->SetSelection(pos, pos+(search_term->GetValue()).length());
		//textCtrl->SetStyle(pos, pos+(search_term->GetValue()).length(),wxTextAttr(wxColour( 28, 255, 251), *wxRED));
	} else {
		hint->SetLabel("No hay coincidencias");
		hint->SetForegroundColour(*wxRED);
	}
}

void wxSearch::assignSearchField(wxTextCtrl *x) {
	textCtrl = x;
}

wxSearch::~wxSearch() {
		
}

