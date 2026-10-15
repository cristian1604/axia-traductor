#ifndef WXPLOTPANEL_H
#define WXPLOTPANEL_H
#include <wx/panel.h>
#include <wx/colour.h>
#include <wx/font.h>
#include <vector>
#include "CncPath.h"
#include "TurnStock.h"

class wxGraphicsContext;
class wxGraphicsPath;
class wxTurnView3D;

/**
	Colores del graficador. Por ahora fijos (default_plot_colours); más
	adelante pasan a las Opciones del usuario.
**/
struct PlotColours {
	wxColour background;
	wxColour feed;         // G01, G02, G03: trazo continuo
	wxColour rapid;        // G00: punteado
	wxColour current;      // tramos de la línea actual del editor
	wxColour unresolved;   // cotas paramétricas y ranuras de ancho supuesto
	wxColour grid;
	wxColour axes;
	wxColour text;
	wxColour ref;          // líneas de referencia #DN/#DA/#HN/#HA
	wxColour stock;        // tubo en bruto (con alfa)
	wxColour stock_edge;
	wxColour section;      // material que queda (sección de la pieza)
	wxColour section_edge;
	wxColour measure;      // enganche y cotas medidas
	int future_alpha;      // opacidad (0-255) de lo posterior a la línea actual
};
PlotColours default_plot_colours();

// Cota medida entre dos puntos del plano Z/R
struct PlotDimension {
	CncPoint a, b;
};

/**
	Graficador de trayectorias y de la pieza: dibuja la salida de
	interpret_cnc (CncPath) en el plano Z/X con wxGraphicsContext (Direct2D o
	GDI+ en Windows, Cairo en Linux) y, si se pide, la sección de la pieza que
	va quedando según simulate_stock (TurnStock). Z hacia la derecha, X hacia
	arriba; la vista es la media sección superior, con X en diámetro en los
	rótulos, las coordenadas del puntero y las cotas.

	- Rueda: zoom conservando el punto bajo el cursor. Arrastre con botón
	  izquierdo o central: desplazamiento. Doble clic o Inicio: encuadrar.
	  + y -: zoom sobre el centro.
	- SetCurrentLine resalta los tramos de esa línea, atenúa lo posterior y
	  recalcula la pieza hasta esa línea, para seguir el programa desde el editor.
	- Medición: el puntero se engancha al vértice más cercano del perfil (o
	  del recorrido, si la pieza está oculta) y muestra sus X y Z. Un clic sin
	  arrastrar fija un punto; el segundo clic deja una cota con ΔX en
	  diámetro y en radio, ΔZ y distancia. Retroceso borra la última, Escape todas.
	Se instancia desde el código generado por wxUiEditor como CustomControl.
**/
class wxPlotPanel : public wxPanel {
public:
	wxPlotPanel(wxWindow *parent, wxWindowID id = wxID_ANY);

	// Trayectoria a dibujar. Encuadra si antes no había nada; si no, conserva la vista.
	void SetPath(const CncPath &path);
	const CncPath &GetPath() const { return m_path; }

	// Línea del programa (desde 0) que se resalta; lo posterior se atenúa. -1: todo.
	void SetCurrentLine(int line);
	int GetCurrentLine() const { return m_current; }

	// Simulación de la pieza (sección con material). La completa sirve para
	// los avisos y la que corresponde a la línea actual es la que se dibuja.
	void SetShowStock(bool show);
	bool GetShowStock() const { return m_show_stock; }
	void SetToolTable(const ToolTable &tools);
	const TurnStock &GetFullStock() const { return m_full_stock; }
	bool HasStock() const { return m_show_stock && m_stock_def.valid(); }

	// Cotas medidas
	const std::vector<PlotDimension> &GetDimensions() const { return m_dims; }
	void ClearDimensions();

	// Vista 3D (wxTurnView3D como hijo que tapa el dibujo 2D). Si OpenGL no
	// está disponible, SetView3D(true) no hace nada y GetView3D() sigue en false.
	static bool View3DSupported();
	void SetView3D(bool on);
	bool GetView3D() const { return m_mode3d; }
	// Posición de la herramienta tras la línea actual (false si no hay tramos)
	bool CurrentToolPosition(CncPoint &out) const;

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

	// Pieza
	bool m_show_stock = true;
	ToolTable m_tools;
	StockDefinition m_stock_def;
	TurnStock m_full_stock;     // programa completo
	TurnStock m_stock;          // hasta la línea actual
	int m_stock_line = -2;      // línea con la que se calculó m_stock

	// Vista 3D
	wxTurnView3D *m_view3d = NULL;
	bool m_mode3d = false;
	void UpdateView3D();

	// Medición
	std::vector<CncPoint> m_snap_points;
	bool m_snapped = false;
	CncPoint m_snap;
	bool m_have_first = false;
	CncPoint m_first;
	std::vector<PlotDimension> m_dims;

	double m_scale = 2;            // píxeles por milímetro
	double m_ox = 0, m_oy = 0;     // posición en pantalla del origen (Z0, X0)
	bool m_user_view = false;      // el usuario movió o acercó: no reencuadrar al cambiar el tamaño

	bool m_dragging = false;
	bool m_moved = false;          // hubo arrastre desde el último botón apretado
	wxPoint m_press;
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

	void RecomputeStock();
	void UpdateSnapPoints();
	void UpdateSnap();
	void ClickAt(const wxPoint &p);

	void DrawStock(wxGraphicsContext *gc);
	void DrawGrid(wxGraphicsContext *gc);
	void DrawRefs(wxGraphicsContext *gc);
	void DrawPath(wxGraphicsContext *gc);
	void DrawToolMarker(wxGraphicsContext *gc);
	void DrawMeasurements(wxGraphicsContext *gc);
	void DrawOverlay(wxGraphicsContext *gc);
	void DrawLabel(wxGraphicsContext *gc, const wxString &text, double x, double y, const wxColour &colour);
	void AddSegment(wxGraphicsPath &path, const CncSegment &s) const;
	double LineWidth(double dip) const;
};

#endif
