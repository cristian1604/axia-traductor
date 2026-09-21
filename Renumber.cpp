#include "Renumber.h"
#include <cstdio>
#include <map>
#include <cctype>

namespace {

struct Cursor {
	const std::string &s;
	size_t i;
	explicit Cursor(const std::string &str, size_t pos = 0) : s(str), i(pos) {}
	bool eof() const { return i >= s.size(); }
	char peek() const { return eof() ? '\0' : s[i]; }
	char get() { return eof() ? '\0' : s[i++]; }
	void skip_ws() { while (!eof() && (s[i] == ' ' || s[i] == '\t')) ++i; }
	void skip_line() { while (!eof() && s[i++] != '\n') {} }
	// Lee un entero decimal (permite espacios/tabs antes). Devuelve -1 si no hay dígitos.
	int read_number() {
		skip_ws();
		if (!isdigit((unsigned char) peek())) return -1;
		int n = 0;
		while (isdigit((unsigned char) peek())) n = n * 10 + (get() - '0');
		return n;
	}
};

bool is_jump_code(const std::string &line, size_t pos) {
	// "G2" seguido de un dígito 5..9
	return pos + 2 < line.size() && line[pos] == 'G' && line[pos + 1] == '2'
		&& line[pos + 2] >= '5' && line[pos + 2] <= '9';
}

struct ParsedLine {
	std::string head;      // contenido hasta el "N" del salto (inclusive), o toda la línea
	std::string tail;      // resto de la línea después del/los número(s) de salto
	int target1 = -1;      // destino de salto (a)
	int target2 = -1;      // segundo destino (b) en la forma "Na.b"
	int label = -1;        // etiqueta N original de la línea
	int parts = 0;         // 1: línea simple, 2: con salto, 3: con salto doble
};

// Analiza una línea completa a partir de cur (que queda al comienzo de la siguiente).
// Devuelve false al final del texto.
bool parse_line(Cursor &cur, ParsedLine &out) {
	out = ParsedLine();
	cur.skip_ws();
	if (cur.eof()) return false;
	out.parts = 1;

	// Etiqueta N inicial: se descarta junto con los espacios que la siguen
	if (cur.peek() == 'N') {
		size_t save = cur.i;
		cur.get();
		int n = 0; bool digits = false;
		while (isdigit((unsigned char) cur.peek())) { n = n * 10 + (cur.get() - '0'); digits = true; }
		if (digits) {
			out.label = n;
			cur.skip_ws();
		} else {
			cur.i = save;   // "N" sin dígitos: se trata como contenido normal
		}
	}

	// Copia del contenido hasta fin de línea, detectando el primer salto G25..G29
	while (!cur.eof() && cur.peek() != '\n') {
		char c = cur.get();
		out.head += c;
		if (c == 'G' && cur.peek() == '2') {
			out.head += cur.get();
			char d = cur.peek();
			if (d >= '5' && d <= '9') {
				out.head += cur.get();
				while (cur.peek() == ' ' || cur.peek() == '\t') out.head += cur.get();
				if (cur.peek() == 'N') {
					out.head += cur.get();
					out.parts = 2;
					out.target1 = cur.read_number();
					if (cur.peek() == '.') {
						cur.get();
						out.parts = 3;
						out.target2 = cur.read_number();
					}
					while (!cur.eof() && cur.peek() != '\n') out.tail += cur.get();
					break;
				}
			}
		}
	}
	if (cur.peek() == '\n') cur.get();
	return true;
}

std::string format_n(int n) {
	char buf[32];
	snprintf(buf, sizeof(buf), "N%04d", n);
	return buf;
}

} // namespace

RenumberResult renumber_program(const std::string &input) {
	RenumberResult result;

	// 1. Inicio del programa: línea que contiene el primer '%'
	size_t percent = input.find('%');
	if (percent == std::string::npos) {
		result.error = "No se encontró el inicio de programa ('%')";
		return result;
	}
	size_t start = input.find('\n', percent);
	start = (start == std::string::npos) ? input.size() : start + 1;

	// 2. Recorrido: índice de línea de cada etiqueta N y destinos de salto usados
	std::map<int, int> label_to_line;      // etiqueta original -> índice de línea (0..)
	std::vector<int> targets;
	int line_count = 0;
	{
		Cursor cur(input, start);
		ParsedLine pl;
		while (parse_line(cur, pl)) {
			if (pl.label >= 0) label_to_line[pl.label] = line_count;   // la última ocurrencia gana
			if (pl.parts >= 2) targets.push_back(pl.target1);
			if (pl.parts >= 3) targets.push_back(pl.target2);
			++line_count;
		}
	}

	// 3. Paso e inicio según la cantidad de líneas
	int step, first;
	if      (line_count < 800)   { step = 10; first = 10; }
	else if (line_count < 1600)  { step = 5;  first = 10; }
	else if (line_count < 4000)  { step = 2;  first = 2;  }
	else if (line_count < 10000) { step = 1;  first = 0;  }
	else {
		result.error = "El programa tiene demasiadas líneas para reenumerar (máximo 9999)";
		return result;
	}

	// 4. Todos los destinos de salto deben existir
	for (size_t k = 0; k < targets.size(); ++k) {
		if (label_to_line.find(targets[k]) == label_to_line.end()) {
			result.undefined.push_back(targets[k]);
		}
	}
	if (!result.undefined.empty()) {
		result.error = "Hay saltos a líneas que no existen en el programa";
		return result;
	}

	// 5. Salida
	std::string out = input.substr(0, start);
	Cursor cur(input, start);
	ParsedLine pl;
	int n = first;
	while (parse_line(cur, pl)) {
		out += format_n(n) + "  " + pl.head;
		if (pl.parts >= 2) {
			out += format_n(first + step * label_to_line[pl.target1]).substr(1);
			if (pl.parts >= 3) {
				out += "." + format_n(first + step * label_to_line[pl.target2]).substr(1);
			}
			out += pl.tail;
		}
		out += '\n';
		n += step;
	}

	result.ok = true;
	result.text = out;
	return result;
}
