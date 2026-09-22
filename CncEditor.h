#ifndef CNCEDITOR_H
#define CNCEDITOR_H
#include <wx/stc/stc.h>
#include "Settings.h"
#include "CncStandard.h"

/**
	Editor de programas CNC sobre wxStyledTextCtrl (Scintilla): coloreado por
	sintaxis instantáneo aunque el programa tenga miles de líneas (las reglas
	están en CncSyntax, sin wxWidgets), números de línea, deshacer/rehacer y
	zoom con Ctrl+rueda. Se instancia desde el código generado por
	wxFormBuilder como CustomControl, así el diseñador no necesita conocerlo.
**/
class CncEditor : public wxStyledTextCtrl {
private:
	int standard;                 // CncStandard activo: define las reglas de coloreado
	void OnStyleNeeded(wxStyledTextEvent &event);
	void OnZoom(wxStyledTextEvent &event);
	void UpdateLineNumberWidth();
public:
	CncEditor(wxWindow *parent, wxWindowID id = wxID_ANY);
	// Sintaxis a colorear (FAGOR_8025, WAS_8035, KIA_FANUC...). Repinta.
	void SetStandard(int cncStandard);
	int GetStandard() const { return standard; }
	// Colores y fuente del editor según la configuración del usuario
	void ApplySettings(const s_Settings &s);
	// Reemplaza todo el texto; con resetUndo se descarta el historial (archivo recién abierto)
	void SetProgram(const wxString &text, bool resetUndo);
};

#endif
