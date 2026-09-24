// Tests del simulador de pieza (TurnStock). Se ejecutan con: ctest --test-dir build
#include "../TurnStock.h"
#include <cmath>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>

static int failures = 0;
#define CHECK(cond) do { if (!(cond)) { ++failures; fprintf(stderr, "FALLO %s:%d: %s\n", __FILE__, __LINE__, #cond); } } while (0)
#define CHECK_NEAR(a, b) do { if (std::fabs((a) - (b)) > 1e-3) { ++failures; fprintf(stderr, "FALLO %s:%d: %s = %g, esperado %g\n", __FILE__, __LINE__, #a, (double) (a), (double) (b)); } } while (0)

static std::string read_file(const std::string &path) {
	std::ifstream in(path.c_str(), std::ios::binary);
	std::stringstream ss;
	ss << in.rdbuf();
	return ss.str();
}

static bool has_vertex(const TurnStock &s, double z, double r) {
	std::vector<CncPoint> v = s.vertices();
	for (size_t k = 0; k < v.size(); ++k) if (std::fabs(v[k].z - z) < 1e-3 && std::fabs(v[k].r - r) < 1e-3) return true;
	return false;
}

static void dump(const TurnStock &s) {
	for (size_t k = 0; k < s.rings.size(); ++k) {
		fprintf(stderr, "  anillo %zu:", k + 1);
		for (size_t i = 0; i < s.rings[k].size(); ++i) fprintf(stderr, " (%.4f, %.4f)", s.rings[k][i].z, s.rings[k][i].r);
		fprintf(stderr, "\n");
	}
}

static void print_messages(const TurnStock &s) {
	for (size_t k = 0; k < s.messages.size(); ++k)
		fprintf(stderr, "  linea %d: %s%s\n", s.messages[k].line + 1, s.messages[k].error ? "ERROR " : "", s.messages[k].text.c_str());
}

static StockDefinition tube(double od, double id, double z_face, double z_end) {
	StockDefinition s;
	s.outer_diameter = od;
	s.inner_diameter = id;
	s.z_face = z_face;
	s.z_end = z_end;
	return s;
}

int main(int argc, char **argv) {
	std::string dir = argc > 1 ? argv[1] : "tests";
	ToolTable tools = default_tool_table();

	// 1. Barra maciza: una pasada exterior deja un escalón exacto
	{
		CncPath p = interpret_cnc("%1\nN10 T0.02\nN20 G00 X40 Z2\nN30 G01 Z-10\nN40 G00 X60\nN50 Z2\n", FAGOR_8025);
		TurnStock s = simulate_stock(p, tube(50, 0, 0, -30), tools);
		print_messages(s);
		CHECK(s.messages.empty());
		CHECK(s.rings.size() == 1);
		CHECK_NEAR(s.area(), 25 * 30 - 5 * 10);
		CHECK(has_vertex(s, 0, 20)); CHECK(has_vertex(s, -10, 20)); CHECK(has_vertex(s, -10, 25)); CHECK(has_vertex(s, -30, 25));
		CHECK(!s.cut_off);
		CHECK_NEAR(s.tool_position.z, 2);
	}

	// 2. Frenteado con T6 sobre un bruto con demasía en la cara
	{
		CncPath p = interpret_cnc("%1\nN10 T0.06\nN20 G00 X60 Z0\nN30 G01 X0\nN40 G00 Z10\n", FAGOR_8025);
		TurnStock s = simulate_stock(p, tube(50, 0, 2, -30), tools);
		CHECK(s.messages.empty());
		CHECK_NEAR(s.area(), 25 * 30);
		if (std::fabs(s.area() - 750) > 1e-3) dump(s);
		CHECK(has_vertex(s, 0, 25)); CHECK(has_vertex(s, 0, 0));
	}

	// 3. Mandrinado con T3 en un tubo: el radio interior crece solo en el tramo recorrido
	{
		CncPath p = interpret_cnc("%1\nN10 T0.03\nN20 G00 X72.381 Z1.5\nN30 G01 Z-8.6\nN40 G00 X70\nN50 Z10\n", FAGOR_8025);
		TurnStock s = simulate_stock(p, tube(110, 70, 0, -20), tools);
		print_messages(s);
		CHECK(s.messages.empty());
		CHECK_NEAR(s.area(), 20 * 20 - (36.1905 - 35) * 8.6);
		CHECK(has_vertex(s, -8.6, 36.1905)); CHECK(has_vertex(s, -8.6, 35)); CHECK(has_vertex(s, 0, 36.1905));
	}

	// 4. Tronzado con T4 a través de la pared: queda solo la pieza
	{
		CncPath p = interpret_cnc("%1\nN10 T0.04\nN20 G00 X118 Z-6\nN30 G01 X60\nN40 G00 X118\nN50 Z40\n", FAGOR_8025);
		TurnStock s = simulate_stock(p, tube(110, 70, 0, -30), tools);
		print_messages(s);
		CHECK(s.messages.empty());
		CHECK(s.cut_off); CHECK_NEAR(s.cut_z, -6);
		CHECK(s.rings.size() == 1);
		CHECK_NEAR(s.area(), 20 * 6);
		CHECK(has_vertex(s, -6, 55)); CHECK(has_vertex(s, -6, 35)); CHECK(has_vertex(s, 0, 55));
		CHECK(s.assumed.empty());
	}

	// 5. Ranura con T1 que no atraviesa: ancho supuesto (3 mm hacia el plato)
	{
		CncPath p = interpret_cnc("%1\nN10 T0.01\nN20 G00 X118 Z-3\nN30 G01 X100\nN40 G00 X118\n", FAGOR_8025);
		TurnStock s = simulate_stock(p, tube(110, 70, 0, -30), tools);
		CHECK(s.messages.empty());
		CHECK(!s.cut_off);
		CHECK_NEAR(s.area(), 20 * 30 - 3 * 5);
		CHECK(s.assumed.size() == 1);
		if (!s.assumed.empty()) { CHECK_NEAR(s.assumed[0].z_from, -6); CHECK_NEAR(s.assumed[0].z_to, -3); }
		CHECK(has_vertex(s, -3, 50)); CHECK(has_vertex(s, -6, 50)); CHECK(has_vertex(s, -6, 55));
		// Con un ancho definido para T1 la ranura cambia
		ToolTable t2 = tools;
		t2.tools[1].width = 2;
		TurnStock s2 = simulate_stock(p, tube(110, 70, 0, -30), t2);
		CHECK_NEAR(s2.area(), 20 * 30 - 2 * 5);
	}

	// 6. Rápido dentro del material: aviso al entrar y al salir; rápido por la cara (Z0) o por aire: nada
	{
		CncPath p = interpret_cnc("%1\nN10 T0.02\nN20 G00 X108.3\nN30 Z0\nN40 G00 X100 Z-5\nN50 G00 Z20\n", FAGOR_8025);
		TurnStock s = simulate_stock(p, tube(110, 70, 0, -30), tools);
		print_messages(s);
		CHECK(s.messages.size() == 2);
		if (s.messages.size() == 2) {
			CHECK(s.messages[0].line == 4); CHECK(s.messages[0].error);
			CHECK(s.messages[1].line == 5);
		}
	}

	// 7. Arco: un redondeo G03 deja vértices sobre el arco y quita el área correcta
	{
		CncPath p = interpret_cnc("%1\nN10 T0.02\nN20 G00 X40 Z2\nN30 G01 Z-5\nN40 G03 X50 Z-10 R5\nN50 G00 X60\nN60 Z5\n", FAGOR_8025);
		TurnStock s = simulate_stock(p, tube(50, 0, 0, -30), tools);
		print_messages(s);
		CHECK(s.messages.empty());
		// Rectángulo 5x5 (z -5..0, r 20..25) menos el cuadrante que queda bajo el arco de radio 5 con centro (-10, 20)... el arco va de (-5,20) a (-10,25):
		// área quitada = 5*5 (de 0 a -5) + (25 - pi*25/4) (esquina cóncava entre -5 y -10)
		double removed = 25 + (25 - 3.14159265358979 * 25 / 4);
		CHECK(std::fabs(s.area() - (25 * 30 - removed)) < 0.05);   // el arco se muestrea en cuerdas: error de área < 0,05
		CHECK(has_vertex(s, -5, 20)); CHECK(has_vertex(s, -10, 25));
	}

	// 8. Simulación por línea: hasta la línea del primer avance no hay corte todavía
	{
		CncPath p = interpret_cnc("%1\nN10 T0.02\nN20 G00 X40 Z2\nN30 G01 Z-10\nN40 G00 X60\n", FAGOR_8025);
		TurnStock s0 = simulate_stock(p, tube(50, 0, 0, -30), tools, 2);
		CHECK_NEAR(s0.area(), 25 * 30);
		CHECK_NEAR(s0.tool_position.r, 20);
		TurnStock s1 = simulate_stock(p, tube(50, 0, 0, -30), tools, 3);
		CHECK_NEAR(s1.area(), 25 * 30 - 50);
	}

	// 9. Programa real P05A: bruto del encabezado, sin rápidos en el material, pieza tronzada con sus cotas
	{
		CncPath p = interpret_cnc(read_file(dir + "/P05A.NC"), FAGOR_8025);
		CHECK(p.cutoff_tool == 4);
		CHECK_NEAR(p.part_length, 6);
		CHECK(p.part_name == "P05A");
		StockDefinition st = stock_from_path(p, tools);
		CHECK(st.valid()); CHECK(!st.deduced);
		CHECK_NEAR(st.outer_diameter, 110); CHECK_NEAR(st.inner_diameter, 70); CHECK_NEAR(st.z_face, 0);
		CHECK(st.z_end < -8.9);
		TurnStock s = simulate_stock(p, st, tools);
		print_messages(s);
		CHECK(s.messages.empty());
		CHECK(s.cut_off); CHECK_NEAR(s.cut_z, -6);
		CHECK(s.rings.size() == 1);
		// Perfil terminado: chaflán exterior entrando en Z0, cilindro X100.3, cono Q210 cortado en Z-6, agujero X93.381
		CHECK(has_vertex(s, 0, 48.7065 + 0.6 * (50.15 - 48.7065)));   // el chaflán entra al material en Z0
		CHECK(has_vertex(s, -1, 50.15));
		CHECK(has_vertex(s, -5, 50.15));
		CHECK(has_vertex(s, -6, 50.15 - 1.0 * std::tan(30 * 3.14159265358979 / 180)));
		CHECK(has_vertex(s, -6, 46.6905));
		CHECK(has_vertex(s, 0, 46.6905));
		std::vector<CncPoint> v = s.vertices();
		for (size_t k = 0; k < v.size(); ++k) { CHECK(v[k].z >= -6 - 1e-6); CHECK(v[k].z <= 1e-6); }
	}

	// 10. Sin encabezado (O0001): bruto estimado, con el interior en el fondo del tronzado; la pieza queda separada
	{
		CncPath p = interpret_cnc(read_file(dir + "/O0001.NC"), FAGOR_8025);
		StockDefinition st = stock_from_path(p, tools);
		CHECK(st.valid()); CHECK(st.deduced);
		CHECK_NEAR(st.inner_diameter, 19);    // G01 X19.000 con T04.04
		CHECK_NEAR(st.outer_diameter, 48);    // mayor avance X46 más 1 mm de radio
		TurnStock s = simulate_stock(p, st, tools);
		print_messages(s);
		CHECK(s.messages.empty());
		CHECK(s.cut_off); CHECK_NEAR(s.cut_z, -10);
		CHECK(s.rings.size() == 1);
		CHECK(has_vertex(s, -9.5, 20)); CHECK(has_vertex(s, -10, 20 - 0.5 * std::tan(30 * 3.14159265358979 / 180)));   // cilindro X40 y cono A210 cortado en Z-10
	}

	if (failures == 0) printf("OK: todos los tests del simulador de pieza pasaron\n");
	return failures == 0 ? 0 : 1;
}
