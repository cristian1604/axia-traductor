#ifndef TURNSTOCK_H
#define TURNSTOCK_H
#include <map>
#include <vector>
#include "CncPath.h"

/**
	Simulación de la pieza torneada: la media sección (plano Z/R) del tubo en
	bruto menos lo que barre la herramienta en cada avance, con booleanas de
	polígonos exactas (Clipper2). Los vértices del resultado son los puntos
	reales de la geometría, aptos para medir. Sin wxWidgets; tests en
	tests/test_stock.cpp. Ver docs/plan-3d.md.

	La herramienta se modela por su función, no por su catálogo: exterior e
	interior barren una cuña abierta hacia +Z desde la punta (filo principal
	casi radial, filo secundario a 52° del eje), hacia afuera o hacia el eje,
	así la superficie que queda es exactamente la trayectoria programada y el
	cuerpo no socava lo ya torneado; las cuchillas de corte y ranurado quitan
	una franja de ancho `width` hacia el plato desde el punto programado.
**/

enum ToolRole {
	TOOL_UNKNOWN = 0,   // se deduce: interior si la primera pasada está más cerca del eje que de la pared
	TOOL_EXTERNAL,
	TOOL_INTERNAL,
	TOOL_FACING,
	TOOL_CUTOFF         // corte y ranurado
};

struct ToolDef {
	ToolRole role = TOOL_UNKNOWN;
	double width = 0;   // ancho de la cuchilla (corte y ranurado); 0 = el ancho por defecto
};

// Tabla por número de herramienta. Por defecto, la convención del taller:
// 1, 4 y 5 corte; 2 exterior; 3 y 7 interior; 6 frenteado; el resto se deduce.
struct ToolTable {
	std::map<int, ToolDef> tools;
	double default_width = 3;   // ancho supuesto de las cuchillas sin definir
	ToolDef get(int tool) const;
	double width_of(int tool) const;
};
ToolTable default_tool_table();

struct StockDefinition {
	double outer_diameter = 0, inner_diameter = 0;
	double z_face = 0;   // cara del bruto: el material está en z <= z_face
	double z_end = 0;    // hasta dónde se representa el tubo hacia el plato
	bool deduced = false;   // sin encabezado: diámetros estimados de los avances
	bool valid() const { return outer_diameter > 0 && z_face > z_end; }
};

// Bruto a partir del programa: diámetros del encabezado o, sin encabezado,
// estimados: exterior un poco mayor que el mayor avance, interior el fondo del
// tronzado (que siempre atraviesa la pared) o un poco menor que el menor
// avance. Largo de la pieza del encabezado o de los avances.
StockDefinition stock_from_path(const CncPath &path, const ToolTable &tools);

typedef std::vector<CncPoint> Ring;   // polígono cerrado en (z, r)

// Ranura hecha con una cuchilla de ancho supuesto (no atraviesa la pared)
struct AssumedGroove {
	double z_from = 0, z_to = 0, r_from = 0, r_to = 0;
	int line = 0;
	int tool = 0;
};

struct TurnStock {
	StockDefinition stock;
	std::vector<Ring> rings;             // sección actual; regla par-impar para agujeros
	std::vector<AssumedGroove> assumed;
	std::vector<CncMessage> messages;    // rápido dentro del material
	CncPoint tool_position;              // posición de la herramienta al final de la simulación
	bool cut_off = false;                // hubo tronzado completo: queda solo la pieza
	double cut_z = 0;
	double area() const;
	std::vector<CncPoint> vertices() const;   // para enganchar mediciones
};

// Simula hasta la línea `up_to_line` inclusive (-1: el programa completo)
TurnStock simulate_stock(const CncPath &path, const StockDefinition &stock, const ToolTable &tools, int up_to_line = -1);

#endif
