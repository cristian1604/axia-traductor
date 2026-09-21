// Tests del traductor contra programas reales. Se ejecutan con: ctest --test-dir build
#include "../TranslatorCore.h"
#include "../Renumber.h"
#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

static int failures = 0;
#define CHECK(cond) do { if (!(cond)) { ++failures; fprintf(stderr, "FALLO %s:%d: %s\n", __FILE__, __LINE__, #cond); } } while (0)

static std::string read_file(const std::string &path) {
	std::ifstream in(path.c_str(), std::ios::binary);
	std::stringstream ss; ss << in.rdbuf();
	return ss.str();
}

// Normaliza como FileManager::readFile: CRLF -> LF
static std::string normalize(const std::string &s) {
	std::string out;
	for (size_t i = 0; i < s.size(); ++i) if (s[i] != '\r') out += s[i];
	return out;
}

static std::vector<std::string> lines_rtrimmed(const std::string &s) {
	std::vector<std::string> v;
	std::istringstream in(s);
	std::string line;
	while (std::getline(in, line)) {
		size_t e = line.find_last_not_of(" \t");
		v.push_back(e == std::string::npos ? "" : line.substr(0, e + 1));
	}
	return v;
}

// Compara desde la línea que contiene `marker` en ambos textos
static void compare_from(const std::string &got, const std::string &expected, const std::string &marker, const char *what) {
	std::vector<std::string> g = lines_rtrimmed(got), e = lines_rtrimmed(expected);
	size_t gi = 0, ei = 0;
	while (gi < g.size() && g[gi].find(marker) == std::string::npos) ++gi;
	while (ei < e.size() && e[ei].find(marker) == std::string::npos) ++ei;
	CHECK(gi < g.size());
	CHECK(ei < e.size());
	bool same = true;
	while (gi < g.size() || ei < e.size()) {
		std::string a = gi < g.size() ? g[gi] : "<fin>";
		std::string b = ei < e.size() ? e[ei] : "<fin>";
		if (a != b) {
			if (same) fprintf(stderr, "FALLO %s: diferencias (obtenido | esperado)\n", what);
			same = false;
			fprintf(stderr, "  %4zu: %-40s | %s\n", gi + 1, a.c_str(), b.c_str());
		}
		++gi; ++ei;
	}
	if (!same) ++failures;
}

int main(int argc, char **argv) {
	std::string dir = argc > 1 ? argv[1] : "tests";
	TranslationSettings defaults;

	// 1. P05A: traducción a 8035 + reenumeración parcial == salida exportada real
	{
		std::string src = normalize(read_file(dir + "/P05A.NC"));
		std::string expected = normalize(read_file(dir + "/P05A_35.NC"));
		CHECK(!src.empty() && !expected.empty());

		std::string translated = translate_8025_to_8035_text(src, defaults);
		RenumberResult r = renumber_program_from(translated, "N0010");
		CHECK(r.ok);
		// La cabecera (línea '%' y comentarios previos) la reescribe el propio
		// control al guardar, por eso se compara desde N0010.
		compare_from(r.text, expected, "N0010", "P05A 8025->8035");
	}

	// 2. Conversiones puntuales
	{
		std::string out = translate_8025_to_8035_text("%1\nN0010 T2.05\nN0020 G04 K0.3\nN0030 A45.5 Z1\nN0040 (coment)\nN0050\n", defaults);
		CHECK(out.find("T2 D05") != std::string::npos);
		CHECK(out.find("G04 K30") != std::string::npos);
		CHECK(out.find("Q45.5 Z1") != std::string::npos);
		CHECK(out.find(";(coment)") != std::string::npos);
		CHECK(out.find("N0050  ;") != std::string::npos);
	}

	// 3. Opciones: eliminar M08 y reemplazo configurable
	{
		TranslationSettings s;
		s.remove_m08 = true;
		s.replace_from = "M03";
		s.replace_to = "M04";
		std::string out = translate_8025_to_8035_text("%1\nN0010 G95 S980 M03 M08\n", s);
		CHECK(out.find("M04") != std::string::npos);
		CHECK(out.find("M08") == std::string::npos);
	}

	// 4. FANUC: O0001.NC traducido y reenumerado == O0001_FANUC.NC (transcripción del
	//    programa deseado, con comentarios en mayúsculas y GOTO a la línea de reinicio)
	{
		std::string src = normalize(read_file(dir + "/O0001.NC"));
		std::string expected = normalize(read_file(dir + "/O0001_FANUC.NC"));
		CHECK(!src.empty() && !expected.empty());
		std::string translated = translate_8025_to_fanuc_text(src, defaults);
		RenumberResult r = renumber_program(translated);
		CHECK(r.ok);
		std::string out = r.text;
		size_t p = out.find('%');
		if (p != std::string::npos) out[p] = 'O';   // como hace MainWindow::enum_lines para FANUC
		compare_from(out, expected, "O0001", "O0001 8025->FANUC");
	}

	// 4b. FANUC con P05A: prólogo con los datos del programa y sin fallos
	{
		std::string src = normalize(read_file(dir + "/P05A.NC"));
		std::string out = translate_8025_to_fanuc_text(src, defaults);
		CHECK(out.find("#501 = 8.500 (++RESTART++)") != std::string::npos);
		CHECK(out.find("N90 #501") != std::string::npos);
		CHECK(out.find("GOTO90") != std::string::npos);
		CHECK(out.find("T0002") != std::string::npos);   // P05A usa T0.02
		CHECK(out.find("M30\n") != std::string::npos);
	}

	// 5. Programas sin estructura esperada no deben fallar
	{
		std::string out = translate_8025_to_fanuc_text("G00 X1\n", defaults);
		CHECK(out.empty());
		out = translate_8025_to_8035_text("", defaults);
		CHECK(out.find('%') == std::string::npos);
	}

	if (failures == 0) printf("OK: todos los tests del traductor pasaron\n");
	return failures == 0 ? 0 : 1;
}
