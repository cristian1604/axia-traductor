#include "Translator.h"
#include "TranslatorCore.h"
#include "FileManager.h"

// Envoltorios wxWidgets sobre el núcleo del traductor (TranslatorCore).

static TranslationSettings load_translation_settings() {
	s_Settings s;
	FileManager F;
	F.loadSettings(s);   // sin archivo quedan los valores por defecto
	TranslationSettings t;
	t.remove_m08 = s.remove_m08;
	t.replace_from = s.replace_from.ToStdString();
	t.replace_to = s.replace_to.ToStdString();
	return t;
}

void translate_8025_to_8035(wxTextCtrl* elem) {
	std::string out = translate_8025_to_8035_text(elem->GetValue().ToStdString(), load_translation_settings());
	elem->SetValue(out);
	elem->SetFocus();
}

void translate_8025_to_Fanuc(wxTextCtrl* elem) {
	std::string out = translate_8025_to_fanuc_text(elem->GetValue().ToStdString(), load_translation_settings());
	elem->SetValue(out);
	elem->SetFocus();
}
