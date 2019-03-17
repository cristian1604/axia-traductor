#include "wxSearch.h"
#include <string>
using namespace std;

wxSearch::wxSearch(wxWindow *parent) : searchDialog(parent) {
	search_term->SetFocus();
}

void wxSearch::search( wxCommandEvent& event )  {
	wxString text = textCtrl->GetValue();
	pos = text.Find(search_term->GetValue());
	if (pos > 0) {
		this->Close();
		textCtrl->SetFocus();
		textCtrl->SetInsertionPoint(pos);
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

unsigned int wxSearch::search_next() {
	string text(textCtrl->GetValue());
	pos = text.find(search_term->GetValue(), pos+1);
	if (pos < 0) {
		pos = text.find(search_term->GetValue(), 0);
	}
	textCtrl->SetFocus();
	textCtrl->SetInsertionPoint(pos);
	return pos;
}
