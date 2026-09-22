// Tests del clasificador léxico del editor. Se ejecutan con: ctest --test-dir build
#include "../CncSyntax.h"
#include <cstdio>
#include <string>
#include <vector>

static int failures = 0;
#define CHECK(cond) do { if (!(cond)) { ++failures; fprintf(stderr, "FALLO %s:%d: %s\n", __FILE__, __LINE__, #cond); } } while (0)

// Estilo del primer carácter de `word` dentro de `text` (o -1 si no está)
static int style_of(const std::string &text, const std::vector<unsigned char> &st, const std::string &word, size_t from = 0) {
	size_t p = text.find(word, from);
	return p == std::string::npos ? -1 : st[p];
}

// Todos los caracteres de `word` tienen el estilo `style`
static bool whole_word(const std::string &text, const std::vector<unsigned char> &st, const std::string &word, int style) {
	size_t p = text.find(word);
	if (p == std::string::npos) return false;
	for (size_t k = 0; k < word.size(); ++k) if (st[p + k] != style) return false;
	return true;
}

int main() {
	// 1. 8025: cabecera previa al % como comentario, % como inicio, palabras y comentario entre paréntesis
	{
		std::string t = "Cliente: RIGOLLEAU\nSubItem: 2-339 - VITON\n%00001\nN0010  T0.02 M03 (Frente) G04 K0.3\nN0020  MM5 X10.5 Z-8.900\n";
		std::vector<unsigned char> st = classify_cnc(t, FAGOR_8025);
		CHECK(st.size() == t.size());
		CHECK(whole_word(t, st, "Cliente: RIGOLLEAU", STYLE_COMMENT));
		CHECK(whole_word(t, st, "VITON", STYLE_COMMENT));          // cabecera: no se colorea la T
		CHECK(whole_word(t, st, "%00001", STYLE_HEADER));
		CHECK(whole_word(t, st, "N0010", STYLE_LABEL));
		CHECK(whole_word(t, st, "T0.02", STYLE_TOOL));
		CHECK(whole_word(t, st, "M03", STYLE_M));
		CHECK(whole_word(t, st, "(Frente)", STYLE_COMMENT));
		CHECK(whole_word(t, st, "G04", STYLE_TEXT));
		CHECK(whole_word(t, st, "K0.3", STYLE_TEXT));
		CHECK(whole_word(t, st, "X10.5", STYLE_TEXT));
		CHECK(whole_word(t, st, "Z-8.900", STYLE_TEXT));
		CHECK(style_of(t, st, "MM5") == STYLE_TEXT);               // letra seguida de letra: no es palabra
	}

	// 2. Dentro del programa, las letras interiores de una palabra no se colorean
	{
		std::string t = "%1\nN0010 (VITON) PPOSZ GOTO90\n";
		std::vector<unsigned char> st = classify_cnc(t, FAGOR_8025);
		CHECK(whole_word(t, st, "(VITON)", STYLE_COMMENT));
		CHECK(whole_word(t, st, "PPOSZ", STYLE_TEXT));
		CHECK(whole_word(t, st, "GOTO90", STYLE_TEXT));
	}

	// 3. 8035: comentario con ';', paréntesis como sentencia normal, corrector D
	{
		std::string t = "%P12EXP_22049,MX--,\nN0010  G55\nN0020  T2 D05\nN0030  ;(  Desbaste ext  ) M04\nN0040  (P100=PPOSZ)\n";
		std::vector<unsigned char> st = classify_cnc(t, WAS_8035);
		CHECK(whole_word(t, st, "%P12EXP_22049,MX--,", STYLE_HEADER));
		CHECK(whole_word(t, st, "T2", STYLE_TOOL));
		CHECK(whole_word(t, st, "D05", STYLE_TOOL));
		CHECK(whole_word(t, st, ";(  Desbaste ext  ) M04", STYLE_COMMENT));   // M04 dentro del comentario
		CHECK(whole_word(t, st, "(P100=PPOSZ)", STYLE_TEXT));
	}

	// 4. FANUC: "Onnnn" inicia el programa, comentarios entre paréntesis, T0202, GOTO e IF sin colorear
	{
		std::string t = "O0001\nN0010  #502 = -538 (LIMITE Z)\nN0050  T0202\nIF[#503 GT #502] GOTO90\nM30\n%\n";
		std::vector<unsigned char> st = classify_cnc(t, KIA_FANUC);
		CHECK(whole_word(t, st, "O0001", STYLE_HEADER));
		CHECK(whole_word(t, st, "#502", STYLE_TEXT));
		CHECK(whole_word(t, st, "(LIMITE Z)", STYLE_COMMENT));
		CHECK(whole_word(t, st, "T0202", STYLE_TOOL));
		CHECK(whole_word(t, st, "IF[#503 GT #502] GOTO90", STYLE_TEXT));
		CHECK(whole_word(t, st, "M30", STYLE_M));
		CHECK(st[t.rfind('%')] == STYLE_HEADER);                       // fin de cinta
	}

	// 5. Sin inicio de programa, todo es cabecera; texto vacío no falla
	{
		std::string t = "N0010 G00\nN0020 M30\n";
		std::vector<unsigned char> st = classify_cnc(t, FAGOR_8025);
		CHECK(whole_word(t, st, "N0010 G00", STYLE_COMMENT));
		CHECK(classify_cnc("", WAS_8035).empty());
	}

	if (failures == 0) printf("OK: todos los tests de sintaxis pasaron\n");
	return failures == 0 ? 0 : 1;
}
