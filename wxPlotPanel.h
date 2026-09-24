#ifndef WXPLOTPANEL_H
#define WXPLOTPANEL_H
#include <wx/panel.h>
#include <wx/colour.h>
#include <wx/font.h>
#include "CncPath.h"

class wxGraphicsContext;
class wxGraphicsPath;

/**
	Colores del graficador. Por ahora fijos (default_plot_colours); en la
	fase 3 pasan a las Opciones del usuario.
**/
struct PlotColours {
	wxColour background;
	wxColour feed;         // G01, G02, G03: trazo continuo
	wxColour rapid;        // G00: punteado
	wxColour current;      // tramos de la línea actual del editor
	wxColour unresolved;   // cotas paramétricas
	wxColour grid;
	wxColour axes;
	wxColour text;
	wxColour ref;          // líneas de referencia #DN/#DA/#HN/#HA
	wxColour stock;        // relleno del tubo en bruto (con alfa)
	wxColour stock_edge;
	int future_alpha;      // opacidad (0-255) de lo posterior a la línea actual
};
PlotColours default_plot_colours();

/**
	Graficador de trayectorias: dibuja la salida de interpret_cnc (CncPath) en
	el plano Z/X con wxGraphicsContext (Direct2D/GDI+ en Windows, Cairo en
	Linux). Z hacia la derecha, X hacia arriba; la vista es la media sección
	superior de la pieza, con X en diámetro en los rótulos y en las
	coordenadas del puntero.

	- Rueda: zoom conservando el punto bajo el cursor. Arrastre con botón
	  izquierdo o central: desplazamiento. Doble clic o Inicio: encuadrar.
	  + y -: zoom sobre el centro.
	- SetCurrentLine resalta los tramos de esa línea y atenúa lo posterior,
	  para seguir el programa desde el editor.
	Se instancia desde el código generado por wxUiEditor como CustomControl.
**/
class wxPlotPanel : public wxPanel {
public:
	wxPlotPanel(wxWindow *parent, wxWindowID id = wxID_ANY);

	// Trayectoria a dibujar. Encuadra si antes no había nada; si no, conserva la vista.
	void SetPath(const CncPath &path);
	const CncPath &GetPath() const { return m_path; }

	// Línea del programa (desde 0) que se resalta; lo posterior se atenúa. -1: todo normal.
	void SetCurrentLine(int line);
	int GetCurrentLine() const { return m_current; }

	void SetColours(const PlotColours &c);
	const PlotColours &GetColours() const { return m_colours; }
	void SetShowFuture(bool show);   // dibujar (atenuado) lo posterior a la línea actual
	bool GetShowFuture() const { return m_show_future; }
	void SetShowRapids(bool show);
	bool GetShowRapids() const { return m_show_rapids; }

	void FitToPath();                          // encuadra avances y bruto
	void ZoomAt(const wxPoint &screen, double factor);
	void ZoomCentre(double factor);

	CncPoint ToWorld(const wxPoint &p) const;  // pantalla -> (z, r)
	void ToScreen(double z, double r, double &u, double &v) const;

private:
	CncPath m_path;
	PlotColours m_colours;
	int m_current = -1;
	bool m_show_future = true;
	bool m_show_rapids = true;

	double m_scale = 2;            // píxeles por milímetro
	double m_ox = 0, m_oy = 0;     // posición en pantalla del origen (Z0, X0)
	bool m_user_view = false;      // el usuario movió o acercó: no reencuadrar al cambiar el tamaño

	bool m_dragging = false;
	wxPoint m_drag_last;
	bool m_have_mouse = false;
	wxPoint m_mouse;
	wxFont m_font;

	void OnPaint(wxPaintEvent &event);
	void OnSize(wxSizeEvent &event);
	void OnWheel(wxMouseEvent &event);
	void OnMouseDown(wxMouseEvent &event);
	void OnMouseUp(wxMouseEvent &event);
	void OnMouseMove(wxMouseEvent &event);
	void OnMouseLeave(wxMouseEvent &event);
	void OnDoubleClick(wxMouseEvent &event);
	void OnCaptureLost(wxMouseCaptureLostEvent &event);
	void OnKey(wxKeyEvent &event);

	void DrawStock(wxGraphicsContext *gc);
	void DrawGrid(wxGraphicsContext *gc);
	void DrawRefs(wxGraphicsContext *gc);
	void DrawPath(wxGraphicsContext *gc);
	void DrawToolMarker(wxGraphicsContext *gc);
	void DrawOverlay(wxGraphicsContext *gc);
	void AddSegment(wxGraphicsPath &path, const CncSegment &s) const;
	double LineWidth(double dip) const;
	// Extremos del tubo en bruto en Z (la longitud real no se conoce: se estima de los avances)
	bool StockRange(double &z_face, double &z_end) const;
};

#endif
