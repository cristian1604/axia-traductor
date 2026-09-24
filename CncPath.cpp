#include "CncPath.h"
#include <cmath>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <algorithm>

namespace {

const double PI = 3.14159265358979323846;
const double EPS = 1e-9;

inline bool is_digit(char c) { return c >= '0' && c <= '9'; }
inline bool is_alpha(char c) { return std::isalpha((unsigned char) c) != 0; }
inline bool is_space(char c) { return c == ' ' || c == '\t'; }
inline char upper(char c) { return (char) std::toupper((unsigned char) c); }
inline double deg2rad(double a) { return a * PI / 180.0; }

// Número con punto decimal, independiente del locale: [+-]dígitos[.dígitos] o [+-].dígitos
bool parse_number(const std::string &s, size_t &i, double &out) {
	size_t p = i;
	double sign = 1;
	if (p < s.size() && (s[p] == '+' || s[p] == '-')) {
		if (s[p] == '-') sign = -1;
		++p;
	}
	double v = 0;
	bool digits = false;
	while (p < s.size() && is_digit(s[p])) { v = v * 10 + (s[p] - '0'); ++p; digits = true; }
	if (p < s.size() && s[p] == '.') {
		++p;
		double f = 0.1;
		while (p < s.size() && is_digit(s[p])) { v += (s[p] - '0') * f; f *= 0.1; ++p; digits = true; }
	}
	if (!digits) return false;
	out = sign * v;
	i = p;
	return true;
}

std::string format_line_text(const char *fmt, int n) {
	char buf[128];
	snprintf(buf, sizeof(buf), fmt, n);
	return buf;
}

// ---- Dialecto ---------------------------------------------------------------

struct Dialect {
	bool fanuc = false;
	bool fagor_8035 = false;
	char angle = 'A';             // letra del ángulo: A en 8025 y FANUC, Q en 8035
	bool semicolon_comments = false;
};

Dialect dialect_for(int standard) {
	Dialect d;
	if (standard == KIA_FANUC) {
		d.fanuc = true;
	} else if (standard == WAS_8035 || standard == WAS_8037 || standard == TAKI_8037) {
		d.fagor_8035 = true;
		d.angle = 'Q';
		d.semicolon_comments = true;
	}
	return d;
}

// ---- Léxico -----------------------------------------------------------------

struct Word {
	char letter = 0;
	double value = 0;
	bool bare = false;     // letra sin número: vale 0 (Fagor lo tolera, FANUC no)
	bool param = false;    // valor paramétrico (P1, #500, [expr]): no resoluble
	bool comma = false;    // FANUC ",R" / ",C"
};

struct Block {
	std::vector<Word> words;
	int line = 0;
	bool junk = false;     // caracteres que no forman palabra
};

// Separa un bloque en palabras. Las asignaciones y sentencias de control ya
// vienen filtradas por el llamador.
void tokenize(const std::string &s, size_t i, const Dialect &d, Block &b) {
	while (i < s.size()) {
		char c = s[i];
		if (is_space(c)) { ++i; continue; }
		if (c == '(') {
			// Comentario (8025, FANUC) o sentencia paramétrica (8035): se salta
			size_t e = s.find(')', i);
			i = (e == std::string::npos) ? s.size() : e + 1;
			continue;
		}
		if (c == ';' && d.semicolon_comments) return;
		if (c == '/' || c == '.') { ++i; continue; }   // salto condicional, punto suelto: se ignoran
		bool comma = false;
		if (c == ',' && d.fanuc && i + 1 < s.size() && is_alpha(s[i + 1])) {
			comma = true;
			++i;
			c = s[i];
		}
		if (!is_alpha(c)) { b.junk = true; ++i; continue; }

		Word w;
		w.letter = upper(c);
		w.comma = comma;
		++i;
		while (i < s.size() && is_space(s[i])) ++i;   // "X 10" vale como X10 (el simulador anterior lo aceptaba)
		size_t p = i;
		if (parse_number(s, p, w.value)) {
			i = p;
		} else if (!d.fanuc && i < s.size() && upper(s[i]) == 'P' && i + 1 < s.size() && is_digit(s[i + 1])) {
			// Fagor: cota paramétrica "ZP1"
			w.param = true;
			i += 2;
			while (i < s.size() && is_digit(s[i])) ++i;
		} else if (d.fanuc && i < s.size() && (s[i] == '#' || s[i] == '[' || s[i] == '-')) {
			// FANUC: variable "#500" o expresión "[#100+2]"
			w.param = true;
			if (s[i] == '[') {
				size_t e = s.find(']', i);
				i = (e == std::string::npos) ? s.size() : e + 1;
			} else {
				++i;
				while (i < s.size() && (is_digit(s[i]) || s[i] == '#' || s[i] == '.')) ++i;
			}
		} else {
			w.bare = true;
		}
		b.words.push_back(w);
	}
}

// ---- Geometría --------------------------------------------------------------

struct Vec { double z, r; };
inline Vec sub(const CncPoint &a, const CncPoint &b) { Vec v = { a.z - b.z, a.r - b.r }; return v; }
inline double len(const Vec &v) { return std::sqrt(v.z * v.z + v.r * v.r); }
inline double cross(const Vec &a, const Vec &b) { return a.z * b.r - a.r * b.z; }
inline double dot(const Vec &a, const Vec &b) { return a.z * b.z + a.r * b.r; }
inline Vec dir_of_angle(double deg) { Vec v = { std::cos(deg2rad(deg)), std::sin(deg2rad(deg)) }; return v; }
inline CncPoint offset(const CncPoint &p, const Vec &v, double k) { CncPoint q; q.z = p.z + k * v.z; q.r = p.r + k * v.r; return q; }

// Intersección de la recta por p0 con ángulo a1 y la recta por p1 con ángulo a2
bool intersect_angles(const CncPoint &p0, double a1, const CncPoint &p1, double a2, CncPoint &out) {
	Vec d1 = dir_of_angle(a1), d2 = dir_of_angle(a2);
	double den = cross(d1, d2);
	if (std::fabs(den) < 1e-9) return false;   // paralelas
	double t = cross(sub(p1, p0), d2) / den;
	out = offset(p0, d1, t);
	return true;
}

// Ángulo (grados) de un punto respecto de un centro, en [0, 360)
double angle_of(const CncPoint &c, const CncPoint &p) {
	double a = std::atan2(p.r - c.r, p.z - c.z) * 180.0 / PI;
	if (a < 0) a += 360;
	return a;
}

// Extremos reales de un arco: sus puntos y los cruces con los ejes dentro del barrido
void arc_bounds(const CncSegment &s, CncBounds &b) {
	b.add(s.from);
	b.add(s.to);
	double a0 = angle_of(s.center, s.from), a1 = angle_of(s.center, s.to);
	if (s.kind == SEG_ARC_CW) std::swap(a0, a1);   // ahora el barrido es antihorario de a0 a a1
	double sweep = a1 - a0;
	if (sweep < 0) sweep += 360;
	if (sweep < 1e-9) sweep = 360;                 // círculo completo
	for (int k = 0; k < 4; ++k) {
		double a = k * 90.0;
		double rel = a - a0;
		if (rel < 0) rel += 360;
		if (rel <= sweep) b.add(s.center.z + s.radius * std::cos(deg2rad(a)), s.center.r + s.radius * std::sin(deg2rad(a)));
	}
}

// ---- Intérprete ------------------------------------------------------------

enum CornerKind { CORNER_NONE = 0, CORNER_ROUND, CORNER_CHAMFER };

class Interpreter {
public:
	Interpreter(int standard) : d(dialect_for(standard)) {
		pos = cnc_initial_position();
	}

	CncPath run(const std::string &text) {
		size_t i = 0;
		int line = 0;
		while (i <= text.size() && !ended) {
			size_t eol = text.find('\n', i);
			if (eol == std::string::npos) eol = text.size();
			size_t end = eol;
			if (end > i && text[end - 1] == '\r') --end;
			process_line(text.substr(i, end - i), line);
			if (eol >= text.size()) break;
			i = eol + 1;
			++line;
		}
		return path;
	}

private:
	Dialect d;
	CncPath path;
	CncPoint pos;
	int motion = 0;             // G modal: 0, 1, 2, 3 o 33 (rosca)
	bool incremental = false;   // G91 (Fagor)
	bool started = false;
	bool ended = false;
	int tool_t = 0, tool_d = 0; // última T y último corrector: la herramienta es T, o D si T es 0
	int comp = 0;               // G40 / G41 / G42 (modal)

	// "A1 A2" sin cota: se resuelve con el bloque siguiente
	bool pending_angles = false;
	double pend_a1 = 0, pend_a2 = 0;
	int pend_line = 0;

	// G36/G39 o ",R"/",C": empalme en la esquina final del último tramo
	CornerKind pending_corner = CORNER_NONE;
	double corner_size = 0;
	size_t corner_segment = 0;

	void message(int line, bool error, const std::string &text) {
		CncMessage m;
		m.line = line;
		m.error = error;
		m.text = text;
		path.messages.push_back(m);
	}

	// --- encabezado del generador: "T4  70  110" y "NOMBRE largo d1 d2" ---
	static std::vector<std::string> split_words(const std::string &s) {
		std::vector<std::string> out;
		size_t i = 0;
		while (i < s.size()) {
			while (i < s.size() && (is_space(s[i]) || s[i] == '\r')) ++i;
			size_t b = i;
			while (i < s.size() && !is_space(s[i]) && s[i] != '\r') ++i;
			if (i > b) out.push_back(s.substr(b, i - b));
		}
		return out;
	}

	static bool whole_number(const std::string &w, double &v) {
		size_t i = 0;
		return parse_number(w, i, v) && i == w.size();
	}

	// true si la línea era una de las dos del generador
	bool generator_line(const std::string &s) {
		std::vector<std::string> w = split_words(s);
		double a, b, c;
		if (w.size() == 3 && upper(w[0][0]) == 'T' && w[0].size() > 1 && is_digit(w[0][1])
		    && whole_number(w[1], a) && whole_number(w[2], b)) {
			path.cutoff_tool = atoi(w[0].c_str() + 1);
			if (!path.stock.valid()) {   // "#ODR"/"#IDR" tienen prioridad si aparecen
				path.stock.outer_diameter = std::max(a, b);
				path.stock.inner_diameter = std::min(a, b);
			}
			return true;
		}
		if (w.size() == 4 && !is_digit(w[0][0]) && whole_number(w[1], a) && whole_number(w[2], b) && whole_number(w[3], c)) {
			path.part_name = w[0];
			path.part_length = a;
			return true;
		}
		return false;
	}

	// Línea de encabezado (antes del '%', o comentario con esos datos después)
	void header_text(const std::string &s, size_t i, int line) {
		while (i < s.size() && (is_space(s[i]) || s[i] == ';')) ++i;
		if (i >= s.size()) return;
		if (s[i] == '#') header_line(s, i, line);
		else generator_line(s.substr(i));
	}

	// --- encabezado "#XX= valor" (antes del inicio del programa) ---
	void header_line(const std::string &s, size_t i, int line) {
		++i;   // '#'
		std::string key;
		while (i < s.size() && is_alpha(s[i])) key += upper(s[i++]);
		while (i < s.size() && is_space(s[i])) ++i;
		if (i >= s.size() || s[i] != '=') return;
		++i;
		while (i < s.size() && is_space(s[i])) ++i;
		double v = 0;
		if (!parse_number(s, i, v)) return;
		CncRefLine ref;
		ref.line = line;
		if (key == "DN" || key == "DA") {
			ref.vertical = false;
			ref.nominal = (key == "DN");
			ref.value = v / 2;
			path.refs.push_back(ref);
		} else if (key == "HN" || key == "HA") {
			ref.vertical = true;
			ref.nominal = (key == "HN");
			ref.value = v;
			path.refs.push_back(ref);
		} else if (key == "ODR") {
			path.stock.outer_diameter = v;
		} else if (key == "IDR") {
			path.stock.inner_diameter = v;
		}
	}

	bool is_program_start(const std::string &s, size_t i) const {
		if (s[i] == '%') return true;
		if (d.fanuc && upper(s[i]) == 'O' && i + 1 < s.size() && is_digit(s[i + 1])) return true;
		return false;
	}

	// Sentencias que no generan movimiento y se saltan enteras
	bool is_statement(const std::string &s, size_t i) const {
		if (i >= s.size()) return false;
		if (d.fanuc) {
			if (s[i] == '#') return true;                     // asignación de variable
			std::string kw;
			size_t p = i;
			while (p < s.size() && is_alpha(s[p])) kw += upper(s[p++]);
			return kw == "IF" || kw == "GOTO" || kw == "WHILE" || kw == "END" || kw == "DO";
		}
		// Fagor: "P1=Z", "P2 = K8.500", "P1 = P1 F2 P2"
		if (upper(s[i]) != 'P') return false;
		size_t p = i + 1;
		if (p >= s.size() || !is_digit(s[p])) return false;
		while (p < s.size() && is_digit(s[p])) ++p;
		while (p < s.size() && is_space(s[p])) ++p;
		return p < s.size() && s[p] == '=';
	}

	void process_line(const std::string &s, int line) {
		size_t i = 0;
		while (i < s.size() && is_space(s[i])) ++i;
		if (i >= s.size()) return;

		if (!started) {
			if (is_program_start(s, i)) started = true;
			else header_text(s, i, line);
			return;
		}
		if (s[i] == '%') return;   // fin de cinta FANUC o '%' repetido
		if (d.fanuc && upper(s[i]) == 'O' && i + 1 < s.size() && is_digit(s[i + 1])) return;
		if (s[i] == '(') {
			// Comentario suelto: puede traer el encabezado conservado por la traducción a FANUC
			size_t e = s.find(')', i);
			header_text(s.substr(i + 1, (e == std::string::npos ? s.size() : e) - i - 1), 0, line);
		} else if (s[i] == ';' && d.semicolon_comments) {
			// Ídem con la traducción a 8035, que lo conserva como ";#DN= ..." tras el '%'
			header_text(s, i, line);
			return;
		}

		// Etiqueta de bloque
		if (upper(s[i]) == 'N' && i + 1 < s.size() && is_digit(s[i + 1])) {
			i += 1;
			while (i < s.size() && is_digit(s[i])) ++i;
			while (i < s.size() && is_space(s[i])) ++i;
		}
		if (is_statement(s, i)) return;

		Block b;
		b.line = line;
		tokenize(s, i, d, b);
		if (b.junk) message(line, false, "caracteres que no forman palabra");
		execute(b);
	}

	// Una G que no es de movimiento hace que el bloque no se dibuje (así se
	// comportaba el simulador anterior: G53 X0, G92, G28 U0, G10 ... no mueven)
	bool g_skips_block(int g, bool &known) {
		known = true;
		if (d.fanuc) {
			switch (g) {
			case 10: case 20: case 21: case 22: case 23: case 28: case 30: case 31: case 50:
			case 52: case 53: case 54: case 55: case 56: case 57: case 58: case 59: case 65:
			case 66: case 67: case 92:
				return true;
			}
		} else {
			switch (g) {
			case 10: case 11: case 12: case 13: case 14: case 20: case 21: case 22: case 23:
			case 24: case 25: case 26: case 27: case 28: case 29: case 30: case 31: case 32:
			case 43: case 44: case 47: case 48: case 49: case 50: case 51: case 52: case 53:
			case 54: case 55: case 56: case 57: case 58: case 59: case 74: case 75: case 92:
			case 93:
				return true;
			}
		}
		known = false;
		return false;
	}

	bool g_is_cycle(int g) const {
		if (d.fanuc) return (g >= 70 && g <= 76) || g == 90 || g == 94;
		return g == 66 || g == 67 || g == 68 || g == 69 || (g >= 81 && g <= 89);
	}

	void execute(const Block &b) {
		const int line = b.line;
		bool skip = false, dwell = false, thread = false, ends = false;
		CornerKind corner = CORNER_NONE;
		double corner_value = 0;
		bool has_corner_value = false;

		// Primera pasada: G y M, que cambian cómo se leen las demás palabras
		for (size_t k = 0; k < b.words.size(); ++k) {
			const Word &w = b.words[k];
			if (w.letter == 'G') {
				if (w.bare && d.fanuc) message(line, true, "G sin número: FANUC lo rechaza");
				int g = (int) std::floor(w.value + 0.5);
				switch (g) {
				case 0: case 1: case 2: case 3:
					motion = g;
					break;
				case 4:
					dwell = true;
					break;
				case 40: case 41: case 42:
					comp = (g == 40) ? 0 : g;
					break;
				case 5: case 7: case 94: case 95: case 96: case 97: case 98: case 99:
					break;
				case 32: case 33:
					if (d.fanuc == (g == 32)) { motion = 33; thread = true; }
					else skip = true;
					break;
				case 36: case 39:
					if (d.fanuc) skip = true;
					else corner = (g == 36) ? CORNER_ROUND : CORNER_CHAMFER;
					break;
				case 90: case 91:
					if (d.fanuc) {
						message(line, false, format_line_text("ciclo fijo G%d no soportado: bloque ignorado", g));
						skip = true;
					} else {
						incremental = (g == 91);
					}
					break;
				default: {
					bool known;
					if (g_skips_block(g, known)) { skip = true; break; }
					if (g_is_cycle(g)) {
						message(line, false, format_line_text("ciclo fijo G%d no soportado: bloque ignorado", g));
					} else {
						message(line, false, format_line_text("G%d no reconocida: bloque ignorado", g));
					}
					skip = true;
				}
				}
			} else if (w.letter == 'M') {
				int m = (int) std::floor(w.value + 0.5);
				if (!w.bare && (m == 30 || m == 2)) ends = true;
			} else if (w.letter == 'T' && !w.bare && !w.param) {
				if (d.fanuc) {
					// Tnnmm: herramienta y corrector de dos dígitos
					int v = (int) std::floor(w.value + 0.5);
					tool_t = v / 100;
					tool_d = v % 100;
				} else {
					// Fagor: Tnn.mm (8025) o Tnn (8035, el corrector va en D)
					double ip = std::floor(w.value + 1e-9);
					tool_t = (int) ip;
					int frac = (int) std::floor((w.value - ip) * 100 + 0.5);
					if (frac > 0) tool_d = frac;
				}
			} else if (w.letter == 'D' && d.fagor_8035 && !w.bare && !w.param) {
				tool_d = (int) std::floor(w.value + 0.5);
			}
		}
		if (ends) ended = true;
		if (skip || dwell) return;

		// Segunda pasada: cotas, ángulos, centro y radio
		bool has_z = false, has_r = false, param = false;
		double zv = 0, rv = 0;
		bool has_i = false, has_k = false, has_rad = false;
		double iv = 0, kv = 0, rad = 0;
		std::vector<double> angles;
		for (size_t k = 0; k < b.words.size(); ++k) {
			const Word &w = b.words[k];
			if (w.bare && d.fanuc && w.letter != 'G') message(line, true, format_line_text("letra sin valor (%c): FANUC lo rechaza", w.letter));
			if (w.param) param = true;
			switch (w.letter) {
			case 'X':
				has_r = true;
				rv = incremental ? pos.r + w.value / 2 : w.value / 2;
				break;
			case 'Z':
				has_z = true;
				zv = incremental ? pos.z + w.value : w.value;
				break;
			case 'U':
				if (!d.fanuc) { message(line, true, "letra no reconocida: U"); break; }
				has_r = true;
				rv = pos.r + w.value / 2;
				break;
			case 'W':
				if (!d.fanuc) { message(line, true, "letra no reconocida: W"); break; }
				has_z = true;
				zv = pos.z + w.value;
				break;
			case 'I':
				has_i = true;
				iv = w.value;
				break;
			case 'K':
				has_k = true;
				kv = w.value;
				break;
			case 'R':
				if (w.comma || (d.fanuc && motion <= 1)) {
					corner = CORNER_ROUND;
					corner_value = w.value;
					has_corner_value = true;
				} else {
					has_rad = true;
					rad = w.value;
					if (corner != CORNER_NONE) { corner_value = w.value; has_corner_value = true; }
				}
				break;
			case 'C':
				if (d.fanuc && (w.comma || motion <= 1)) {
					corner = CORNER_CHAMFER;
					corner_value = w.value;
					has_corner_value = true;
				} else if (!d.fanuc) {
					message(line, true, "letra no reconocida: C");
				}
				break;
			case 'A': case 'Q':
				if (w.letter == d.angle) {
					if (angles.size() < 2) angles.push_back(w.value);
					else message(line, true, "más de dos ángulos en el bloque");
				} else if (w.letter == 'Q' && d.fanuc) {
					// Q de ciclos FANUC: sin efecto
				} else {
					message(line, true, format_line_text("letra no reconocida: %c", w.letter));
				}
				break;
			case 'G': case 'M': case 'N': case 'F': case 'S': case 'T': case 'P':
				break;
			case 'D': case 'L': case 'H': case 'O': case 'E': case 'B':
				if (!d.fanuc && !d.fagor_8035) message(line, true, format_line_text("letra no reconocida: %c", w.letter));
				else if (!d.fanuc && w.letter != 'D') message(line, true, format_line_text("letra no reconocida: %c", w.letter));
				break;
			default:
				message(line, true, format_line_text("letra no reconocida: %c", w.letter));
			}
		}
		if (thread) { has_i = has_k = false; }   // en rosca I/K es el paso

		if (!has_z && !has_r && angles.empty()) return;   // sin cotas: no hay movimiento

		CncPoint target = pos;
		if (has_z) target.z = zv;
		if (has_r) target.r = rv;

		// Ángulos
		if (angles.size() == 2 && !has_z && !has_r) {
			pending_angles = true;
			pend_a1 = angles[0];
			pend_a2 = angles[1];
			pend_line = line;
			return;
		}
		if (angles.size() == 1) {
			double a = angles[0];
			Vec dv = dir_of_angle(a);
			if (has_z && has_r) {
				message(line, false, "ángulo con las dos cotas: se ignora el ángulo");
			} else if (has_z) {
				if (std::fabs(dv.z) < 1e-9) { message(line, true, "ángulo perpendicular a Z con cota Z"); return; }
				target.r = pos.r + (target.z - pos.z) * dv.r / dv.z;
			} else if (has_r) {
				if (std::fabs(dv.r) < 1e-9) { message(line, true, "ángulo paralelo a Z con cota X"); return; }
				target.z = pos.z + (target.r - pos.r) * dv.z / dv.r;
			} else {
				message(line, true, "ángulo sin cota");
				return;
			}
		}
		if (angles.size() == 2) {
			CncPoint p;
			if (!intersect_angles(pos, angles[0], target, angles[1], p)) {
				message(line, true, "ángulos paralelos: no hay intersección");
			} else {
				emit_line(p, line, param);
			}
		} else if (pending_angles) {
			pending_angles = false;
			CncPoint p;
			if (!intersect_angles(pos, pend_a1, target, pend_a2, p)) {
				message(pend_line, true, "ángulos paralelos: no hay intersección");
			} else {
				emit_line(p, pend_line, param);
			}
		}

		// Movimiento principal del bloque
		if (motion == 2 || motion == 3) {
			emit_arc(target, line, param, has_rad, rad, has_i || has_k, iv, kv);
		} else {
			emit_line(target, line, param);
		}

		// Empalme en la esquina final, con el tramo siguiente
		if (corner != CORNER_NONE && !has_corner_value) message(line, true, "G36/G39 sin radio");
		if (corner != CORNER_NONE && has_corner_value && !path.segments.empty() && path.segments.back().kind == SEG_LINE) {
			pending_corner = corner;
			corner_size = std::fabs(corner_value);
			corner_segment = path.segments.size() - 1;
		}
	}

	CncSegmentKind line_kind() const { return motion == 0 ? SEG_RAPID : SEG_LINE; }

	int current_tool() const { return tool_t > 0 ? tool_t : tool_d; }

	void emit_line(const CncPoint &to, int line, bool param) {
		CncSegment s;
		s.kind = line_kind();
		s.from = pos;
		s.to = to;
		s.line = line;
		s.unresolved = param;
		s.tool = current_tool();
		s.comp = comp;
		if (param) message(line, false, "cota paramétrica: tramo no resuelto");
		apply_corner(s);
		path.segments.push_back(s);
		pos = to;
	}

	void emit_arc(const CncPoint &to, int line, bool param, bool has_rad, double rad, bool has_ik, double iv, double kv) {
		CncSegment s;
		s.kind = (motion == 2) ? SEG_ARC_CW : SEG_ARC_CCW;
		s.from = pos;
		s.to = to;
		s.line = line;
		s.unresolved = param;
		s.tool = current_tool();
		s.comp = comp;
		if (param) message(line, false, "cota paramétrica: tramo no resuelto");

		bool ok = false;
		if (has_ik) {
			s.center.z = pos.z + kv;
			s.center.r = pos.r + iv;
			s.radius = len(sub(pos, s.center));
			double r2 = len(sub(to, s.center));
			if (s.radius < 1e-9) {
				message(line, true, "arco con centro en el punto inicial");
			} else {
				if (std::fabs(r2 - s.radius) > 0.01 + 0.001 * s.radius)
					message(line, false, "arco: el punto final no está sobre el arco definido por I/K");
				ok = true;
			}
		} else if (has_rad) {
			Vec c = sub(to, pos);
			double L = len(c);
			double R = std::fabs(rad);
			if (L < 1e-9) {
				message(line, true, "arco con R sin desplazamiento");
			} else if (L / 2 > R * 1.001) {
				message(line, true, "arco: el radio es menor que media cuerda");
			} else {
				if (L / 2 > R) R = L / 2;
				double h = std::sqrt(std::fabs(R * R - L * L / 4));
				Vec n = { -c.r / L, c.z / L };   // normal a la izquierda de la cuerda
				bool left = (s.kind == SEG_ARC_CCW);
				if (rad < 0) left = !left;         // R negativo: arco mayor
				CncPoint m;
				m.z = (pos.z + to.z) / 2;
				m.r = (pos.r + to.r) / 2;
				s.center = offset(m, n, left ? h : -h);
				s.radius = R;
				ok = true;
			}
		} else {
			message(line, true, "arco sin R ni I/K");
		}
		if (!ok) {
			s.kind = SEG_LINE;
			s.center = CncPoint();
			s.radius = 0;
		}
		pending_corner = CORNER_NONE;   // un arco no se empalma
		path.segments.push_back(s);
		pos = to;
	}

	// Redondeo o chaflán entre el tramo pendiente y el nuevo tramo `next`
	// (que todavía no está en la lista). Recorta ambos e inserta el empalme.
	void apply_corner(CncSegment &next) {
		CornerKind kind = pending_corner;
		pending_corner = CORNER_NONE;
		if (kind == CORNER_NONE) return;
		if (next.kind != SEG_LINE || corner_segment >= path.segments.size()) return;
		CncSegment &prev = path.segments[corner_segment];
		if (prev.kind != SEG_LINE) return;

		CncPoint c = prev.to;
		Vec v1 = sub(c, prev.from), v2 = sub(next.to, c);
		double l1 = len(v1), l2 = len(v2);
		if (l1 < 1e-9 || l2 < 1e-9) return;
		Vec d1 = { v1.z / l1, v1.r / l1 }, d2 = { v2.z / l2, v2.r / l2 };
		double cr = cross(d1, d2), dt = dot(d1, d2);
		if (std::fabs(cr) < 1e-9) return;   // tramos alineados: no hay esquina

		double cut = corner_size;
		if (kind == CORNER_ROUND) cut = corner_size * std::fabs(cr) / (1 + dt);   // R·tan(θ/2)
		if (cut > l1 + 1e-9 || cut > l2 + 1e-9) {
			message(prev.line, true, "redondeo o chaflán mayor que los tramos que une");
			return;
		}
		CncPoint t1 = offset(c, d1, -cut), t2 = offset(c, d2, cut);
		CncSegment blend;
		blend.line = prev.line;
		blend.from = t1;
		blend.to = t2;
		if (kind == CORNER_ROUND) {
			blend.kind = (cr > 0) ? SEG_ARC_CCW : SEG_ARC_CW;
			Vec n1 = { -d1.r, d1.z };   // normal a la izquierda del primer tramo
			blend.center = offset(t1, n1, (cr > 0) ? corner_size : -corner_size);
			blend.radius = corner_size;
		} else {
			blend.kind = SEG_LINE;
		}
		prev.to = t1;
		next.from = t2;
		path.segments.push_back(blend);
	}
};

} // namespace

// ---- API --------------------------------------------------------------------

void CncBounds::add(double z, double r) {
	if (!valid) {
		zmin = zmax = z;
		rmin = rmax = r;
		valid = true;
		return;
	}
	if (z < zmin) zmin = z;
	if (z > zmax) zmax = z;
	if (r < rmin) rmin = r;
	if (r > rmax) rmax = r;
}

void CncBounds::add(const CncPoint &p) { add(p.z, p.r); }

CncBounds CncPath::feed_bounds() const {
	CncBounds b;
	for (size_t k = 0; k < segments.size(); ++k) {
		const CncSegment &s = segments[k];
		if (s.kind == SEG_RAPID || s.unresolved) continue;
		if (s.is_arc()) arc_bounds(s, b);
		else { b.add(s.from); b.add(s.to); }
	}
	return b;
}

bool CncPath::has_errors() const {
	for (size_t k = 0; k < messages.size(); ++k) if (messages[k].error) return true;
	return false;
}

CncPoint cnc_initial_position() {
	CncPoint p;
	p.z = 830;
	p.r = 200;
	return p;
}

CncPath interpret_cnc(const std::string &text, int standard) {
	Interpreter in(standard);
	return in.run(text);
}
