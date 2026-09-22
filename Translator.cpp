#include "Translator.h"
#include "TranslatorCore.h"
#include "FileManager.h"
#include "AppPaths.h"
#include "json.hpp"
#include <fstream>
#include <sstream>

// Envoltorios wxWidgets sobre el núcleo del traductor (TranslatorCore).

using json = nlohmann::json;

static json lines_to_json(const std::string &text) {
	json arr = json::array();
	std::istringstream in(text);
	std::string line;
	while (std::getline(in, line)) arr.push_back(line);
	return arr;
}

static std::string json_to_lines(const json &arr) {
	std::string out;
	for (const json &l : arr) out += l.get<std::string>() + "\n";
	return out;
}

/**
	Plantillas de prólogo y epílogo en templates.json junto al ejecutable
	(compartido). Se crea con los valores por defecto si no existe; si está
	dañado se usan los valores por defecto.
**/
static void load_templates(TranslationSettings &t) {
	std::string path = shared_config_file("templates.json").ToStdString();
	std::ifstream in(path.c_str());
	if (!in.is_open()) {
		json j;
		j["fanuc"]["prologue"] = lines_to_json(t.fanuc_prologue);
		j["fanuc"]["epilogue"] = lines_to_json(t.fanuc_epilogue);
		j["_ayuda"] = "Marcadores: {RESTART} valor de reinicio, {RESTART_LABEL} etiqueta N de esa linea, {TOOL} primera herramienta";
		std::ofstream out(path.c_str());
		if (out.is_open()) out << j.dump(4) << std::endl;
		return;
	}
	try {
		json j;
		in >> j;
		if (j.contains("fanuc")) {
			if (j["fanuc"].contains("prologue")) t.fanuc_prologue = json_to_lines(j["fanuc"]["prologue"]);
			if (j["fanuc"].contains("epilogue")) t.fanuc_epilogue = json_to_lines(j["fanuc"]["epilogue"]);
		}
	} catch (const json::exception &) {
		// plantillas por defecto
	}
}

static TranslationSettings load_translation_settings() {
	s_Settings s;
	FileManager F;
	F.loadSettings(s);   // sin archivo quedan los valores por defecto
	TranslationSettings t;
	t.remove_m08 = s.remove_m08;
	t.replace_from = s.replace_from.ToStdString();
	t.replace_to = s.replace_to.ToStdString();
	load_templates(t);
	return t;
}

void translate_8025_to_8035(wxStyledTextCtrl* elem) {
	std::string out = translate_8025_to_8035_text(elem->GetText().ToStdString(), load_translation_settings());
	elem->SetText(out);
	elem->SetFocus();
}

void translate_8025_to_Fanuc(wxStyledTextCtrl* elem) {
	std::string out = translate_8025_to_fanuc_text(elem->GetText().ToStdString(), load_translation_settings());
	elem->SetText(out);
	elem->SetFocus();
}
