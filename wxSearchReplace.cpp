#include "wxSearchReplace.h"
#include <wx/string.h>
#include <wx/msgdlg.h>
#include <string>
using namespace std;

wxSearchReplace::wxSearchReplace(wxWindow *parent) : search_and_replace(parent), editor(NULL) {

}

wxSearchReplace::~wxSearchReplace() {

}

void wxSearchReplace::start_replace( wxCommandEvent& event )  {
	unsigned int replacements = replace(m_search->GetValue(), m_replace->GetValue());
	if (replacements > 0) {
		wxString message;
		message << "Se han hallado y reemplazado " << replacements << " elementos";
		wxMessageBox( message, "Finalizado", wxICON_INFORMATION);
		this->Close();
	} else {
		wxMessageBox( "No se ha encontrado el texto a reemplazar", "No encontrado", wxICON_ERROR);
	}

}

void wxSearchReplace::close( wxCommandEvent& event )  {
	this->Close();
}

void wxSearchReplace::assignTextField(wxStyledTextCtrl *elem) {
	editor = elem;
}

// Reemplaza todas las apariciones como una única acción deshacible
unsigned int wxSearchReplace::replace( wxString source, wxString replacement )  {
	if (source.IsEmpty()) return 0;
	wxString text = editor->GetText();
	unsigned int replacements = text.Replace(source, replacement, true);
	if (replacements > 0) {
		int pos = editor->GetCurrentPos();
		editor->SetText(text);
		editor->GotoPos(pos);
	}
	return replacements;
}

void wxSearchReplace::evt_key_up( wxKeyEvent& event )  {
	if (event.GetKeyCode() == WXK_ESCAPE || (event.GetKeyCode()==87 && event.ControlDown())) {
		this->Close();
	}
}
