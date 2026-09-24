#include "wxCncEditor.h"
#include "CncSyntax.h"
#include <wx/settings.h>
#include <string>
#include <vector>

// Marcadores de Scintilla para las líneas con mensajes del intérprete de trayectorias
enum { MARK_ERROR = 1, MARK_WARNING = 2 };

// Mezcla de dos colores: t = 0 da `a`, t = 1 da `b`
static wxColour blend(const wxColour &a, const wxColour &b, double t) {
	return wxColour((unsigned char) (a.Red() + (b.Red() - a.Red()) * t),
	                (unsigned char) (a.Green() + (b.Green() - a.Green()) * t),
	                (unsigned char) (a.Blue() + (b.Blue() - a.Blue()) * t));
}

// Color de la línea del cursor: el fondo un poco más claro (o más oscuro si el fondo es claro)
static wxColour caret_line_colour(const wxColour &bg) {
	int lum = (bg.Red() * 299 + bg.Green() * 587 + bg.Blue() * 114) / 1000;
	int d = lum < 128 ? 22 : -18;
	auto clamp = [](int v) { return (unsigned char) (v < 0 ? 0 : (v > 255 ? 255 : v)); };
	return wxColour(clamp(bg.Red() + d), clamp(bg.Green() + d), clamp(bg.Blue() + d));
}

wxCncEditor::wxCncEditor(wxWindow *parent, wxWindowID id)
	: wxStyledTextCtrl(parent, id, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN),
	  standard(FAGOR_8025) {
	// El coloreado lo hace la aplicación (CncSyntax) a pedido de Scintilla
	SetLexer(wxSTC_LEX_CONTAINER);
	SetWrapMode(wxSTC_WRAP_NONE);
	SetTabWidth(4);
	SetUseTabs(false);
	SetEOLMode(wxSTC_EOL_LF);
	SetMarginType(0, wxSTC_MARGIN_NUMBER);
	SetMarginWidth(1, 0);       // sin margen de símbolos
	SetMarginWidth(2, 0);       // sin margen de plegado
	SetMarginLeft(6);
	SetCaretLineVisible(true);
	SetCaretWidth(2);
	SetScrollWidthTracking(true);
	SetMouseDwellTime(wxSTC_TIME_FOREVER);
	SetViewEOL(false);

	// Errores y avisos del graficador: fondo de línea (sin margen de símbolos)
	MarkerDefine(MARK_ERROR, wxSTC_MARK_BACKGROUND);
	MarkerDefine(MARK_WARNING, wxSTC_MARK_BACKGROUND);

	Bind(wxEVT_STC_STYLENEEDED, &wxCncEditor::OnStyleNeeded, this);
	Bind(wxEVT_STC_ZOOM, &wxCncEditor::OnZoom, this);

	ApplySettings(default_settings());
}

void wxCncEditor::SetMessageMarks(const std::vector<int> &error_lines, const std::vector<int> &warning_lines) {
	ClearMessageMarks();
	for (size_t k = 0; k < warning_lines.size(); ++k) MarkerAdd(warning_lines[k], MARK_WARNING);
	for (size_t k = 0; k < error_lines.size(); ++k) MarkerAdd(error_lines[k], MARK_ERROR);
}

void wxCncEditor::ClearMessageMarks() {
	MarkerDeleteAll(MARK_ERROR);
	MarkerDeleteAll(MARK_WARNING);
}

void wxCncEditor::ApplySettings(const s_Settings &s) {
	// Fuente y fondo en el estilo por defecto, y se propaga a todos con StyleClearAll
	wxFont font(12, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxT("Courier New"));
	StyleSetFont(wxSTC_STYLE_DEFAULT, font);
	StyleSetForeground(wxSTC_STYLE_DEFAULT, s.colour_text);
	StyleSetBackground(wxSTC_STYLE_DEFAULT, s.colour_textCtrl);
	StyleClearAll();

	StyleSetForeground(STYLE_TEXT,    s.colour_text);
	StyleSetForeground(STYLE_COMMENT, s.colour_comments);
	StyleSetForeground(STYLE_HEADER,  wxColour(28, 255, 251));   // inicio de programa, como siempre
	StyleSetForeground(STYLE_LABEL,   s.colour_line_number);
	StyleSetForeground(STYLE_M,       s.colour_command_m);
	StyleSetForeground(STYLE_TOOL,    s.colour_command_tool);

	// Números de línea atenuados, sobre el mismo fondo
	StyleSetForeground(wxSTC_STYLE_LINENUMBER, s.colour_comments.ChangeLightness(70));
	StyleSetBackground(wxSTC_STYLE_LINENUMBER, s.colour_textCtrl);

	SetCaretForeground(s.colour_text);
	SetCaretLineBackground(caret_line_colour(s.colour_textCtrl));
	MarkerSetBackground(MARK_ERROR, blend(s.colour_textCtrl, wxColour(255, 0, 0), 0.35));
	MarkerSetBackground(MARK_WARNING, blend(s.colour_textCtrl, wxColour(255, 160, 0), 0.25));
	SetSelBackground(true, wxColour(60, 110, 170));
	SetSelForeground(false, wxNullColour);   // la selección conserva los colores del texto

	UpdateLineNumberWidth();
	Refresh();
}

void wxCncEditor::SetStandard(int cncStandard) {
	standard = cncStandard;
	Colourise(0, -1);   // con lexer CONTAINER dispara StyleNeeded para todo el texto
}

void wxCncEditor::SetProgram(const wxString &text, bool resetUndo) {
	SetText(text);
	if (resetUndo) {
		EmptyUndoBuffer();
		SetSavePoint();   // el texto recién cargado es el estado "guardado"
	}
	GotoPos(0);
	UpdateLineNumberWidth();
}

void wxCncEditor::OnStyleNeeded(wxStyledTextEvent &event) {
	// Se clasifica el programa completo: Scintilla trabaja con posiciones en
	// bytes (UTF-8 interno), por eso se toma el texto crudo y no un wxString.
	wxCharBuffer raw = GetTextRaw();
	std::string text(raw.data(), raw.length());
	std::vector<unsigned char> st = classify_cnc(text, standard);

	StartStyling(0);
	size_t run = 0;
	for (size_t i = 0; i < st.size(); ++i) {
		if (st[i] != st[run]) {
			SetStyling((int) (i - run), st[run]);
			run = i;
		}
	}
	if (run < st.size()) SetStyling((int) (st.size() - run), st[run]);
	UpdateLineNumberWidth();
}

void wxCncEditor::OnZoom(wxStyledTextEvent &event) {
	UpdateLineNumberWidth();
	event.Skip();
}

void wxCncEditor::UpdateLineNumberWidth() {
	int lines = GetLineCount();
	wxString widest = wxT("9999");
	while ((int) widest.Length() < (int) wxString::Format(wxT("%d"), lines).Length()) widest += wxT("9");
	SetMarginWidth(0, TextWidth(wxSTC_STYLE_LINENUMBER, widest + wxT(" ")));
}
