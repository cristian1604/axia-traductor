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
					translated += sentence + '\n';
					translated += comments;
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

string translate_8025_to_fanuc_text(const string &source, const TranslationSettings &settings) {
	string translated;
	string aux = source;
	if (aux.empty() || aux.back() != '\n') aux += '\n';
	bool program_initiated = false;
	bool comments_inserted = false;
	string prevSentence;

	/** Bloques constantes: prólogo y epílogo **/
	// Se elimina todo lo que hay entre la línea del '%' y la línea donde
	// comienza el prólogo ("P2 = K"). Si falta alguno de los dos, no se toca.
	size_t inicio = aux.find('%');
	size_t fin = (inicio == string::npos) ? string::npos : aux.find("P2 = K", inicio);
	if (inicio != string::npos && fin != string::npos) {
		while (inicio < aux.length() && aux[inicio] != '\n') ++inicio;
		while (fin > inicio && aux[fin] != '\n') --fin;
		if (fin > inicio) aux.erase(inicio, fin - inicio);
	}

	string p1 = prologue_parameter(aux);
	string rep = "#501 = " + p1 + "\nG40\nG55\nT2.2\nM00\n#500=#5022\nG10 L2 P1 Z#500\nG54\nG00 W50\n";
	aux = block_conversion("P2 = K", "G53\n", aux, rep);

	p1 = epilogue_target(aux);
	rep = "M00\n#500=#5222-#501\nG10 L2 P1 Z#500\nG54\nGOTO" + p1 + "\n";
	aux = block_conversion("P1 = P1 F2 P2", "M30", aux, rep);

	while (!aux.empty()) {
		string line = take_line(aux);
		// Las líneas anteriores al '%' se descartan
		if (!program_initiated && (line.empty() || line[0] != '%')) {
			continue;
		}
		program_initiated = true;

		if (line.empty()) translated += "  ";

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
					translated += sentence + '\n';
					comments_inserted = true;
				}
				break;
			case '(':
				sentence = sentence + line;
				line.clear();
				break;
			case 'N':
				if (line.length() <= 2) sentence += "  ";
				break;
			case 'T': {
				// Taa.bb -> Taabb (dos dígitos cada parte)
				size_t x = sentence.find('.');
				if (x == string::npos) break;
				string tool = sentence.substr(1, x - 1);
				if (is_number(tool) && atoi(tool.c_str()) < 10) tool = "0" + tool;
				string corr = sentence.substr(x + 1);
				if (is_number(corr) && atoi(corr.c_str()) < 10 && corr[0] != '0') corr = "0" + corr;
				sentence = 'T' + tool + corr;
				break;
			}
			case 'K':
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
		}
		translated += '\n';
	}
	apply_settings(translated, settings);
	return translated;
}
