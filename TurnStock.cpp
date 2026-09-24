#include "TurnStock.h"
#include "clipper2/clipper.h"
#include <cmath>
#include <algorithm>

using namespace Clipper2Lib;

namespace {

const double PI = 3.14159265358979323846;
const int PREC = 5;              // decimales que conserva Clipper2 (0,00001 mm)
const double EPS = 1e-6;
const double RAPID_MARGIN = 0.01; // un rápido "dentro del material" tiene que entrar más que esto

Ring from_path(const PathD &p) {
	Ring r;
	r.reserve(p.size());
	for (size_t k = 0; k < p.size(); ++k) {
		CncPoint c;
		c.z = p[k].x;
		c.r = p[k].y;
		r.push_back(c);
	}
	return r;
}

double cross(const PointD &o, const PointD &a, const PointD &b) {
	return (a.x - o.x) * (b.y - o.y) - (a.y - o.y) * (b.x - o.x);
}

// Casco convexo por cadena monótona
PathD convex_hull(std::vector<PointD> pts) {
	std::sort(pts.begin(), pts.end(), [](const PointD &a, const PointD &b) {
		return a.x < b.x || (a.x == b.x && a.y < b.y);
	});
	pts.erase(std::unique(pts.begin(), pts.end(), [](const PointD &a, const PointD &b) {
		return a.x == b.x && a.y == b.y;
	}), pts.end());
	if (pts.size() < 3) return PathD(pts.begin(), pts.end());
	PathD h(2 * pts.size());
	size_t k = 0;
	for (size_t i = 0; i < pts.size(); ++i) {
		while (k >= 2 && cross(h[k - 2], h[k - 1], pts[i]) <= 0) --k;
		h[k++] = pts[i];
	}
	for (size_t i = pts.size() - 1, t = k + 1; i > 0; --i) {
		while (k >= t && cross(h[k - 2], h[k - 1], pts[i - 1]) <= 0) --k;
		h[k++] = pts[i - 1];
	}
	h.resize(k - 1);
	return h;
}

// Área que barre una forma convexa (relativa a la punta) al ir en línea recta de a hasta b
PathD sweep(const PathD &shape, const CncPoint &a, const CncPoint &c) {
	std::vector<PointD> pts;
	const CncPoint *ends[2] = { &a, &c };
	for (int k = 0; k < 2; ++k) {
		for (size_t i = 0; i < shape.size(); ++i) pts.push_back(PointD(ends[k]->z + shape[i].x, ends[k]->r + shape[i].y));
	}
	return convex_hull(pts);
}

PathD rectangle(double z0, double z1, double r0, double r1) {
	PathD p;
	p.push_back(PointD(z0, r0));
	p.push_back(PointD(z1, r0));
	p.push_back(PointD(z1, r1));
	p.push_back(PointD(z0, r1));
	return p;
}

double angle_of(const CncPoint &c, const CncPoint &p) {
	return std::atan2(p.r - c.r, p.z - c.z);
}

// Distancia de un punto al segmento ab
double dist_to_segment(const PointD &p, const PointD &a, const PointD &b) {
	double dx = b.x - a.x, dy = b.y - a.y;
	double L2 = dx * dx + dy * dy;
	double t = L2 < 1e-18 ? 0 : ((p.x - a.x) * dx + (p.y - a.y) * dy) / L2;
	t = std::max(0.0, std::min(1.0, t));
	double qx = a.x + t * dx - p.x, qy = a.y + t * dy - p.y;
	return std::sqrt(qx * qx + qy * qy);
}

bool inside_with_margin(const PathsD &section, const PointD &p, double margin) {
	int count = 0;
	for (size_t k = 0; k < section.size(); ++k) {
		PointInPolygonResult r = PointInPolygon(p, section[k]);
		if (r == PointInPolygonResult::IsOn) return false;
		if (r == PointInPolygonResult::IsInside) ++count;
	}
	if (count % 2 == 0) return false;
	for (size_t k = 0; k < section.size(); ++k) {
		const PathD &ring = section[k];
		for (size_t i = 0, n = ring.size(); i < n; ++i) {
			if (dist_to_segment(p, ring[i], ring[(i + 1) % n]) < margin) return false;
		}
	}
	return true;
}

class Simulator {
public:
	Simulator(const CncPath &path_, const StockDefinition &stock_, const ToolTable &tools_)
		: path(path_), tools(tools_) {
		out.stock = stock_;
		out.tool_position = cnc_initial_position();
		const StockDefinition &s = out.stock;
		body_size = s.outer_diameter + (s.z_face - s.z_end) + 50;
		section.push_back(rectangle(s.z_end, s.z_face, s.inner_diameter / 2, s.outer_diameter / 2));
	}

	TurnStock run(int up_to_line) {
		for (size_t k = 0; k < path.segments.size(); ++k) {
			const CncSegment &seg = path.segments[k];
			if (up_to_line >= 0 && seg.line > up_to_line) break;
			if (seg.unresolved) continue;
			if (seg.kind == SEG_RAPID) rapid(seg);
			else feed(seg);
			out.tool_position = seg.to;
		}
		if (out.cut_off) drop_chuck_side();
		for (size_t k = 0; k < section.size(); ++k) out.rings.push_back(from_path(section[k]));
		return out;
	}

private:
	const CncPath &path;
	const ToolTable &tools;
	TurnStock out;
	PathsD section;
	double body_size;
	double cut_width = 0;
	std::map<int, ToolRole> inferred;

	void message(int line, bool error, const std::string &text) {
		CncMessage m;
		m.line = line;
		m.error = error;
		m.text = text;
		out.messages.push_back(m);
	}

	ToolRole role_for(const CncSegment &seg) {
		ToolDef d = tools.get(seg.tool);
		if (d.role != TOOL_UNKNOWN) return d.role;
		std::map<int, ToolRole>::iterator it = inferred.find(seg.tool);
		if (it != inferred.end()) return it->second;
		double mid = (out.stock.inner_diameter + out.stock.outer_diameter) / 4;
		ToolRole r = (seg.from.r < mid) ? TOOL_INTERNAL : TOOL_EXTERNAL;
		inferred[seg.tool] = r;
		return r;
	}

	// Forma de la herramienta relativa a la punta programada. Exterior e
	// interior: una cuña abierta hacia +Z como un inserto real, con el filo
	// principal radial (así los hombros y las caras quedan exactamente en la
	// cota programada) y el secundario a 52° del eje, el de un inserto de 35°,
	// el más agudo habitual. Los conos de hasta 52° quedan exactos y el cuerpo
	// nunca socava lo ya torneado. Cuchilla: su ancho hacia el plato.
	PathD shape_for(ToolRole role, double width) const {
		const double s = body_size;
		PathD p;
		if (role == TOOL_CUTOFF) {
			p.push_back(PointD(-width, 0.0));
			p.push_back(PointD(0.0, 0.0));
			p.push_back(PointD(0.0, s));
			p.push_back(PointD(-width, s));
			return p;
		}
		if (role == TOOL_FACING) {
			// Herramienta de frenteo: cuadrante entero hacia +Z y hacia afuera, para
			// que la cara quede plana hasta el centro (la cuña de 52° dejaría un
			// cono en el eje al frentear una barra maciza)
			p.push_back(PointD(0.0, 0.0));
			p.push_back(PointD(s, 0.0));
			p.push_back(PointD(s, s));
			p.push_back(PointD(0.0, s));
			return p;
		}
		const double sign = (role == TOOL_INTERNAL) ? -1 : 1;
		const double a_minor = 52 * PI / 180;
		p.push_back(PointD(0.0, 0.0));
		p.push_back(PointD(s * std::cos(a_minor), sign * s * std::sin(a_minor)));
		p.push_back(PointD(0.0, sign * s));
		return p;
	}

	void subtract(const PathD &poly) {
		PathsD clip;
		clip.push_back(poly);
		section = Difference(section, clip, FillRule::NonZero, PREC);
	}

	static bool radial_plunge(const CncSegment &seg) {
		return std::fabs(seg.to.z - seg.from.z) < EPS && seg.to.r < seg.from.r - EPS;
	}

	void feed(const CncSegment &seg) {
		ToolRole role = role_for(seg);
		double width = tools.width_of(seg.tool);
		if (role == TOOL_CUTOFF && radial_plunge(seg)) {
			plunge(seg, width);
			return;
		}
		PathD shape = shape_for(role, width);
		if (!seg.is_arc()) {
			subtract(sweep(shape, seg.from, seg.to));
			return;
		}
		// Arco: se muestrea en tramos cortos (error de cuerda menor a 0,002 mm)
		double a0 = angle_of(seg.center, seg.from), a1 = angle_of(seg.center, seg.to);
		double sweep_angle = a1 - a0;
		if (seg.kind == SEG_ARC_CCW) { if (sweep_angle <= 0) sweep_angle += 2 * PI; }
		else { if (sweep_angle >= 0) sweep_angle -= 2 * PI; }
		double step = (seg.radius > 0.002) ? 2 * std::acos(1 - 0.002 / seg.radius) : PI / 18;
		step = std::max(PI / 360, std::min(PI / 18, step));
		int n = std::max(1, (int) std::ceil(std::fabs(sweep_angle) / step));
		CncPoint prev = seg.from;
		for (int k = 1; k <= n; ++k) {
			CncPoint p;
			if (k == n) {
				p = seg.to;
			} else {
				double a = a0 + sweep_angle * k / n;
				p.z = seg.center.z + seg.radius * std::cos(a);
				p.r = seg.center.r + seg.radius * std::sin(a);
			}
			subtract(sweep(shape, prev, p));
			prev = p;
		}
	}

	// Cuchilla de corte o ranurado entrando en sentido radial. La hoja es alta:
	// quita todo lo que haya por encima del fondo dentro de su ancho, también
	// lo que quedara por arriba del punto de partida (en la realidad chocaría).
	void plunge(const CncSegment &seg, double width) {
		double z = seg.to.z;
		double r_lo = seg.to.r, r_hi = seg.from.r;
		subtract(rectangle(z - width, z, r_lo, body_size));
		// ¿Atravesó la pared? Entonces el anillo del lado del plato (el de menor Z)
		// ya no llega al plano de corte y hay otro anillo que empieza en él: la
		// pieza quedó separada. El lado del plato puede seguir teniendo material
		// dentro del ancho de la franja, por debajo o por encima de ella.
		// (Clipper redondea a 1e-5: la tolerancia es más holgada que EPS)
		const double TOL = 1e-4;
		size_t chuck = section.size();
		double chuck_zmin = 1e300, chuck_zmax = -1e300;
		bool part = false;
		for (size_t k = 0; k < section.size(); ++k) {
			double zmin = 1e300, zmax = -1e300;
			for (size_t i = 0; i < section[k].size(); ++i) {
				zmin = std::min(zmin, section[k][i].x);
				zmax = std::max(zmax, section[k][i].x);
			}
			if (zmin < chuck_zmin) { chuck_zmin = zmin; chuck_zmax = zmax; chuck = k; }
			if (zmin >= z - TOL) part = true;
		}
		if (chuck < section.size() && part && chuck_zmax < z - TOL) {
			out.cut_off = true;
			out.cut_z = z;
			cut_width = width;
		} else {
			AssumedGroove g;
			g.z_from = z - width;
			g.z_to = z;
			g.r_from = r_lo;
			g.r_to = r_hi;
			g.line = seg.line;
			g.tool = seg.tool;
			out.assumed.push_back(g);
		}
	}

	void rapid(const CncSegment &seg) {
		const int N = 24;
		for (int k = 0; k <= N; ++k) {
			double t = (double) k / N;
			PointD p(seg.from.z + (seg.to.z - seg.from.z) * t, seg.from.r + (seg.to.r - seg.from.r) * t);
			if (inside_with_margin(section, p, RAPID_MARGIN)) {
				message(seg.line, true, "rápido dentro del material");
				return;
			}
		}
	}

	// Tras el tronzado queda solo la pieza: se descarta todo lo que no llega al plano de corte
	void drop_chuck_side() {
		PathsD kept;
		for (size_t k = 0; k < section.size(); ++k) {
			double zmax = -1e300;
			for (size_t i = 0; i < section[k].size(); ++i) zmax = std::max(zmax, section[k][i].x);
			if (zmax >= out.cut_z - 1e-4) kept.push_back(section[k]);
		}
		section = kept;
	}
};

} // namespace

// ---- API --------------------------------------------------------------------

ToolDef ToolTable::get(int tool) const {
	std::map<int, ToolDef>::const_iterator it = tools.find(tool);
	return it == tools.end() ? ToolDef() : it->second;
}

double ToolTable::width_of(int tool) const {
	ToolDef d = get(tool);
	return d.width > 0 ? d.width : default_width;
}

ToolTable default_tool_table() {
	ToolTable t;
	ToolDef cut, ext, in, face;
	cut.role = TOOL_CUTOFF;
	ext.role = TOOL_EXTERNAL;
	in.role = TOOL_INTERNAL;
	face.role = TOOL_FACING;
	t.tools[1] = cut;
	t.tools[2] = ext;
	t.tools[3] = in;
	t.tools[4] = cut;
	t.tools[5] = cut;
	t.tools[6] = face;
	t.tools[7] = in;
	return t;
}

StockDefinition stock_from_path(const CncPath &path, const ToolTable &tools) {
	StockDefinition s;
	CncBounds b = path.feed_bounds();
	if (path.stock.valid()) {
		s.outer_diameter = path.stock.outer_diameter;
		s.inner_diameter = path.stock.inner_diameter;
	} else if (b.valid) {
		s.deduced = true;
		s.outer_diameter = 2 * (b.rmax + 1);
		s.inner_diameter = (b.rmin > 1) ? 2 * (b.rmin - 1) : 0;
		// El tronzado llega hasta el interior del tubo: su fondo es el mejor dato
		double cut_r = -1;
		for (size_t k = 0; k < path.segments.size(); ++k) {
			const CncSegment &seg = path.segments[k];
			if (seg.kind == SEG_RAPID || seg.unresolved || tools.get(seg.tool).role != TOOL_CUTOFF) continue;
			if (std::fabs(seg.to.z - seg.from.z) < EPS && seg.to.r < seg.from.r - EPS) {
				if (cut_r < 0 || seg.to.r < cut_r) cut_r = seg.to.r;
			}
		}
		if (cut_r >= 0) s.inner_diameter = 2 * cut_r;
	} else {
		return s;   // sin nada que simular
	}
	// La cara del bruto es Z0: la dejó el tronzado de la pieza anterior. Lo que
	// se programa por encima de Z0 es aire; simularlo como material generaría
	// falsos "rápido dentro del material".
	s.z_face = 0;
	double length = path.part_length > 0 ? path.part_length : (b.valid ? -b.zmin : 20);
	if (length < 1) length = 1;
	s.z_end = -length - std::max(5.0, 0.3 * length);
	if (b.valid && b.zmin - 5 < s.z_end) s.z_end = b.zmin - 5;
	return s;
}

double TurnStock::area() const {
	double a = 0;
	for (size_t k = 0; k < rings.size(); ++k) {
		const Ring &r = rings[k];
		double s = 0;
		for (size_t i = 0, n = r.size(); i < n; ++i) {
			const CncPoint &p = r[i], &q = r[(i + 1) % n];
			s += p.z * q.r - q.z * p.r;
		}
		a += s / 2;   // exteriores positivos, agujeros negativos
	}
	return std::fabs(a);
}

std::vector<CncPoint> TurnStock::vertices() const {
	std::vector<CncPoint> v;
	for (size_t k = 0; k < rings.size(); ++k) v.insert(v.end(), rings[k].begin(), rings[k].end());
	return v;
}

TurnStock simulate_stock(const CncPath &path, const StockDefinition &stock, const ToolTable &tools, int up_to_line) {
	if (!stock.valid()) {
		TurnStock empty;
		empty.stock = stock;
		return empty;
	}
	Simulator sim(path, stock, tools);
	return sim.run(up_to_line);
}
