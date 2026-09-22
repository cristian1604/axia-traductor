#include "wxSearch.h"

wxSearch::wxSearch(wxWindow *parent) : searchDialog(parent), editor(NULL), pos(-1) {
	// Tamaño fijo del diseño en píxeles lógicos: se escala al DPI del monitor
	SetSize(FromDIP(GetSize()));
	Centre(wxBOTH);
	search_term->SetFocus();
}

// Busca el término desde `from` (posición en el editor); si lo encuentra lo
// selecciona, lo muestra y deja el foco en el editor.
bool wxSearch::find_from(int from) {
	wxString term = search_term->GetValue();
	if (term.IsEmpty()) return false;
	int end = 0;
	int found = editor->FindText(from, editor->GetTextLength(), term, 0, &end);
	if (found < 0) return false;
	pos = found;
	editor->SetSelection(found, end);
	editor->EnsureCaretVisible();
	editor->SetFocus();
	return true;
}

void wxSearch::search( wxCommandEvent& event )  {
	if (find_from(0)) {
		this->Close();
	} else {
		hint->SetLabel("No hay coincidencias");
		hint->SetForegroundColour(*wxRED);
	}
}

void wxSearch::assignSearchField(wxStyledTextCtrl *x) {
	editor = x;
}

wxSearch::~wxSearch() {
}

int wxSearch::search_next() {
	if (search_term->GetValue().IsEmpty()) return -1;
	if (find_from(pos + 1)) return pos;
	hint->SetLabel("Comienzo desde el principio");
	if (find_from(0)) return pos;
	hint->SetLabel("No hay coincidencias");
	return -1;
}

void wxSearch::evt_key_up( wxKeyEvent& event )  {
	if (event.GetKeyCode() == WXK_ESCAPE) {
		this->Close();
	} else {
		event.Skip();
	}
}
