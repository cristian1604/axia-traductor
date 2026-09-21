// Tests del reenumerador. Se ejecutan con: ctest --test-dir build
#include "../Renumber.h"
#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>

static int failures = 0;

#define CHECK(cond) do { if (!(cond)) { ++failures; fprintf(stderr, "FALLO %s:%d: %s\n", __FILE__, __LINE__, #cond); } } while (0)
#define CHECK_EQ(a, b) do { std::string _a = (a), _b = (b); if (_a != _b) { ++failures; fprintf(stderr, "FALLO %s:%d:\n--- esperado ---\n%s\n--- obtenido ---\n%s\n", __FILE__, __LINE__, _b.c_str(), _a.c_str()); } } while (0)

static std::string read_file(const std::string &path) {
	std::ifstream in(path.c_str());
	std::stringstream ss; ss << in.rdbuf();
	return ss.str();
}

// Quita espacios finales de cada línea (los archivos guardados por el CNC los pierden)
static std::string rtrim_lines(const std::string &s) {
	std::string out, line;
	std::istringstream in(s);
	while (std::getline(in, line)) {
		size_t e = line.find_last_not_of(" \t");
		out += (e == std::string::npos) ? "" : line.substr(0, e + 1);
		out += '\n';
	}
	return out;
}

int main(int argc, char **argv) {
	std::string data_dir = argc > 1 ? argv[1] : "tests/data";

	// 1. Programas ya numerados por Num2.exe deben quedar idénticos
	const char *samples[] = { "000001.pit", "000030.pit" };
	for (int i = 0; i < 2; ++i) {
		std::string src = read_file(data_dir + "/" + samples[i]);
		CHECK(!src.empty());
		RenumberResult r = renumber_program(src);
		CHECK(r.ok);
		CHECK_EQ(rtrim_lines(r.text), rtrim_lines(src));
	}

	// 2. Caso básico: cabecera intacta, etiquetas viejas descartadas, líneas vacías y comentarios numerados
	{
		RenumberResult r = renumber_program("(cabecera)\n%PROG\nN0100 G55\n  T02\n\n(coment)\nM30");
		CHECK(r.ok);
		CHECK_EQ(r.text, "(cabecera)\n%PROG\nN0010  G55\nN0020  T02\nN0030  \nN0040  (coment)\nN0050  M30\n");
	}

	// 3. Saltos G25..G29 con destino simple y doble (Na.b) actualizados
	{
		RenumberResult r = renumber_program("%\nN1 G00\nN2 G01\nN3 G29 N1\nN4 G25 N1.2 .3\nN5 G26 N 2 X1\nM30\n");
		CHECK(r.ok);
		CHECK_EQ(r.text, "%\nN0010  G00\nN0020  G01\nN0030  G29 N0010\nN0040  G25 N0010.0020 .3\nN0050  G26 N0020 X1\nN0060  M30\n");
	}

	// 4. Sin '%' no se reenumera
	{
		RenumberResult r = renumber_program("N10 G00\nN20 M30\n");
		CHECK(!r.ok);
		CHECK(r.text.empty());
	}

	// 5. Salto a línea inexistente: error con la lista de destinos
	{
		RenumberResult r = renumber_program("%\nN10 G00\nN20 G29 N999\n");
		CHECK(!r.ok);
		CHECK(r.undefined.size() == 1 && r.undefined[0] == 999);
	}

	// 6. Paso según cantidad de líneas: 800 líneas -> paso 5
	{
		std::string prog = "%\n";
		for (int i = 0; i < 800; ++i) prog += "G01\n";
		RenumberResult r = renumber_program(prog);
		CHECK(r.ok);
		CHECK(r.text.compare(0, 13, "%\nN0010  G01\n") == 0);
		CHECK(r.text.find("N0015  G01\n") != std::string::npos);
		CHECK(r.text.find("N4005  G01\n") != std::string::npos);
	}

	// 7. Última línea sin salto de línea final
	{
		RenumberResult r = renumber_program("%\nG00\nM30");
		CHECK(r.ok);
		CHECK_EQ(r.text, "%\nN0010  G00\nN0020  M30\n");
	}

	if (failures == 0) printf("OK: todos los tests del reenumerador pasaron\n");
	return failures == 0 ? 0 : 1;
}
