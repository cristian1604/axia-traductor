#include "wxSearch.h"

wxSearch::wxSearch(wxWindow *parent) : searchDialog(parent) {
	search_term->SetFocus();
}

void wxSearch::search( wxCommandEvent& event )  {
	wxString search = textCtrl->GetValue();
	long pos = search.Find(search_term->GetValue());
	if (pos > 0) {
		this->Close();
		textCtrl->SetFocus();
		textCtrl->SetInsertionPoint(pos);
	}
}

void wxSearch::assignSearchField(wxTextCtrl *x) {
	textCtrl = x;
}

wxSearch::~wxSearch() {
	
}

void wxSearch::search2( wxCommandEvent& event )  {
	search(event);
}

