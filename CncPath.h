#ifndef CNCPATH_H
#define CNCPATH_H
#include <string>
#include <vector>
#include "CncStandard.h"

/**
	Intérprete de trayectorias: convierte un programa CNC de torno (Fagor 8025,
	8035/8037 o FANUC) en segmentos (rápidos, rectas y arcos) en el plano Z/X,
	cada uno con la línea del programa que lo generó. Sin dependencias de
	wxWidgets, para probarlo con programas reales (tests/test_path.cpp) y
	reutilizarlo con cualquier interfaz.

	Coordenadas internas: z y r (radio). X se programa y se muestra en
	diámetro, así que r = X/2. Vista de referencia: Z hacia la derecha, X
	hacia arriba; en ella G03 es antihorario y G02 horario.

	Las reglas del dialecto (ángulos, A1 A2 diferido, arcos por R o I/K,
	G36/G39, letra sin número, encabezado #DN/#DA/#HN/#HA/#ODR/#IDR) salen del
	simulador anterior: ver docs/absim-analisis.md.
**/
struct CncPoint {
	double z = 0, r = 0;
	double x() const { return 2 * r; }   // diámetro
};

enum CncSegmentKind {
	SEG_RAPID = 0,   // G00: se dibuja punteado
	SEG_LINE  = 1,   // G01, G33 (rosca) y chaflanes
	SEG_ARC_CW  = 2, // G02 y redondeos horarios
	SEG_ARC_CCW = 3  // G03 y redondeos antihorarios
};

struct CncSegment {
	CncSegmentKind kind = SEG_LINE;
	CncPoint from, to;
	CncPoint center;         // solo arcos
	double radius = 0;       // solo arcos
	int line = 0;            // índice (desde 0) de la línea del programa
	bool unresolved = false; // alguna cota era paramétrica: el punto final no es confiable
	// Herramienta activa: el número T o, si T es 0, el corrector (T0.02 -> 2,
	// D03 -> 3, T0202 -> 2), que es como el taller identifica cada herramienta
	int tool = 0;
	bool is_arc() const { return kind == SEG_ARC_CW || kind == SEG_ARC_CCW; }
};

// Líneas de referencia del encabezado (#DN, #DA, #HN, #HA)
struct CncRefLine {
	bool vertical = false;   // true: z = value (#HN/#HA); false: r = value (#DN/#DA, ya en radio)
	bool nominal = true;     // #DN/#HN (trazo-punto) o #DA/#HA (trazo)
	double value = 0;
	int line = 0;
};

// Tubo en bruto (#ODR, #IDR), en diámetro
struct CncStock {
	double outer_diameter = 0, inner_diameter = 0;
	bool valid() const { return outer_diameter > 0; }
};

struct CncMessage {
	int line = 0;
	bool error = false;      // false: aviso (ciclo no soportado, cota paramétrica, etc.)
	std::string text;
};

struct CncBounds {
	double zmin = 0, zmax = 0, rmin = 0, rmax = 0;
	bool valid = false;
	void add(const CncPoint &p);
	void add(double z, double r);
};

struct CncPath {
	std::vector<CncSegment> segments;   // en orden de programa; `line` no decrece
	std::vector<CncRefLine> refs;
	CncStock stock;
	// Del encabezado del generador: "T4  70  110" (cuchilla de corte y tubo
	// interior/exterior) y "P05A  6  84.5  100" (nombre, largo y diámetros de la
	// pieza). Se leen antes del inicio del programa, también precedidos de ';'
	// (traducción a 8035) o entre paréntesis (traducción a FANUC).
	int cutoff_tool = 0;
	std::string part_name;
	double part_length = 0;
	std::vector<CncMessage> messages;

	// Caja que contiene rectas y arcos de avance (no los rápidos ni los tramos
	// no resueltos): es lo que conviene encuadrar. Los arcos aportan sus extremos reales.
	CncBounds feed_bounds() const;
	bool has_errors() const;
};

// Posición de la herramienta antes del primer bloque: X400 Z830, como en el
// simulador anterior. Queda fuera de feed_bounds() porque es un rápido.
CncPoint cnc_initial_position();

// `standard` es un valor de CncStandard. Nunca lanza excepciones: los
// problemas quedan en `messages` y el intérprete sigue con el bloque siguiente.
CncPath interpret_cnc(const std::string &text, int standard);

#endif
