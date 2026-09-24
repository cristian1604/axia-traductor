#ifndef WXCNCEDITOR_H
#define WXCNCEDITOR_H
#include <wx/stc/stc.h>
#include <vector>
#include "Settings.h"
#include "CncStandard.h"

/**
	Editor de programas CNC sobre wxStyledTextCtrl (Scintilla): coloreado por
	sintaxis instantáneo aunque el programa tenga miles de líneas (las reglas
	están en CncSyntax, sin wxWidgets), números de línea, deshacer/rehacer y
	zoom con Ctrl+rueda. Se instancia desde el código generado por
	wxFormBuilder como CustomControl, así el diseñador no necesita conocerlo.
**/
class wxCncEditor : public wxStyledTextCtrl {
private:
	int standard;                 // CncStandard activo: define las reglas de coloreado
	void OnStyleNeeded(wxStyledTextEvent &event);
	void OnZoom(wxStyledTextEvent &event);
	void UpdateLineNumberWidth();
public:
	wxCncEditor(wxWindow *parent, wxWindowID id = wxID_ANY);
	// Sintaxis a colorear (FAGOR_8025, WAS_8035, KIA_FANUC...). Repinta.
	void SetStandard(int cncStandard);
	int GetStandard() const { return standard; }
	// Colores y fuente del editor según la configuración del usuario
	void ApplySettings(const s_Settings &s);
	// Reemplaza todo el texto; con resetUndo se descarta el historial (archivo recién abierto)
	void SetProgram(const wxString &text, bool resetUndo);
	// Fondo de las líneas con errores o avisos del intérprete de trayectorias (líneas desde 0)
	void SetMessageMarks(const std::vector<int> &error_lines, const std::vector<int> &warning_lines);
	void ClearMessageMarks();
};

#endif
