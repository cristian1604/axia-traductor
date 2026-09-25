#include "wxTurnView3D.h"
#include "clipper2/clipper.h"
#include <wx/dcclient.h>
#include <cmath>
#include <algorithm>
#ifdef __WXMSW__
#include <windows.h>
#endif
#include <GL/gl.h>

namespace {

const double PI = 3.14159265358979323846;
const int STEPS = 96;          // pasos angulares de la vuelta completa
const double FOV = 35;         // grados

wxGLAttributes canvas_attributes() {
	wxGLAttributes va;
	va.PlatformDefaults().RGBA().DoubleBuffer().Depth(24).EndList();
	return va;
}

void push3(std::vector<float> &v, double x, double y, double z) {
	v.push_back((float) x);
	v.push_back((float) y);
	v.push_back((float) z);
}

void gl_colour(const wxColour &c) {
	glColor4f(c.Red() / 255.0f, c.Green() / 255.0f, c.Blue() / 255.0f, c.Alpha() / 255.0f);
}

} // namespace

bool wxTurnView3D::IsSupported() {
	return wxGLCanvas::IsDisplaySupported(canvas_attributes());
}

wxTurnView3D::wxTurnView3D(wxWindow *parent)
	: wxGLCanvas(parent, canvas_attributes(), wxID_ANY, wxDefaultPosition, wxDefaultSize, wxWANTS_CHARS),
	  m_context(NULL), m_colours(default_plot_colours()) {
	// El contexto se crea en el primer dibujo, con el lienzo ya visible (en GTK
	// no puede crearse antes)
	SetCursor(wxCursor(wxCURSOR_ARROW));
	Bind(wxEVT_PAINT, &wxTurnView3D::OnPaint, this);
	Bind(wxEVT_SIZE, &wxTurnView3D::OnSize, this);
	Bind(wxEVT_ERASE_BACKGROUND, &wxTurnView3D::OnEraseBackground, this);
	Bind(wxEVT_LEFT_DOWN, &wxTurnView3D::OnMouseDown, this);
	Bind(wxEVT_RIGHT_DOWN, &wxTurnView3D::OnMouseDown, this);
	Bind(wxEVT_MIDDLE_DOWN, &wxTurnView3D::OnMouseDown, this);
	Bind(wxEVT_LEFT_UP, &wxTurnView3D::OnMouseUp, this);
	Bind(wxEVT_RIGHT_UP, &wxTurnView3D::OnMouseUp, this);
	Bind(wxEVT_MIDDLE_UP, &wxTurnView3D::OnMouseUp, this);
	Bind(wxEVT_MOTION, &wxTurnView3D::OnMouseMove, this);
	Bind(wxEVT_MOUSEWHEEL, &wxTurnView3D::OnWheel, this);
	Bind(wxEVT_KEY_DOWN, &wxTurnView3D::OnKey, this);
	Bind(wxEVT_MOUSE_CAPTURE_LOST, &wxTurnView3D::OnCaptureLost, this);
}

wxTurnView3D::~wxTurnView3D() {
	delete m_context;
}

// ---- Escena --------------------------------------------------------------------

void wxTurnView3D::SetScene(const TurnStock &stock, const CncPath &path, int current_line, const CncPoint &tool) {
	m_stock = stock;
	m_path = path;
	m_current = current_line;
	m_tool = tool;
	m_have_tool = !path.segments.empty();
	BuildMesh();
	Refresh(false);
}

void wxTurnView3D::SetColours(const PlotColours &c) {
	m_colours = c;
	Refresh(false);
}

void wxTurnView3D::SetCutAway(bool cut) {
	if (cut == m_cut_away) return;
	m_cut_away = cut;
	BuildMesh();
	Refresh(false);
}

void wxTurnView3D::SetShowPath(bool show) {
	m_show_path = show;
	Refresh(false);
}

void wxTurnView3D::ResetCamera() {
	m_yaw = 35;
	m_pitch = 22;
	m_distance = m_radius * 2.6;
	for (int k = 0; k < 3; ++k) m_target[k] = m_centre[k];
	Refresh(false);
}

// Superficie de revolución de cada lado de la sección y, en corte, las caras
// de la sección en el plano Z_gl = 0 (mitad superior igual al 2D, inferior espejada).
// Mundo OpenGL: X = Z del torno, Y = r·cos(θ), Z = r·sin(θ).
void wxTurnView3D::BuildMesh() {
	m_verts.clear();
	m_norms.clear();
	m_caps.clear();
	m_path_feed.clear();
	m_path_rapid.clear();

	double zmin = 1e300, zmax = -1e300, rmax = 0;
	// Se conserva la mitad de atrás (θ de 180° a 360°): el plano de corte queda de frente
	const double t0 = m_cut_away ? PI : 0, t1 = 2 * PI;
	const int steps = m_cut_away ? STEPS / 2 : STEPS;

	for (size_t k = 0; k < m_stock.rings.size(); ++k) {
		const Ring &ring = m_stock.rings[k];
		size_t n = ring.size();
		if (n < 3) continue;
		for (size_t i = 0; i < n; ++i) {
			const CncPoint &a = ring[i], &b = ring[(i + 1) % n];
			zmin = std::min(zmin, a.z); zmax = std::max(zmax, a.z); rmax = std::max(rmax, a.r);
			if (a.r <= 1e-9 && b.r <= 1e-9) continue;   // lado sobre el eje: no genera superficie
			double dz = b.z - a.z, dr = b.r - a.r;
			double L = std::sqrt(dz * dz + dr * dr);
			if (L < 1e-9) continue;
			double nz = dr / L, nr = -dz / L;   // normal hacia afuera del material (anillos exteriores antihorarios)
			for (int s = 0; s < steps; ++s) {
				double ta = t0 + (t1 - t0) * s / steps, tb = t0 + (t1 - t0) * (s + 1) / steps;
				double ca = std::cos(ta), sa = std::sin(ta), cb = std::cos(tb), sb = std::sin(tb);
				// A: a en ta, B: b en ta, C: b en tb, D: a en tb
				push3(m_verts, a.z, a.r * ca, a.r * sa); push3(m_norms, nz, nr * ca, nr * sa);
				push3(m_verts, b.z, b.r * ca, b.r * sa); push3(m_norms, nz, nr * ca, nr * sa);
				push3(m_verts, b.z, b.r * cb, b.r * sb); push3(m_norms, nz, nr * cb, nr * sb);
				push3(m_verts, a.z, a.r * ca, a.r * sa); push3(m_norms, nz, nr * ca, nr * sa);
				push3(m_verts, b.z, b.r * cb, b.r * sb); push3(m_norms, nz, nr * cb, nr * sb);
				push3(m_verts, a.z, a.r * cb, a.r * sb); push3(m_norms, nz, nr * cb, nr * sb);
			}
		}
	}

	if (m_cut_away && !m_stock.rings.empty()) {
		Clipper2Lib::PathsD section, triangles;
		for (size_t k = 0; k < m_stock.rings.size(); ++k) {
			Clipper2Lib::PathD p;
			for (size_t i = 0; i < m_stock.rings[k].size(); ++i) p.push_back(Clipper2Lib::PointD(m_stock.rings[k][i].z, m_stock.rings[k][i].r));
			section.push_back(p);
		}
		if (Clipper2Lib::Triangulate(section, 5, triangles) == Clipper2Lib::TriangulateResult::success) {
			for (size_t t = 0; t < triangles.size(); ++t) {
				if (triangles[t].size() != 3) continue;
				for (int mirror = 0; mirror < 2; ++mirror) {
					double sign = mirror ? -1 : 1;
					for (int i = 0; i < 3; ++i) push3(m_caps, triangles[t][i].x, sign * triangles[t][i].y, 0);
				}
			}
		}
	}

	// Encuadre: centro y radio de la escena, solo con la pieza (los rápidos se van lejos)
	if (zmin > zmax) {
		const StockDefinition &d = m_stock.stock;
		if (d.valid()) { zmin = d.z_end; zmax = d.z_face; rmax = d.outer_diameter / 2; }
		else { zmin = -20; zmax = 0; rmax = 10; }
	}
	double old_centre = m_centre[0], old_radius = m_radius;
	bool first = m_verts.empty() && m_caps.empty() && m_path_feed.empty();
	m_centre[0] = (zmin + zmax) / 2;
	m_centre[1] = 0;
	m_centre[2] = 0;
	double half_len = (zmax - zmin) / 2;
	m_radius = std::max(1.0, std::sqrt(half_len * half_len + rmax * rmax));
	// Se reencuadra al principio y cuando la escena cambia mucho (tronzado): en
	// el paso a paso normal la cámara se queda donde el usuario la dejó
	if (first || m_distance <= 0 || m_radius > old_radius * 1.5 || m_radius < old_radius / 1.5
	    || std::fabs(m_centre[0] - old_centre) > old_radius * 0.5) {
		ResetCamera();
	}

	// Recorrido hasta la línea actual, en el plano de corte, recortado a la zona de la pieza
	const double z_lim = zmax + 0.4 * (zmax - zmin);
	for (size_t k = 0; k < m_path.segments.size(); ++k) {
		const CncSegment &s = m_path.segments[k];
		if (m_current >= 0 && s.line > m_current) break;
		if (s.unresolved || (s.from.z > z_lim && s.to.z > z_lim)) continue;
		std::vector<float> &dst = (s.kind == SEG_RAPID) ? m_path_rapid : m_path_feed;
		if (!s.is_arc() || s.radius <= 0) {
			CncPoint a = s.from, b = s.to;
			if (a.z > z_lim) a.z = z_lim;
			if (b.z > z_lim) b.z = z_lim;
			push3(dst, a.z, a.r, 0);
			push3(dst, b.z, b.r, 0);
			continue;
		}
		double a0 = std::atan2(s.from.r - s.center.r, s.from.z - s.center.z);
		double a1 = std::atan2(s.to.r - s.center.r, s.to.z - s.center.z);
		double sweep = a1 - a0;
		if (s.kind == SEG_ARC_CCW) { if (sweep <= 0) sweep += 2 * PI; }
		else { if (sweep >= 0) sweep -= 2 * PI; }
		int n = std::max(4, (int) (std::fabs(sweep) / (PI / 36)));
		for (int i = 0; i < n; ++i) {
			double aa = a0 + sweep * i / n, ab = a0 + sweep * (i + 1) / n;
			push3(dst, s.center.z + s.radius * std::cos(aa), s.center.r + s.radius * std::sin(aa), 0);
			push3(dst, s.center.z + s.radius * std::cos(ab), s.center.r + s.radius * std::sin(ab), 0);
		}
	}
}

// ---- Eventos -------------------------------------------------------------------

void wxTurnView3D::OnSize(wxSizeEvent &event) {
	Refresh(false);
	event.Skip();
}

void wxTurnView3D::OnEraseBackground(wxEraseEvent &event) {
	// OpenGL borra todo: evita el parpadeo
}

void wxTurnView3D::OnMouseDown(wxMouseEvent &event) {
	SetFocus();
	m_dragging = true;
	m_drag_button = event.GetButton();
	m_last = event.GetPosition();
	CaptureMouse();
}

void wxTurnView3D::OnMouseUp(wxMouseEvent &event) {
	if (m_dragging) {
		m_dragging = false;
		if (HasCapture()) ReleaseMouse();
	}
}

void wxTurnView3D::OnCaptureLost(wxMouseCaptureLostEvent &event) {
	m_dragging = false;
}

void wxTurnView3D::OnMouseMove(wxMouseEvent &event) {
	if (!m_dragging) return;
	wxPoint p = event.GetPosition();
	int dx = p.x - m_last.x, dy = p.y - m_last.y;
	m_last = p;
	if (m_drag_button == wxMOUSE_BTN_LEFT) {
		m_yaw += dx * 0.5;
		m_pitch = std::max(-89.0, std::min(89.0, m_pitch + dy * 0.5));
	} else {
		// Desplazamiento en el plano de la cámara
		int h = std::max(1, GetClientSize().y);
		double k = 2 * m_distance * std::tan(FOV / 2 * PI / 180) / h;
		double yaw = m_yaw * PI / 180, pitch = m_pitch * PI / 180;
		// Vectores derecha y arriba de la cámara en coordenadas del mundo
		double right[3] = { std::cos(yaw), 0, -std::sin(yaw) };
		double up[3] = { std::sin(yaw) * std::sin(pitch), std::cos(pitch), std::cos(yaw) * std::sin(pitch) };
		for (int i = 0; i < 3; ++i) m_target[i] += -dx * k * right[i] + dy * k * up[i];
	}
	Refresh(false);
}

void wxTurnView3D::OnWheel(wxMouseEvent &event) {
	int delta = event.GetWheelDelta();
	if (delta <= 0) delta = 120;
	double notches = (double) event.GetWheelRotation() / delta;
	m_distance = std::max(m_radius * 0.05, std::min(m_radius * 30, m_distance * std::pow(1.2, -notches)));
	Refresh(false);
}

void wxTurnView3D::OnKey(wxKeyEvent &event) {
	switch (event.GetKeyCode()) {
	case WXK_HOME: ResetCamera(); break;
	case 'C': SetCutAway(!m_cut_away); break;
	case 'T': SetShowPath(!m_show_path); break;
	default: event.Skip();
	}
}

// ---- Dibujo --------------------------------------------------------------------

void wxTurnView3D::InitGL() {
	if (m_gl_ready) return;
	m_gl_ready = true;
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	GLfloat ambient[] = { 0.35f, 0.35f, 0.38f, 1 };
	GLfloat diffuse[] = { 0.85f, 0.85f, 0.82f, 1 };
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	GLfloat specular[] = { 0.25f, 0.25f, 0.25f, 1 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 24);
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void wxTurnView3D::OnPaint(wxPaintEvent &event) {
	wxPaintDC dc(this);
	if (m_context_failed || !IsShownOnScreen()) return;
	if (!m_context) {
		m_context = new wxGLContext(this);
		if (!m_context->IsOK()) {
			delete m_context;
			m_context = NULL;
			m_context_failed = true;
			return;
		}
	}
	SetCurrent(*m_context);
	InitGL();
	Render();
	SwapBuffers();
}

void wxTurnView3D::Render() {
	wxSize sz = GetClientSize();
	int w = std::max(1, sz.x), h = std::max(1, sz.y);
	glViewport(0, 0, w, h);
	const wxColour &bg = m_colours.background;
	glClearColor(bg.Red() / 255.0f, bg.Green() / 255.0f, bg.Blue() / 255.0f, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Proyección en perspectiva
	double aspect = (double) w / h;
	double near_ = std::max(0.01, m_distance - m_radius * 3), far_ = m_distance + m_radius * 3;
	double top = near_ * std::tan(FOV / 2 * PI / 180), right = top * aspect;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-right, right, -top, top, near_, far_);

	// Cámara: luz fija respecto del observador, luego la órbita
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	GLfloat light[] = { 0.4f, 0.7f, 1.0f, 0 };
	glLightfv(GL_LIGHT0, GL_POSITION, light);
	glTranslated(0, 0, -m_distance);
	glRotated(m_pitch, 1, 0, 0);
	glRotated(m_yaw, 0, 1, 0);
	glTranslated(-m_target[0], -m_target[1], -m_target[2]);

	// Superficie de la pieza
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	if (!m_verts.empty()) {
		gl_colour(wxColour(m_colours.section.Red(), m_colours.section.Green(), m_colours.section.Blue()));
		glVertexPointer(3, GL_FLOAT, 0, &m_verts[0]);
		glNormalPointer(GL_FLOAT, 0, &m_norms[0]);
		glDrawArrays(GL_TRIANGLES, 0, (GLsizei) (m_verts.size() / 3));
	}
	glDisableClientState(GL_NORMAL_ARRAY);
	// Caras de la sección: normal hacia el observador
	if (!m_caps.empty()) {
		glNormal3f(0, 0, 1);
		gl_colour(m_colours.section_edge);
		glVertexPointer(3, GL_FLOAT, 0, &m_caps[0]);
		glDrawArrays(GL_TRIANGLES, 0, (GLsizei) (m_caps.size() / 3));
	}
	glDisableClientState(GL_VERTEX_ARRAY);

	// Recorrido y herramienta en el plano de corte, sin iluminación y apenas delante de las caras
	glDisable(GL_LIGHTING);
	glPushMatrix();
	glTranslated(0, 0, m_radius * 0.002);
	if (m_show_path && m_cut_away) {
		glLineWidth(1.5f);
		if (!m_path_rapid.empty()) {
			glEnable(GL_LINE_STIPPLE);
			glLineStipple(2, 0x3333);
			gl_colour(m_colours.rapid);
			glBegin(GL_LINES);
			for (size_t i = 0; i + 2 < m_path_rapid.size(); i += 3) glVertex3fv(&m_path_rapid[i]);
			glEnd();
			glDisable(GL_LINE_STIPPLE);
		}
		if (!m_path_feed.empty()) {
			gl_colour(m_colours.feed);
			glBegin(GL_LINES);
			for (size_t i = 0; i + 2 < m_path_feed.size(); i += 3) glVertex3fv(&m_path_feed[i]);
			glEnd();
		}
	}
	if (m_have_tool) {
		// Herramienta: un rombo en su posición
		double s = m_radius * 0.03;
		gl_colour(m_colours.current);
		glBegin(GL_TRIANGLES);
		double cz = m_tool.z, cr = m_tool.r;
		glVertex3d(cz, cr + s, 0); glVertex3d(cz - s, cr, 0); glVertex3d(cz, cr - s, 0);
		glVertex3d(cz, cr + s, 0); glVertex3d(cz, cr - s, 0); glVertex3d(cz + s, cr, 0);
		glEnd();
	}
	// Eje de la pieza
	gl_colour(m_colours.axes);
	glEnable(GL_LINE_STIPPLE);
	glLineStipple(3, 0x0F0F);
	glBegin(GL_LINES);
	glVertex3d(m_centre[0] - m_radius * 1.3, 0, 0);
	glVertex3d(m_centre[0] + m_radius * 1.3, 0, 0);
	glEnd();
	glDisable(GL_LINE_STIPPLE);
	glPopMatrix();
	glEnable(GL_LIGHTING);
}
