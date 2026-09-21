#include "wxSearch.h"

wxSearch::wxSearch(wxWindow *parent) : searchDialog(parent), textCtrl(NULL), pos(-1) {
	search_term->SetFocus();
}

void wxSearch::search( wxCommandEvent& event )  {
	wxString text = textCtrl->GetValue();
	pos = text.Find(search_term->GetValue());
	if (pos >= 0) {
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

int wxSearch::search_next() {
	wxString text = textCtrl->GetValue();
	wxString term = search_term->GetValue();
	if (term.IsEmpty()) return -1;
	size_t found = text.find(term, pos + 1);
	if (found == wxString::npos) {
		found = text.find(term, 0);
		hint->SetLabel("Comienzo desde el principio");
	}
	if (found == wxString::npos) {
		hint->SetLabel("No hay coincidencias");
		return -1;
	}
	pos = (int) found;
	textCtrl->SetFocus();
	textCtrl->SetInsertionPoint(pos);
	return pos;
}

void wxSearch::evt_key_up( wxKeyEvent& event )  {
	if (event.GetKeyCode() == WXK_ESCAPE) {
		this->Close();
	} else {
		event.Skip();
	}
}
