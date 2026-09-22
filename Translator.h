#ifndef TRANSLATOR_H
#define TRANSLATOR_H
#include <wx/stc/stc.h>

// Envoltorios sobre el núcleo del traductor (TranslatorCore) que operan
// directamente sobre el editor y aplican la configuración del usuario.
void translate_8025_to_8035(wxStyledTextCtrl* elem);
void translate_8025_to_Fanuc(wxStyledTextCtrl* elem);

#endif
