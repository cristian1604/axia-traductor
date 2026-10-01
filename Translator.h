#ifndef TRANSLATOR_H
#define TRANSLATOR_H
#include <wx/stc/stc.h>

// Envoltorios sobre el núcleo del traductor (TranslatorCore) que operan
// directamente sobre el editor y aplican la configuración del usuario.
// `programName` es el nombre para el encabezado del 8035 (el del archivo
// cargado, sin extensión); vacío si el programa todavía no tiene nombre.
void translate_8025_to_8035(wxStyledTextCtrl* elem, const wxString &programName);
void translate_8025_to_Fanuc(wxStyledTextCtrl* elem);

#endif
