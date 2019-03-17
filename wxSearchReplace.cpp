#include "wxSearchReplace.h"
#include <wx/string.h>
#include <wx/msgdlg.h>
#include <string>
using namespace std;

wxSearchReplace::wxSearchReplace(wxWindow *parent) : search_and_replace(parent) {
	
}

wxSearchReplace::~wxSearchReplace() {
	
}

void wxSearchReplace::start_replace( wxCommandEvent& event )  {
	wxString text = textCtrl->GetValue();
	unsigned int replacements = text.Replace(m_search->GetValue(), m_replace->GetValue(), true);
	if (replacements > 0) {
		textCtrl->SetValue(text);
		wxString message;
		message << "Se han hallado y reemplazado " << replacements << " elementos";
		wxMessageBox( message, "Finalizado", wxICON_INFORMATION);
		this->Close();
	} else {
		wxMessageBox( "No se ha realizado ninguna palabra fuente para reemplazar", "No encontrado", wxICON_ERROR);
	}
	
}

void wxSearchReplace::close( wxCommandEvent& event )  {
	this->Close();
}

void wxSearchReplace::assignTextField(wxTextCtrl *elem) {
	textCtrl = elem;
}

unsigned int wxSearchReplace::replace( wxString source, wxString replacement )  {
	wxString text = textCtrl->GetValue();
	unsigned int replacements = text.Replace(source, replacement, true);
	textCtrl->SetValue(text);
	return replacements;
}

