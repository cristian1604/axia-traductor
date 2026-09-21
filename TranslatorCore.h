#ifndef TRANSLATORCORE_H
#define TRANSLATORCORE_H
#include <string>

/**
	Núcleo del traductor de código CNC. Sin dependencias de wxWidgets, para
	poder probarlo con programas reales (ver tests/test_translator.cpp).
**/
struct TranslationSettings {
	bool remove_m08 = false;        // eliminar todas las instrucciones M08
	std::string replace_from;       // reemplazo configurable (vacío = no aplica)
	std::string replace_to;
};

// Fagor 8025 -> Fagor 8035 / 8037
std::string translate_8025_to_8035_text(const std::string &source, const TranslationSettings &settings);

// Fagor 8025 -> FANUC
std::string translate_8025_to_fanuc_text(const std::string &source, const TranslationSettings &settings);

#endif
