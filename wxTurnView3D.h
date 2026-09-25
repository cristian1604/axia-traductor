#ifndef WXTURNVIEW3D_H
#define WXTURNVIEW3D_H
#include <wx/glcanvas.h>
#include <vector>
#include "CncPath.h"
#include "TurnStock.h"
#include "wxPlotPanel.h"

/**
	Vista 3D de la pieza torneada: la sección que deja simulate_stock girada
	alrededor del eje Z, con OpenGL 1.1 de función fija (corre en cualquier PC
	de planta, incluso con render por software). Es un hijo de wxPlotPanel,
	que lo muestra en lugar del dibujo 2D y le pasa la escena.

	Ejes: el Z del torno es horizontal; la vista en corte quita la mitad de
	adelante y muestra las caras de la sección, que en la mitad superior es la
	misma que dibuja el 2D. Arrastre izquierdo: orbitar; derecho o central:
	desplazar; rueda: zoom; Inicio: encuadrar; C: vista en corte; T: trayectoria.
**/
class wxTurnView3D : public wxGLCanvas {
public:
	static bool IsSupported();
	wxTurnView3D(wxWindow *parent);
	~wxTurnView3D();

	// Escena: sección actual, recorrido (se dibuja en el plano de corte hasta la
	// línea actual) y posición de la herramienta
	void SetScene(const TurnStock &stock, const CncPath &path, int current_line, const CncPoint &tool);
	void SetColours(const PlotColours &c);
	void SetCutAway(bool cut);
	bool GetCutAway() const { return m_cut_away; }
	void SetShowPath(bool show);
	void ResetCamera();

private:
	wxGLContext *m_context;
	bool m_context_failed = false;
	PlotColours m_colours;
	bool m_cut_away = true;
	bool m_show_path = true;
	bool m_gl_ready = false;

	// Malla: triángulos con normales (x, y, z por vértice)
	std::vector<float> m_verts, m_norms;   // superficie de revolución
	std::vector<float> m_caps;             // caras de la sección en el plano de corte
	std::vector<float> m_path_feed, m_path_rapid;   // segmentos (pares de puntos)
	CncPoint m_tool;
	bool m_have_tool = false;
	double m_centre[3] = { 0, 0, 0 };
	double m_radius = 50;                  // radio de la escena, para la cámara

	// Cámara orbital alrededor de m_target
	double m_yaw = 35, m_pitch = 22, m_distance = 150;
	double m_target[3] = { 0, 0, 0 };
	bool m_dragging = false;
	int m_drag_button = 0;
	wxPoint m_last;

	TurnStock m_stock;
	CncPath m_path;
	int m_current = -1;

	void OnPaint(wxPaintEvent &event);
	void OnSize(wxSizeEvent &event);
	void OnEraseBackground(wxEraseEvent &event);
	void OnMouseDown(wxMouseEvent &event);
	void OnMouseUp(wxMouseEvent &event);
	void OnMouseMove(wxMouseEvent &event);
	void OnWheel(wxMouseEvent &event);
	void OnKey(wxKeyEvent &event);
	void OnCaptureLost(wxMouseCaptureLostEvent &event);

	void BuildMesh();
	void Render();
	void InitGL();
};

#endif
