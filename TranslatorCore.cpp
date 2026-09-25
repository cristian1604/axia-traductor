#include "TranslatorCore.h"
#include <cstdio>
#include <cstdlib>
#include <cctype>
#include <sstream>
#include <locale>

using std::string;

namespace {

/**
	Reemplaza un bloque de código delimitado por su primera instrucción (beg) y
	su última (end, inclusive). Si no se encuentran ambos, el código no cambia.
**/
string block_conversion(const string &beg, const string &end, const string &code, const string &replacement) {
	size_t l_beg = code.find(beg);
	if (l_beg == string::npos) return code;
	size_t l_end = code.find(end, l_beg);
	if (l_end == string::npos) return code;
	l_end += end.length();
	string out = code;
	return out.replace(l_beg, l_end - l_beg, replacement);
}

/**
	Obtiene el parámetro que sigue a una instrucción. Ejemplo:
	"G29 N1234" con param "G29" devuelve "N1234".
**/
string obtain_parameter(size_t beg, const string &code, const string &param) {
	size_t pos = code.find(param, beg);
	if (pos == string::npos) return "";
	size_t carriage_return = code.find('\n', pos);
	string line = code.substr(pos + param.length(), carriage_return - pos);
	size_t char_spacing_beg = line.find(' ');
	if (char_spacing_beg == string::npos) return "";
	++char_spacing_beg;
	size_t char_spacing_end = line.find(' ', char_spacing_beg);
	if (char_spacing_end == string::npos) {
		line = line.substr(char_spacing_beg);
		pos = line.find('\n');
		if (pos != string::npos) line = line.substr(0, pos);
	} else {
		line = line.substr(char_spacing_beg, char_spacing_end - char_spacing_beg);
	}
	return line;
}

// Número de línea destino del epílogo: el del G29 original más 10, porque el
// prólogo convertido inserta una línea (G05) antes de la etiqueta de reinicio.
string epilogue_target(const string &code) {
	string p1 = obtain_parameter(0, code, "G29");
	if (p1.empty()) return p1;
	p1 = p1.substr(1);
	p1 = std::to_string(atoi(p1.c_str()) + 10);
	if (p1.length() < 4) p1 = string(4 - p1.length(), '0') + p1;
	return p1;
}

string prologue_parameter(const string &code) {
	string p1 = obtain_parameter(0, code, "P2 =");
	if (!p1.empty()) p1 = p1.substr(1);
	return p1;
}

bool is_number(const string &s) {
	if (s.empty()) return false;
	size_t i = (s[0] == '-' || s[0] == '+') ? 1 : 0;
	if (i >= s.size()) return false;
	for (; i < s.size(); ++i) if (!isdigit((unsigned char) s[i])) return false;
	return true;
}

// Conversión de segundos a centésimas para G04 (independiente del locale)
string seconds_to_hundredths(const string &value) {
	std::istringstream in(value);
	in.imbue(std::locale::classic());
	double db = 0;
	in >> db;
	char buf[64];
	snprintf(buf, sizeof(buf), "%g", db * 100);
	return buf;
}

// Conversión de segundos a milisegundos enteros para el G04 del FANUC
string seconds_to_milliseconds(const string &value) {
	std::istringstream in(value);
	in.imbue(std::locale::classic());
	double db = 0;
	in >> db;
	char buf[64];
	snprintf(buf, sizeof(buf), "%.0f", db * 1000);
	return buf;
}

void replace_all(string &text, const string &from, const string &to) {
	if (from.empty()) return;
	size_t pos = 0;
	while ((pos = text.find(from, pos)) != string::npos) {
		text.replace(pos, from.length(), to);
		pos += to.length();
	}
}

void apply_settings(string &code, const TranslationSettings &s) {
	replace_all(code, s.replace_from, s.replace_to);
	if (s.remove_m08) replace_all(code, "M08", "");
}

/**
	Reemplaza el bloque de líneas completas que va desde la línea que contiene
	`beg` hasta la línea que contiene `end` (inclusive). Las etiquetas N de esas
	líneas desaparecen con ellas. Si no se encuentran ambos, no cambia nada.
**/
string replace_block_lines(const string &beg, const string &end, const string &code, const string &replacement) {
	size_t l_beg = code.find(beg);
	if (l_beg == string::npos) return code;
	size_t l_end = code.find(end, l_beg);
	if (l_end == string::npos) return code;
	size_t line_start = code.rfind('\n', l_beg);
	line_start = (line_start == string::npos) ? 0 : line_start + 1;
	size_t line_end = code.find('\n', l_end);
	line_end = (line_end == string::npos) ? code.size() : line_end + 1;
	string out = code;
	return out.replace(line_start, line_end - line_start, replacement);
}

// Etiqueta N (como número, sin ceros) de la línea que contiene `text`; "" si no tiene
string label_of_line_with(const string &code, const string &text) {
	size_t pos = code.find(text);
	if (pos == string::npos) return "";
	size_t line_start = code.rfind('\n', pos);
	line_start = (line_start == string::npos) ? 0 : line_start + 1;
	if (code[line_start] != 'N') return "";
	size_t i = line_start + 1;
	string digits;
	while (i < code.size() && isdigit((unsigned char) code[i])) digits += code[i++];
	return digits.empty() ? "" : std::to_string(atoi(digits.c_str()));
}

// "Taa.bb" -> "Taabb" con dos dígitos por parte; "" si la palabra no es una herramienta con corrector
string fanuc_tool(const string &word) {
	if (word.empty() || word[0] != 'T') return "";
	size_t dot = word.find('.');
	if (dot == string::npos) return "";
	string tool = word.substr(1, dot - 1), corr = word.substr(dot + 1);
	if (!is_number(tool) || !is_number(corr)) return "";
	char buf[32];
	snprintf(buf, sizeof(buf), "T%02d%02d", atoi(tool.c_str()), atoi(corr.c_str()));
	return buf;
}

// Primera herramienta con corrector que aparece en el programa, en formato FANUC
string first_fanuc_tool(const string &code) {
	size_t pos = 0;
	while (pos < code.size()) {
		size_t end = code.find_first_of(" \n", pos);
		if (end == string::npos) end = code.size();
		string t = fanuc_tool(code.substr(pos, end - pos));
		if (!t.empty()) return t;
		pos = end + 1;
	}
	return "";
}

/**
	Redondeo de esquina: Fagor "G36 R<r> X.. Z.." -> FANUC "X.. Z.. ,R<r>"
	(programación directa de cotas de plano; la coma es obligatoria).
	Devuelve la línea sin cambios si no contiene G36.
**/
string convert_corner_rounding(const string &line) {
	size_t g36 = line.find("G36");
	if (g36 == string::npos) return line;
	if (g36 + 3 < line.size() && line[g36 + 3] != ' ') return line;   // p. ej. G360
	string prefix = line.substr(0, g36);
	string rest = line.substr(g36 + 3);
	string radius, out;
	size_t pos = 0;
	while (pos < rest.size()) {
		size_t end = rest.find(' ', pos);
		if (end == string::npos) end = rest.size();
		string word = rest.substr(pos, end - pos);
		if (!word.empty()) {
			if (radius.empty() && word[0] == 'R') {
				radius = word.substr(1);
			} else {
				if (!out.empty()) out += ' ';
				out += word;
			}
		}
		pos = end + 1;
	}
	if (radius.empty()) return line;
	return prefix + out + " ,R" + radius;
}

string expand_template(string tpl, const string &restart, const string &restart_label, const string &tool) {
	replace_all(tpl, "{RESTART_LABEL}", restart_label);
	replace_all(tpl, "{RESTART}", restart);
	replace_all(tpl, "{TOOL}", tool);
	return tpl;
}

// Extrae la primera línea de aux (sin el '\n') y la elimina de aux
string take_line(string &aux) {
	size_t pos = aux.find('\n');
	string line;
	if (pos == string::npos) {
		line = aux;
		aux.clear();
	} else {
		line = aux.substr(0, pos);
		aux = aux.substr(pos + 1);
	}
	return line;
}

// Extrae la primera palabra de line (separada por espacio), dejando el resto en line
string take_word(string &line) {
	size_t pos = line.find(' ');
	if (pos == string::npos) pos = line.length();
	string word = line.substr(0, pos);
	line = line.substr(pos);
	return word;
}

/**
	Conversiones fijas de asignaciones sobre el resto de la línea. Si alguna se
	aplica, el resto de la línea se agrega a la sentencia y se consume.
**/
void fixed_conversions(string &sentence, string &line) {
	size_t x;
	const string p1z = "P1=Z";
	if ((x = line.find(p1z)) != string::npos) {
		line.replace(x, p1z.length(), "(P100=PPOSZ)");
		sentence += line;
		line.clear();
	}
	const string g29 = "G29";
	if ((x = line.find(g29)) != string::npos) {
		line.replace(x, g29.length(), "(GOTO");
		sentence += line + ")";
		line.clear();
	}
	const string dec = "P1 = P1 F2 P2";
	if ((x = line.find(dec)) != string::npos) {
		line.replace(x, dec.length(), "(P100 = P100 - P102)");
		sentence += line;
		line.clear();
	}
}

} // namespace

//------------------------------------------------------------------------------------------------------------------

string translate_8025_to_8035_text(const string &source, const TranslationSettings &settings) {
	string translated;
	string aux = source;
	if (aux.empty() || aux.back() != '\n') aux += '\n';
	bool program_initiated = false;
	bool comments_inserted = false;
	string comments;
	string prevSentence;

	/** Bloques constantes: prólogo y epílogo **/
	// El carácter ` al inicio de una línea evita que se convierta (uso interno).
	string p1 = prologue_parameter(aux);
	string rep = "G05\n`(P102 = " + p1 + ")\nG40\n`(ORGX 54=0 , ORGZ 54=P100)\nG54\n";
	aux = block_conversion("P2 = K", "G53\n", aux, rep);

	p1 = epilogue_target(aux);
	rep = "`(P100 = P100 - P102)\nM00 M05\n`(GOTO N" + p1 + ")\nM30";
	aux = block_conversion("P1 = P1 F2 P2", "M30", aux, rep);

	while (!aux.empty()) {
		string line = take_line(aux);
		// Las líneas anteriores al '%' se conservan como comentarios
		if (!program_initiated && (line.empty() || line[0] != '%')) {
			comments += ';' + line + '\n';
			continue;
		}
		program_initiated = true;

		if (line.empty()) translated += "  ;";

		while (!line.empty()) {
			if (line[0] == ' ') {
				translated += ' ';
				line = line.substr(1);
				continue;
			}
			string sentence = take_word(line);
			switch (sentence[0]) {
			case '%':
				if (!comments_inserted) {
					// Cabecera y comentarios previos; el último '\n' lo agrega el fin de línea
					translated += sentence;
					if (!comments.empty()) translated += '\n' + comments.substr(0, comments.size() - 1);
					comments_inserted = true;
				}
				break;
			case 'A':
				// Ángulo: A -> Q
				sentence[0] = 'Q';
				break;
			case '(':
				// Comentario: se antepone ; y se consume el resto de la línea
				sentence = ';' + sentence + line;
				line.clear();
				break;
			case 'N':
				// Separador de bloque (línea con solo la etiqueta)
				if (line.length() <= 2) sentence += "  ;";
				break;
			case 'T': {
				// Taa.bb -> Taa Dbb. Sin punto no hay corrector y queda igual.
				size_t x = sentence.find('.');
				if (x == string::npos) break;
				string tool;
				for (size_t i = 1; i < x; i++) {
					if (isdigit((unsigned char) sentence[i])) tool += sentence[i];
				}
				if (tool != "0" && tool != "00") {
					sentence = 'T' + tool + " D" + sentence.substr(x + 1);
				} else {
					sentence = 'D' + sentence.substr(x + 1);
				}
				break;
			}
			case 'K':
				// Temporización: segundos -> centésimas, solo tras G04
				if (prevSentence != "G04") break;
				sentence = 'K' + seconds_to_hundredths(sentence.substr(1));
				break;
			case '`':
				sentence = sentence.substr(1);
				break;
			}

			fixed_conversions(sentence, line);

			if (comments_inserted && sentence[0] != '%') {
				translated += sentence;
				prevSentence = sentence;
			}
			if (comments_inserted && sentence[0] == '%') {
				translated += ';';
			}
		}
		translated += '\n';
	}
	apply_settings(translated, settings);
	return translated;
}

//------------------------------------------------------------------------------------------------------------------

string default_fanuc_prologue() {
	return
		"#502 = -538 (LIMITE Z)\n"
		"G58 (SETEO FRENTE)\n"
		"M40\n"
		"G40 G97 G99\n"
		"{TOOL}\n"
		"M00\n"
		"#500=#5042\n"
		"G00 G40 W70\n"
		"\n"
		"N{RESTART_LABEL} #501 = {RESTART} (++RESTART++)\n"
		"G10 L2 P1 X0 Z#500\n"
		"G54\n"
		"\n";
}

string default_fanuc_epilogue() {
	return
		"\n"
		"#500=#5222-#501\n"
		"#503=#500-#501-3.7\n"
		"M00\n"
		"IF[#503 GT #502] GOTO{RESTART_LABEL}\n"
		"\n"
		"M5\n"
		"G28 U0\n"
		"G28 W0\n"
		"M30\n";
}

string translate_8025_to_fanuc_text(const string &source, const TranslationSettings &settings) {
	string translated;
	string aux = source;
	if (aux.empty() || aux.back() != '\n') aux += '\n';
	bool program_initiated = false;
	bool header_inserted = false;
	string prevSentence;

	/** Datos para las plantillas, tomados del programa original **/
	string restart = prologue_parameter(aux);                 // "12.500"
	string restart_label = label_of_line_with(aux, "P2 = K"); // "90"
	string tool = first_fanuc_tool(aux);                      // "T0202"

	/** Prólogo: todo lo que hay entre la línea del '%' y la línea "G53" que
	    cierra el bloque de reinicio se reemplaza por la plantilla **/
	size_t percent = aux.find('%');
	size_t restart_pos = (percent == string::npos) ? string::npos : aux.find("P2 = K", percent);
	size_t g53 = (restart_pos == string::npos) ? string::npos : aux.find("G53\n", restart_pos);
	if (g53 != string::npos) {
		size_t from = aux.find('\n', percent) + 1;
		size_t to = g53 + 4;
		aux.replace(from, to - from, expand_template(settings.fanuc_prologue, restart, restart_label, tool));
	}

	/** Epílogo: desde "P1 = P1 F2 P2" hasta "M30" **/
	aux = replace_block_lines("P1 = P1 F2 P2", "M30", aux, expand_template(settings.fanuc_epilogue, restart, restart_label, tool));

	// Las líneas anteriores al '%' (encabezado del generador: cliente, #DN, tubo,
	// pieza) se conservan como comentarios después del número de programa, en
	// mayúsculas y sin paréntesis internos, para que el graficador pueda leer
	// el bruto y las cotas de referencia también en la versión FANUC.
	string header;
	while (!aux.empty()) {
		string line = take_line(aux);
		if (!program_initiated && (line.empty() || line[0] != '%')) {
			size_t b = line.find_first_not_of(" \t\r");
			if (b == string::npos) continue;
			size_t e = line.find_last_not_of(" \t\r");
			string text = line.substr(b, e - b + 1);
			for (size_t i = 0; i < text.size(); ++i) {
				if (text[i] == '(' || text[i] == ')') text[i] = ' ';
				else text[i] = (char) toupper((unsigned char) text[i]);
			}
			header += "(" + text + ")\n";
			continue;
		}
		program_initiated = true;

		if (line.empty()) translated += "  ";
		line = convert_corner_rounding(line);

		while (!line.empty()) {
			if (line[0] == ' ') {
				translated += ' ';
				line = line.substr(1);
				continue;
			}
			string sentence = take_word(line);
			switch (sentence[0]) {
			case '%': {
				// Número de programa de cuatro dígitos (%00001 -> %0001; la 'O' la
				// pone la reenumeración, que necesita el '%' para ubicar el inicio)
				string digits = sentence.substr(1);
				if (is_number(digits) && digits[0] != '-' && digits[0] != '+') {
					char buf[16];
					snprintf(buf, sizeof(buf), "%%%04d", atoi(digits.c_str()));
					sentence = buf;
				}
				if (!header_inserted) {
					translated += sentence;   // el '\n' lo agrega el fin de línea
					if (!header.empty()) translated += '\n' + header.substr(0, header.size() - 1);
					header_inserted = true;
				}
				break;
			}
			case '(':
				// Comentario: en mayúsculas, porque el control muestra solo
				// la primera letra si hay minúsculas
				sentence = sentence + line;
				for (size_t i = 0; i < sentence.size(); ++i) sentence[i] = (char) toupper((unsigned char) sentence[i]);
				line.clear();
				break;
			case 'N':
				if (line.length() <= 2) sentence += "  ";
				break;
			case 'G':
				// Avance por vuelta: en FANUC queda fijado por G99 en el prólogo
				if (sentence == "G95") sentence = "G40";
				break;
			case 'M':
				if (sentence == "MM5") sentence = "M00";
				break;
			case 'T': {
				string t = fanuc_tool(sentence);
				if (!t.empty()) sentence = t;
				break;
			}
			case 'K':
				// Temporización: el 8025 la programa en segundos (K.2) y el FANUC
				// la espera como P en milisegundos (P200)
				if (prevSentence != "G04") break;
				sentence = 'P' + seconds_to_milliseconds(sentence.substr(1));
				break;
			case '`':
				sentence = sentence.substr(1);
				break;
			}

			if (header_inserted && sentence[0] != '%') {
				translated += sentence;
				prevSentence = sentence;
			}
		}
		translated += '\n';
	}
	apply_settings(translated, settings);
	return translated;
}
