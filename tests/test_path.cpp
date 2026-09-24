// Tests del intérprete de trayectorias. Se ejecutan con: ctest --test-dir build
// Recibe la carpeta tests/ para leer los programas reales.
#include "../CncPath.h"
#include <cmath>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

static int failures = 0;
#define CHECK(cond) do { if (!(cond)) { ++failures; fprintf(stderr, "FALLO %s:%d: %s\n", __FILE__, __LINE__, #cond); } } while (0)
#define CHECK_NEAR(a, b) do { if (std::fabs((a) - (b)) > 1e-3) { ++failures; fprintf(stderr, "FALLO %s:%d: %s = %g, esperado %g\n", __FILE__, __LINE__, #a, (double) (a), (double) (b)); } } while (0)

static std::string read_file(const std::string &path) {
	std::ifstream in(path.c_str(), std::ios::binary);
	std::stringstream ss;
	ss << in.rdbuf();
	return ss.str();
}

// Segmentos de avance (rectas y arcos, sin rápidos)
static std::vector<CncSegment> feeds(const CncPath &p) {
	std::vector<CncSegment> out;
	for (size_t k = 0; k < p.segments.size(); ++k) if (p.segments[k].kind != SEG_RAPID) out.push_back(p.segments[k]);
	return out;
}

static int count_errors(const CncPath &p) {
	int n = 0;
	for (size_t k = 0; k < p.messages.size(); ++k) if (p.messages[k].error) ++n;
	return n;
}

static void print_messages(const CncPath &p) {
	for (size_t k = 0; k < p.messages.size(); ++k)
		fprintf(stderr, "  linea %d: %s%s\n", p.messages[k].line + 1, p.messages[k].error ? "ERROR " : "", p.messages[k].text.c_str());
}

static bool same_point(const CncPoint &a, const CncPoint &b) {
	return std::fabs(a.z - b.z) < 1e-3 && std::fabs(a.r - b.r) < 1e-3;
}

int main(int argc, char **argv) {
	std::string dir = argc > 1 ? argv[1] : "tests";

	// 1. 8025 básico: encabezado, rápidos y rectas, X en diámetro, cotas modales, letra sin número
	{
		std::string t =
			"Cliente: PRUEBA\n#DN= 100.000\n#DA= 93.081\n#HN= -6.000\n#ODR= 110.000\n#IDR= 70.000\n"
			"%00001\nN0010 G00 X50 Z2\nN0020 G01 Z-10\nN0030 X60\nN0040 GZ5\nN0050 M30\nN0060 G01 X10\n";
		CncPath p = interpret_cnc(t, FAGOR_8025);
		print_messages(p);
		CHECK(count_errors(p) == 0);
		CHECK(p.refs.size() == 3);
		CHECK(!p.refs[0].vertical && p.refs[0].nominal); CHECK_NEAR(p.refs[0].value, 50);
		CHECK(!p.refs[1].vertical && !p.refs[1].nominal); CHECK_NEAR(p.refs[1].value, 46.5405);
		CHECK(p.refs[2].vertical && p.refs[2].nominal); CHECK_NEAR(p.refs[2].value, -6);
		CHECK(p.stock.valid()); CHECK_NEAR(p.stock.outer_diameter, 110); CHECK_NEAR(p.stock.inner_diameter, 70);
		CHECK(p.segments.size() == 4);   // el G01 X10 después de M30 no cuenta
		CHECK(p.segments[0].kind == SEG_RAPID);
		CHECK(same_point(p.segments[0].from, cnc_initial_position()));
		CHECK_NEAR(p.segments[0].to.r, 25); CHECK_NEAR(p.segments[0].to.z, 2);
		CHECK(p.segments[0].line == 7);
		CHECK(p.segments[1].kind == SEG_LINE); CHECK_NEAR(p.segments[1].to.z, -10); CHECK_NEAR(p.segments[1].to.r, 25);
		CHECK(p.segments[2].kind == SEG_LINE); CHECK_NEAR(p.segments[2].to.r, 30); CHECK_NEAR(p.segments[2].to.z, -10);
		CHECK(p.segments[3].kind == SEG_RAPID); CHECK_NEAR(p.segments[3].to.z, 5); CHECK_NEAR(p.segments[3].to.r, 30);   // GZ5 = G00 Z5
		CncBounds b = p.feed_bounds();
		CHECK(b.valid); CHECK_NEAR(b.zmin, -10); CHECK_NEAR(b.zmax, 2); CHECK_NEAR(b.rmin, 25); CHECK_NEAR(b.rmax, 30);
	}

	// 2. Ángulos: A con Z, A con X, A1 A2 diferido; G91/G90
	{
		std::string t =
			"%1\nG00 X169.3 Z0\nG01 A225 Z-3.350\n"      // 11K1: baja 6.7 en diámetro
			"G00 X37.690 Z1.5\nG01 A150.000 X40.000\n"   // O0001: chaflán de entrada
			"G00 X0 Z0\nG01 A0 A90\nX20 Z10\n"           // A1 A2: esquina en (z=10, r=0)
			"G91 G00 Z5\nX-4\nG90 X20\n";
		CncPath p = interpret_cnc(t, FAGOR_8025);
		print_messages(p);
		CHECK(count_errors(p) == 0);
		CHECK(p.segments.size() == 10);
		CHECK_NEAR(p.segments[1].to.r, 81.3); CHECK_NEAR(p.segments[1].to.z, -3.35);
		CHECK_NEAR(p.segments[3].to.r, 20); CHECK_NEAR(p.segments[3].to.z, -0.5);
		CHECK(p.segments[5].kind == SEG_LINE); CHECK_NEAR(p.segments[5].to.z, 10); CHECK_NEAR(p.segments[5].to.r, 0);
		CHECK(p.segments[5].line == 6);        // el tramo hasta la intersección pertenece a la línea "A0 A90"
		CHECK(p.segments[6].line == 7); CHECK_NEAR(p.segments[6].to.z, 10); CHECK_NEAR(p.segments[6].to.r, 10);
		CHECK_NEAR(p.segments[7].to.z, 15); CHECK_NEAR(p.segments[7].to.r, 10);    // G91 Z5
		CHECK_NEAR(p.segments[8].to.r, 8);                                          // X-4 incremental en diámetro
		CHECK(p.segments[8].kind == SEG_RAPID);
		CHECK_NEAR(p.segments[9].to.r, 10); CHECK_NEAR(p.segments[9].to.z, 15);     // G90: X vuelve a ser absoluto
	}

	// 3. Arcos por I/K (I en radio) y por R; sentido; error si R es menor que media cuerda
	{
		std::string t =
			"%1\nG00 X162.6 Z-3.35\nG03 X160.000 Z-3.868 I-1.250 K1.250\n"   // 11K1
			"G00 X164.925 Z-2.305\nG03 X161.925 Z-5.100 R1.8\n"              // polypak
			"G00 X158.925 Z-2.305\nG02 X161.925 Z-5.100 R1.8\n"
			"G00 X0 Z0\nG02 X20 Z0 R1\n";
		CncPath p = interpret_cnc(t, FAGOR_8025);
		print_messages(p);
		CHECK(count_errors(p) == 1);   // solo el último arco
		CHECK(p.segments.size() == 8);
		const CncSegment &a = p.segments[1];
		CHECK(a.kind == SEG_ARC_CCW);
		CHECK_NEAR(a.center.z, -2.1); CHECK_NEAR(a.center.r, 80.05); CHECK_NEAR(a.radius, 1.76777);
		const CncSegment &b = p.segments[3];
		CHECK(b.kind == SEG_ARC_CCW); CHECK_NEAR(b.radius, 1.8);
		CHECK_NEAR(std::hypot(b.center.z - b.from.z, b.center.r - b.from.r), 1.8);
		CHECK_NEAR(std::hypot(b.center.z - b.to.z, b.center.r - b.to.r), 1.8);
		// arco menor antihorario: el centro queda a la izquierda de la cuerda
		{
			double cz = b.to.z - b.from.z, cr = b.to.r - b.from.r;
			double side = cz * (b.center.r - b.from.r) - cr * (b.center.z - b.from.z);
			CHECK(side > 0);
		}
		const CncSegment &c = p.segments[5];
		CHECK(c.kind == SEG_ARC_CW);
		{
			double cz = c.to.z - c.from.z, cr = c.to.r - c.from.r;
			double side = cz * (c.center.r - c.from.r) - cr * (c.center.z - c.from.z);
			CHECK(side < 0);
		}
		CHECK(p.segments[7].kind == SEG_LINE);   // arco imposible: se dibuja recta y se avisa
	}

	// 4. G36 redondeo y G39 chaflán en una esquina a 90°
	{
		std::string t = "%1\nG00 X50 Z0\nG01 G36 R5 Z-20\nX90\nG00 X50 Z0\nG01 G39 R5 Z-20\nX90\n";
		CncPath p = interpret_cnc(t, FAGOR_8025);
		print_messages(p);
		CHECK(count_errors(p) == 0);
		CHECK(p.segments.size() == 8);
		CHECK_NEAR(p.segments[1].to.z, -15); CHECK_NEAR(p.segments[1].to.r, 25);        // recortado
		const CncSegment &arc = p.segments[2];
		CHECK(arc.kind == SEG_ARC_CW); CHECK(arc.line == 2);
		CHECK_NEAR(arc.center.z, -15); CHECK_NEAR(arc.center.r, 30); CHECK_NEAR(arc.radius, 5);
		CHECK_NEAR(arc.to.z, -20); CHECK_NEAR(arc.to.r, 30);
		CHECK_NEAR(p.segments[3].from.r, 30); CHECK_NEAR(p.segments[3].to.r, 45);
		const CncSegment &ch = p.segments[6];
		CHECK(ch.kind == SEG_LINE);
		CHECK_NEAR(ch.from.z, -15); CHECK_NEAR(ch.from.r, 25); CHECK_NEAR(ch.to.z, -20); CHECK_NEAR(ch.to.r, 30);
	}

	// 5. FANUC: inicio con O, U/W incrementales, ",R", variables y sentencias, letra sin valor, ciclo
	{
		std::string t =
			"%\nO0001\nN0010 #502 = -538 (LIMITE Z)\nN0080 G00 G40 W70\nN0110 G10 L2 P1 X0 Z#500\n"
			"G00 X50 Z2\nG01 Z-20 ,R5\nX90\nG01 U10 W-5\nIF[#503 GT #502] GOTO100\nG28 U0\n"
			"G00 Z#500\nGZ5\nG90 X30 Z-10 F0.2\nG02 X100 Z-5 R10\nM30\n%\n";   // el G90 es un ciclo: no mueve
		CncPath p = interpret_cnc(t, KIA_FANUC);
		print_messages(p);
		std::vector<CncSegment> f = feeds(p);
		CHECK(f.size() == 5);   // Z-20 recortado, redondeo, X90, U/W, G02
		CHECK(p.segments[0].kind == SEG_RAPID); CHECK_NEAR(p.segments[0].to.z, 900); CHECK_NEAR(p.segments[0].to.r, 200);
		CHECK_NEAR(f[0].to.z, -15);
		CHECK(f[1].kind == SEG_ARC_CW); CHECK_NEAR(f[1].radius, 5);
		CHECK_NEAR(f[3].to.r, 50); CHECK_NEAR(f[3].to.z, -25);
		CHECK(f[4].kind == SEG_ARC_CW); CHECK_NEAR(f[4].from.z, 5); CHECK_NEAR(f[4].to.z, -5); CHECK_NEAR(f[4].to.r, 50);
		bool unresolved = false, bare = false, cycle = false;
		for (size_t k = 0; k < p.segments.size(); ++k) if (p.segments[k].unresolved) unresolved = true;
		for (size_t k = 0; k < p.messages.size(); ++k) {
			if (p.messages[k].error && p.messages[k].text.find("FANUC") != std::string::npos) bare = true;
			if (p.messages[k].text.find("G90") != std::string::npos) cycle = true;
		}
		CHECK(unresolved); CHECK(bare); CHECK(cycle);
	}

	// 6. 8035: Q como ángulo, comentarios con ';', paramétricas entre paréntesis, D
	{
		std::string t = "%P05A,MX--,\nN0010 D02\nN0020 (P100=PPOSZ)\nN0030 G00 X108.3 Z0 ;(frente)\nN0040 G01 X85.381\nN0050 Q315 Z1\nN0060 ;G01 X0\n";
		CncPath p = interpret_cnc(t, WAS_8035);
		print_messages(p);
		CHECK(count_errors(p) == 0);
		CHECK(p.segments.size() == 3);
		CHECK_NEAR(p.segments[2].to.z, 1); CHECK_NEAR(p.segments[2].to.r, 42.6905 - 1);
	}

	// 7. Errores que no interrumpen: letra desconocida, ángulo sin cota, arco sin datos
	{
		std::string t = "%1\nG00 X10 Z0\nG01 Y5 Z-1\nA45\nG02 X20 Z-5\nG01 Z-8\n";
		CncPath p = interpret_cnc(t, FAGOR_8025);
		CHECK(count_errors(p) == 3);
		CHECK(p.segments.size() == 4);
		CHECK_NEAR(p.segments.back().to.z, -8);
	}

	// 8. Programas reales: el 8025 y su traducción a 8035 y a FANUC dan los mismos avances
	{
		struct Pair { const char *a; int sa; const char *b; int sb; } pairs[] = {
			{ "P05A.NC", FAGOR_8025, "P05A_35.NC", WAS_8035 },
			{ "O0001.NC", FAGOR_8025, "O0001_FANUC.NC", KIA_FANUC },
		};
		for (size_t n = 0; n < sizeof(pairs) / sizeof(pairs[0]); ++n) {
			std::string ta = read_file(dir + "/" + pairs[n].a), tb = read_file(dir + "/" + pairs[n].b);
			CHECK(!ta.empty()); CHECK(!tb.empty());
			CncPath pa = interpret_cnc(ta, pairs[n].sa), pb = interpret_cnc(tb, pairs[n].sb);
			fprintf(stderr, "%s:\n", pairs[n].a); print_messages(pa);
			fprintf(stderr, "%s:\n", pairs[n].b); print_messages(pb);
			CHECK(count_errors(pa) == 0);
			CHECK(count_errors(pb) == 0);
			std::vector<CncSegment> fa = feeds(pa), fb = feeds(pb);
			CHECK(fa.size() > 5);
			CHECK(fa.size() == fb.size());
			for (size_t k = 0; k < fa.size() && k < fb.size(); ++k) {
				CHECK(fa[k].kind == fb[k].kind);
				CHECK(same_point(fa[k].from, fb[k].from));
				CHECK(same_point(fa[k].to, fb[k].to));
			}
			CncBounds b = pa.feed_bounds();
			CHECK(b.valid && b.zmax <= 2.0 && b.zmin < -5);
		}
		// Detalles de P05A: encabezado y primer tramo de avance (A178 Z-8.900 desde X105.3 Z1.5)
		CncPath p = interpret_cnc(read_file(dir + "/P05A.NC"), FAGOR_8025);
		CHECK(p.refs.size() == 4);
		CHECK_NEAR(p.stock.outer_diameter, 110); CHECK_NEAR(p.stock.inner_diameter, 70);
		std::vector<CncSegment> f = feeds(p);
		CHECK(!f.empty());
		if (!f.empty()) {
			CHECK_NEAR(f[0].from.r, 52.65); CHECK_NEAR(f[0].from.z, 1.5);
			CHECK_NEAR(f[0].to.z, -8.9); CHECK_NEAR(f[0].to.r, 52.65 + (-10.4) * std::tan(178.0 * 3.14159265358979 / 180));
		}
	}

	if (failures == 0) printf("OK: todos los tests del intérprete de trayectorias pasaron\n");
	return failures == 0 ? 0 : 1;
}
