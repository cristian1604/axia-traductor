#include "wxPlotPanel.h"
#include <wx/dcbuffer.h>
#include <wx/dcclient.h>
#include <wx/graphics.h>
#include <wx/settings.h>
#include <cmath>
#include <algorithm>

namespace {

const double PI = 3.14159265358979323846;
const double SCALE_MIN = 0.02;     // píxeles por mm
const double SCALE_MAX = 5000;
const double ZOOM_STEP = 1.2;      // por muesca de rueda, como el simulador anterior

// "12.500" -> "12.5", "-0.000" -> "0"
wxString fmt_mm(double v, int decimals) {
	wxString s = wxString::Format(wxT("%.*f"), decimals, v);
	if (s.Find('.') != wxNOT_FOUND) {
		while (s.EndsWith(wxT("0"))) s.RemoveLast();
		if (s.EndsWith(wxT("."))) s.RemoveLast();
	}
	if (s == wxT("-0")) s = wxT("0");
	return s;
}

// Menor paso "redondo" (1, 2, 5 x 10^n) mayor o igual que min_value
double nice_step(double min_value) {
	if (min_value <= 0) return 1;
	double e = std::pow(10.0, std::floor(std::log10(min_value)));
	const double m[] = { 1, 2, 5, 10 };
	for (int k = 0; k < 4; ++k) if (m[k] * e >= min_value) return m[k] * e;
	return 10 * e;
}

wxColour with_alpha(const wxColour &c, int alpha) {
	return wxColour(c.Red(), c.Green(), c.Blue(), (unsigned char) std::max(0, std::min(255, alpha)));
}

} // namespace

PlotColours default_plot_colours() {
	PlotColours c;
	c.background = wxColour(18, 28, 58);      // azul marino, como el simulador anterior
	c.feed       = wxColour(255, 214, 0);
	c.rapid      = wxColour(150, 165, 190);
	c.current    = wxColour(80, 240, 255);
	c.unresolved = wxColour(255, 96, 96);
	c.grid       = wxColour(40, 54, 92);
	c.axes       = wxColour(110, 126, 162);
	c.text       = wxColour(214, 220, 232);
	c.ref        = wxColour(192, 192, 192);
	c.stock      = wxColour(150, 118, 70, 55);
	c.stock_edge = wxColour(190, 155, 100);
	c.future_alpha = 90;
	return c;
}

wxPlotPanel::wxPlotPanel(wxWindow *parent, wxWindowID id)
	: wxPanel(parent, id, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN | wxWANTS_CHARS | wxFULL_REPAINT_ON_RESIZE),
	  m_colours(default_plot_colours()) {
	SetBackgroundStyle(wxBG_STYLE_PAINT);
	SetBackgroundColour(m_colours.background);
	m_font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	SetCursor(wxCursor(wxCURSOR_CROSS));

	Bind(wxEVT_PAINT, &wxPlotPanel::OnPaint, this);
	Bind(wxEVT_SIZE, &wxPlotPanel::OnSize, this);
	Bind(wxEVT_MOUSEWHEEL, &wxPlotPanel::OnWheel, this);
	Bind(wxEVT_LEFT_DOWN, &wxPlotPanel::OnMouseDown, this);
	Bind(wxEVT_MIDDLE_DOWN, &wxPlotPanel::OnMouseDown, this);
	Bind(wxEVT_LEFT_UP, &wxPlotPanel::OnMouseUp, this);
	Bind(wxEVT_MIDDLE_UP, &wxPlotPanel::OnMouseUp, this);
	Bind(wxEVT_MOTION, &wxPlotPanel::OnMouseMove, this);
	Bind(wxEVT_LEAVE_WINDOW, &wxPlotPanel::OnMouseLeave, this);
	Bind(wxEVT_LEFT_DCLICK, &wxPlotPanel::OnDoubleClick, this);
	Bind(wxEVT_MOUSE_CAPTURE_LOST, &wxPlotPanel::OnCaptureLost, this);
	Bind(wxEVT_KEY_DOWN, &wxPlotPanel::OnKey, this);
}

// ---- Estado --------------------------------------------------------------------

void wxPlotPanel::SetPath(const CncPath &path) {
	bool had = !m_path.segments.empty();
	m_path = path;
	if (!had) FitToPath();
	Refresh();
}

void wxPlotPanel::SetCurrentLine(int line) {
	if (line == m_current) return;
	m_current = line;
	Refresh();
}

void wxPlotPanel::SetColours(const PlotColours &c) {
	m_colours = c;
	SetBackgroundColour(c.background);
	Refresh();
}

void wxPlotPanel::SetShowFuture(bool show) {
	m_show_future = show;
	Refresh();
}

void wxPlotPanel::SetShowRapids(bool show) {
	m_show_rapids = show;
	Refresh();
}

// ---- Vista ---------------------------------------------------------------------

void wxPlotPanel::ToScreen(double z, double r, double &u, double &v) const {
	u = m_ox + z * m_scale;
	v = m_oy - r * m_scale;
}

CncPoint wxPlotPanel::ToWorld(const wxPoint &p) const {
	CncPoint w;
	w.z = (p.x - m_ox) / m_scale;
	w.r = (m_oy - p.y) / m_scale;
	return w;
}

bool wxPlotPanel::StockRange(double &z_face, double &z_end) const {
	if (!m_path.stock.valid()) return false;
	CncBounds b = m_path.feed_bounds();
	if (!b.valid) {
		z_face = 2;
		z_end = -50;
		return true;
	}
	double length = std::max(b.zmax - b.zmin, 5.0);
	z_face = std::max(b.zmax, 0.0);
	z_end = b.zmin - 0.3 * length;   // el tubo sigue más allá de la pieza
	return true;
}

void wxPlotPanel::FitToPath() {
	wxSize sz = GetClientSize();
	if (sz.x <= 0 || sz.y <= 0) return;
	CncBounds b = m_path.feed_bounds();
	double z_face, z_end;
	if (StockRange(z_face, z_end)) {
		b.add(z_face, m_path.stock.outer_diameter / 2);
		b.add(z_end, m_path.stock.inner_diameter / 2);
	}
	if (!b.valid) {
		// Sin nada que encuadrar: origen al centro y escala moderada
		m_scale = 2;
		m_ox = sz.x / 2.0;
		m_oy = sz.y / 2.0;
	} else {
		double dz = std::max(b.zmax - b.zmin, 1.0), dr = std::max(b.rmax - b.rmin, 1.0);
		m_scale = std::min(sz.x * 0.86 / dz, sz.y * 0.86 / dr);
		m_scale = std::max(SCALE_MIN, std::min(SCALE_MAX, m_scale));
		double cz = (b.zmin + b.zmax) / 2, cr = (b.rmin + b.rmax) / 2;
		m_ox = sz.x / 2.0 - cz * m_scale;
		m_oy = sz.y / 2.0 + cr * m_scale;
	}
	m_user_view = false;
	Refresh();
}

void wxPlotPanel::ZoomAt(const wxPoint &p, double factor) {
	double ns = std::max(SCALE_MIN, std::min(SCALE_MAX, m_scale * factor));
	factor = ns / m_scale;
	// El punto del mundo bajo el cursor no se mueve
	m_ox = p.x - (p.x - m_ox) * factor;
	m_oy = p.y - (p.y - m_oy) * factor;
	m_scale = ns;
	m_user_view = true;
	Refresh();
}

void wxPlotPanel::ZoomCentre(double factor) {
	wxSize sz = GetClientSize();
	ZoomAt(wxPoint(sz.x / 2, sz.y / 2), factor);
}

// ---- Eventos -------------------------------------------------------------------

void wxPlotPanel::OnSize(wxSizeEvent &event) {
	if (!m_user_view) FitToPath();
	Refresh();
	event.Skip();
}

void wxPlotPanel::OnWheel(wxMouseEvent &event) {
	int delta = event.GetWheelDelta();
	if (delta <= 0) delta = 120;
	double notches = (double) event.GetWheelRotation() / delta;
	ZoomAt(event.GetPosition(), std::pow(ZOOM_STEP, notches));
}

void wxPlotPanel::OnMouseDown(wxMouseEvent &event) {
	SetFocus();
	m_dragging = true;
	m_drag_last = event.GetPosition();
	CaptureMouse();
	SetCursor(wxCursor(wxCURSOR_SIZING));
}

void wxPlotPanel::OnMouseUp(wxMouseEvent &event) {
	if (m_dragging) {
		m_dragging = false;
		if (HasCapture()) ReleaseMouse();
		SetCursor(wxCursor(wxCURSOR_CROSS));
	}
	event.Skip();
}

void wxPlotPanel::OnCaptureLost(wxMouseCaptureLostEvent &event) {
	m_dragging = false;
	SetCursor(wxCursor(wxCURSOR_CROSS));
}

void wxPlotPanel::OnMouseMove(wxMouseEvent &event) {
	m_have_mouse = true;
	m_mouse = event.GetPosition();
	if (m_dragging && (event.LeftIsDown() || event.MiddleIsDown())) {
		wxPoint d = m_mouse - m_drag_last;
		m_ox += d.x;
		m_oy += d.y;
		m_drag_last = m_mouse;
		m_user_view = true;
	}
	Refresh();   // coordenadas del puntero
}

void wxPlotPanel::OnMouseLeave(wxMouseEvent &event) {
	m_have_mouse = false;
	Refresh();
	event.Skip();
}

void wxPlotPanel::OnDoubleClick(wxMouseEvent &event) {
	FitToPath();
}

void wxPlotPanel::OnKey(wxKeyEvent &event) {
	switch (event.GetKeyCode()) {
	case WXK_HOME:
		FitToPath();
		break;
	case '+': case WXK_NUMPAD_ADD: case WXK_ADD:
		ZoomCentre(ZOOM_STEP);
		break;
	case '-': case WXK_NUMPAD_SUBTRACT: case WXK_SUBTRACT:
		ZoomCentre(1 / ZOOM_STEP);
		break;
	default:
		event.Skip();
	}
}

// ---- Dibujo --------------------------------------------------------------------

double wxPlotPanel::LineWidth(double dip) const {
	return dip * GetDPIScaleFactor();
}

void wxPlotPanel::OnPaint(wxPaintEvent &event) {
	wxAutoBufferedPaintDC dc(this);
	dc.SetBackground(wxBrush(m_colours.background));
	dc.Clear();
	wxGraphicsContext *gc = wxGraphicsContext::Create(dc);
	if (!gc) return;
	gc->SetAntialiasMode(wxANTIALIAS_DEFAULT);
	gc->SetInterpolationQuality(wxINTERPOLATION_BEST);

	DrawStock(gc);
	DrawGrid(gc);
	DrawRefs(gc);
	DrawPath(gc);
	DrawToolMarker(gc);
	DrawOverlay(gc);
	delete gc;
}

void wxPlotPanel::DrawStock(wxGraphicsContext *gc) {
	double z_face, z_end;
	if (!StockRange(z_face, z_end)) return;
	double u0, v0, u1, v1;
	ToScreen(z_end, m_path.stock.outer_diameter / 2, u0, v0);
	ToScreen(z_face, m_path.stock.inner_diameter / 2, u1, v1);
	gc->SetBrush(wxBrush(m_colours.stock));
	gc->SetPen(gc->CreatePen(wxGraphicsPenInfo(m_colours.stock_edge).Width(LineWidth(1))));
	gc->DrawRectangle(u0, v0, u1 - u0, v1 - v0);
}

void wxPlotPanel::DrawGrid(wxGraphicsContext *gc) {
	wxSize sz = GetClientSize();
	CncPoint tl = ToWorld(wxPoint(0, 0)), br = ToWorld(wxPoint(sz.x, sz.y));
	double zmin = tl.z, zmax = br.z, rmin = br.r, rmax = tl.r;

	// Pasos: al menos 70 píxeles entre líneas; en X el rótulo es el diámetro
	double step_z = nice_step(70 / m_scale);
	double step_x = nice_step(2 * 70 / m_scale);

	gc->SetFont(m_font, m_colours.text);
	double tw, th;
	gc->GetTextExtent(wxT("0"), &tw, &th);

	wxGraphicsPath grid = gc->CreatePath();
	wxGraphicsPath axes = gc->CreatePath();
	for (double z = std::ceil(zmin / step_z) * step_z; z <= zmax; z += step_z) {
		double u, v;
		ToScreen(z, 0, u, v);
		wxGraphicsPath &p = (std::fabs(z) < step_z * 1e-6) ? axes : grid;
		p.MoveToPoint(u, 0);
		p.AddLineToPoint(u, sz.y);
	}
	for (double x = std::ceil(2 * rmin / step_x) * step_x; x <= 2 * rmax; x += step_x) {
		double u, v;
		ToScreen(0, x / 2, u, v);
		if (std::fabs(x) < step_x * 1e-6) continue;   // el eje Z se dibuja aparte, como línea de centro
		grid.MoveToPoint(0, v);
		grid.AddLineToPoint(sz.x, v);
	}
	gc->SetPen(gc->CreatePen(wxGraphicsPenInfo(m_colours.grid).Width(LineWidth(1))));
	gc->StrokePath(grid);
	gc->SetPen(gc->CreatePen(wxGraphicsPenInfo(m_colours.axes).Width(LineWidth(1))));
	gc->StrokePath(axes);
	// Eje de la pieza (X0): trazo y punto
	{
		double u, v;
		ToScreen(0, 0, u, v);
		if (v >= 0 && v <= sz.y) {
			gc->SetPen(gc->CreatePen(wxGraphicsPenInfo(m_colours.axes).Width(LineWidth(1)).Style(wxPENSTYLE_DOT_DASH)));
			gc->StrokeLine(0, v, sz.x, v);
		}
	}

	// Rótulos: Z abajo, X (diámetro) a la izquierda
	for (double z = std::ceil(zmin / step_z) * step_z; z <= zmax; z += step_z) {
		double u, v;
		ToScreen(z, 0, u, v);
		gc->DrawText(fmt_mm(z, 3), u + 3, sz.y - th - 2);
	}
	for (double x = std::ceil(2 * rmin / step_x) * step_x; x <= 2 * rmax; x += step_x) {
		double u, v;
		ToScreen(0, x / 2, u, v);
		if (v > sz.y - th - 4) continue;   // no pisar los rótulos de Z
		gc->DrawText(fmt_mm(x, 3), 3, v - th - 1);
	}
}

void wxPlotPanel::DrawRefs(wxGraphicsContext *gc) {
	if (m_path.refs.empty()) return;
	wxSize sz = GetClientSize();
	wxGraphicsPath nominal = gc->CreatePath(), aux = gc->CreatePath();
	for (size_t k = 0; k < m_path.refs.size(); ++k) {
		const CncRefLine &r = m_path.refs[k];
		wxGraphicsPath &p = r.nominal ? nominal : aux;
		double u, v;
		if (r.vertical) {
			ToScreen(r.value, 0, u, v);
			p.MoveToPoint(u, 0);
			p.AddLineToPoint(u, sz.y);
		} else {
			ToScreen(0, r.value, u, v);
			p.MoveToPoint(0, v);
			p.AddLineToPoint(sz.x, v);
		}
	}
	gc->SetPen(gc->CreatePen(wxGraphicsPenInfo(m_colours.ref).Width(LineWidth(1)).Style(wxPENSTYLE_DOT_DASH)));
	gc->StrokePath(nominal);
	gc->SetPen(gc->CreatePen(wxGraphicsPenInfo(m_colours.ref).Width(LineWidth(1)).Style(wxPENSTYLE_SHORT_DASH)));
	gc->StrokePath(aux);
}

void wxPlotPanel::AddSegment(wxGraphicsPath &path, const CncSegment &s) const {
	double u0, v0, u1, v1;
	ToScreen(s.from.z, s.from.r, u0, v0);
	ToScreen(s.to.z, s.to.r, u1, v1);
	path.MoveToPoint(u0, v0);
	if (!s.is_arc() || s.radius <= 0) {
		path.AddLineToPoint(u1, v1);
		return;
	}
	// En pantalla el eje vertical apunta hacia abajo: el ángulo cambia de signo
	double cu, cv;
	ToScreen(s.center.z, s.center.r, cu, cv);
	double a0 = -std::atan2(s.from.r - s.center.r, s.from.z - s.center.z);
	double a1 = -std::atan2(s.to.r - s.center.r, s.to.z - s.center.z);
	path.AddArc(cu, cv, s.radius * m_scale, a0, a1, s.kind == SEG_ARC_CW);
}

void wxPlotPanel::DrawPath(wxGraphicsContext *gc) {
	if (m_path.segments.empty()) return;
	const bool tracking = (m_current >= 0);
	wxGraphicsPath feed = gc->CreatePath(), rapid = gc->CreatePath();
	wxGraphicsPath feed_future = gc->CreatePath(), rapid_future = gc->CreatePath();
	wxGraphicsPath current = gc->CreatePath(), unresolved = gc->CreatePath();
	bool has_future = false;

	for (size_t k = 0; k < m_path.segments.size(); ++k) {
		const CncSegment &s = m_path.segments[k];
		if (s.kind == SEG_RAPID && !m_show_rapids) continue;
		if (s.unresolved) { AddSegment(unresolved, s); continue; }
		if (tracking && s.line == m_current) { AddSegment(current, s); continue; }
		if (tracking && s.line > m_current) {
			if (!m_show_future) continue;
			has_future = true;
			AddSegment(s.kind == SEG_RAPID ? rapid_future : feed_future, s);
			continue;
		}
		AddSegment(s.kind == SEG_RAPID ? rapid : feed, s);
	}

	const double w = LineWidth(1.4);
	if (has_future) {
		gc->SetPen(gc->CreatePen(wxGraphicsPenInfo(with_alpha(m_colours.rapid, m_colours.future_alpha)).Width(w).Style(wxPENSTYLE_DOT)));
		gc->StrokePath(rapid_future);
		gc->SetPen(gc->CreatePen(wxGraphicsPenInfo(with_alpha(m_colours.feed, m_colours.future_alpha)).Width(w)));
		gc->StrokePath(feed_future);
	}
	gc->SetPen(gc->CreatePen(wxGraphicsPenInfo(m_colours.rapid).Width(w).Style(wxPENSTYLE_DOT)));
	gc->StrokePath(rapid);
	gc->SetPen(gc->CreatePen(wxGraphicsPenInfo(m_colours.feed).Width(w)));
	gc->StrokePath(feed);
	gc->SetPen(gc->CreatePen(wxGraphicsPenInfo(m_colours.unresolved).Width(w).Style(wxPENSTYLE_SHORT_DASH)));
	gc->StrokePath(unresolved);
	gc->SetPen(gc->CreatePen(wxGraphicsPenInfo(m_colours.current).Width(LineWidth(3))));
	gc->StrokePath(current);
}

void wxPlotPanel::DrawToolMarker(wxGraphicsContext *gc) {
	if (m_path.segments.empty()) return;
	// Posición de la herramienta tras la línea actual (o al final del programa)
	const CncSegment *last = NULL;
	for (size_t k = 0; k < m_path.segments.size(); ++k) {
		const CncSegment &s = m_path.segments[k];
		if (m_current >= 0 && s.line > m_current) break;
		last = &s;
	}
	if (!last) return;
	double u, v;
	ToScreen(last->to.z, last->to.r, u, v);
	double R = LineWidth(5), L = LineWidth(9);
	gc->SetPen(gc->CreatePen(wxGraphicsPenInfo(m_colours.current).Width(LineWidth(1.5))));
	gc->SetBrush(wxBrush(with_alpha(m_colours.current, 60)));
	gc->DrawEllipse(u - R, v - R, 2 * R, 2 * R);
	gc->StrokeLine(u - L, v, u + L, v);
	gc->StrokeLine(u, v - L, u, v + L);
}

void wxPlotPanel::DrawOverlay(wxGraphicsContext *gc) {
	wxSize sz = GetClientSize();
	gc->SetFont(m_font, m_colours.text);
	wxString text;
	if (m_have_mouse) {
		CncPoint w = ToWorld(m_mouse);
		text = wxString::Format(wxT("X %s   Z %s"), fmt_mm(w.x(), 3), fmt_mm(w.z, 3));
	} else if (m_path.segments.empty()) {
		text = wxT("Sin trayectoria");
	} else {
		return;
	}
	double tw, th;
	gc->GetTextExtent(text, &tw, &th);
	double pad = LineWidth(6);
	double x = sz.x - tw - 2 * pad - LineWidth(4), y = LineWidth(4);
	gc->SetBrush(wxBrush(with_alpha(m_colours.background, 200)));
	gc->SetPen(gc->CreatePen(wxGraphicsPenInfo(m_colours.grid).Width(LineWidth(1))));
	gc->DrawRoundedRectangle(x, y, tw + 2 * pad, th + 2 * pad, LineWidth(3));
	gc->DrawText(text, x + pad, y + pad);
}
