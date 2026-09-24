// Herramienta de depuración: interpreta un programa y lista sus tramos y avisos,
// y luego simula la pieza y lista los vértices de su sección.
// Uso: cncpath_dump <archivo> [8025|8035|fanuc] [línea]   (por defecto 8025 y el
// programa completo; con línea, simula la pieza solo hasta esa línea, desde 1)
#include "../CncPath.h"
#include "../TurnStock.h"
#include <cstdio>
#include <cstring>
#include <fstream>
#include <sstream>
#include <string>

int main(int argc, char **argv) {
	if (argc < 2) {
		fprintf(stderr, "uso: %s <archivo> [8025|8035|fanuc]\n", argv[0]);
		return 2;
	}
	int standard = FAGOR_8025;
	if (argc > 2) {
		if (!strcmp(argv[2], "8035")) standard = WAS_8035;
		else if (!strcmp(argv[2], "fanuc")) standard = KIA_FANUC;
	}
	std::ifstream in(argv[1], std::ios::binary);
	if (!in) {
		fprintf(stderr, "no se pudo abrir %s\n", argv[1]);
		return 2;
	}
	std::stringstream ss;
	ss << in.rdbuf();
	CncPath p = interpret_cnc(ss.str(), standard);

	static const char *names[] = { "RAPIDO", "RECTA ", "ARC CW", "ARC CCW" };
	for (size_t k = 0; k < p.segments.size(); ++k) {
		const CncSegment &s = p.segments[k];
		printf("L%-4d %-7s X%9.3f Z%9.3f -> X%9.3f Z%9.3f", s.line + 1, names[s.kind], s.from.x(), s.from.z, s.to.x(), s.to.z);
		if (s.is_arc()) printf("  centro X%9.3f Z%9.3f R%7.3f", s.center.x(), s.center.z, s.radius);
		if (s.unresolved) printf("  (no resuelto)");
		printf("\n");
	}
	for (size_t k = 0; k < p.refs.size(); ++k) {
		const CncRefLine &r = p.refs[k];
		printf("REF  %s %s = %.3f\n", r.vertical ? "Z" : "X", r.nominal ? "nominal " : "auxiliar", r.vertical ? r.value : r.value * 2);
	}
	if (p.stock.valid()) printf("BRUTO exterior %.3f interior %.3f\n", p.stock.outer_diameter, p.stock.inner_diameter);
	CncBounds b = p.feed_bounds();
	if (b.valid) printf("AVANCES Z [%.3f, %.3f]  X [%.3f, %.3f]\n", b.zmin, b.zmax, 2 * b.rmin, 2 * b.rmax);
	for (size_t k = 0; k < p.messages.size(); ++k)
		printf("%s linea %d: %s\n", p.messages[k].error ? "ERROR" : "AVISO", p.messages[k].line + 1, p.messages[k].text.c_str());
	printf("%zu tramos, %zu avisos\n", p.segments.size(), p.messages.size());

	ToolTable tools = default_tool_table();
	StockDefinition st = stock_from_path(p, tools);
	if (!st.valid()) {
		printf("SIN BRUTO: no se simula la pieza\n");
		return 0;
	}
	printf("--- pieza: bruto X%.3f / X%.3f%s, Z de %.3f a %.3f\n", st.outer_diameter, st.inner_diameter,
	       st.deduced ? " (estimado)" : "", st.z_face, st.z_end);
	int up_to = (argc > 3) ? atoi(argv[3]) - 1 : -1;
	TurnStock s = simulate_stock(p, st, tools, up_to);
	for (size_t k = 0; k < s.rings.size(); ++k) {
		printf("ANILLO %zu (%zu vertices)\n", k + 1, s.rings[k].size());
		for (size_t i = 0; i < s.rings[k].size(); ++i) printf("   Z%11.5f  X%11.5f\n", s.rings[k][i].z, s.rings[k][i].x());
	}
	for (size_t k = 0; k < s.assumed.size(); ++k)
		printf("RANURA de ancho supuesto en Z [%.3f, %.3f] (linea %d, T%d)\n", s.assumed[k].z_from, s.assumed[k].z_to, s.assumed[k].line + 1, s.assumed[k].tool);
	for (size_t k = 0; k < s.messages.size(); ++k)
		printf("%s linea %d: %s\n", s.messages[k].error ? "ERROR" : "AVISO", s.messages[k].line + 1, s.messages[k].text.c_str());
	printf("area %.4f, tronzado: %s\n", s.area(), s.cut_off ? "si" : "no");
	return 0;
}
