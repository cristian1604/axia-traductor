#ifndef CNCSYNTAX_H
#define CNCSYNTAX_H
#include <string>
#include <vector>
#include "CncStandard.h"

/**
	Clasificación léxica de programas CNC para el coloreado del editor. Sin
	dependencias de wxWidgets, para poder probarla (tests/test_syntax.cpp) y
	reutilizarla con cualquier otro sistema de interfaz.
**/
enum CncStyle {
	STYLE_TEXT    = 0,   // cotas, G, F, S, P, variables #, etc.
	STYLE_COMMENT = 1,   // comentarios y cabecera previa al inicio del programa
	STYLE_HEADER  = 2,   // inicio (y fin) de programa: línea "%..." u "Onnnn" en FANUC
	STYLE_LABEL   = 3,   // etiqueta de bloque Nnnnn
	STYLE_M       = 4,   // funciones auxiliares Mnn
	STYLE_TOOL    = 5    // herramienta y corrector: Tnn, Tnn.nn, Dnn
};

/**
	Devuelve el estilo (CncStyle) de cada byte de `text`, en un vector del
	mismo tamaño. `standard` es un valor de CncStandard:
	- 8035 / 8037: el comentario empieza con ';' y llega al fin de línea.
	- 8025 y FANUC: el comentario va entre paréntesis.
	Todo lo anterior a la línea de inicio del programa (la primera que empieza
	con '%' o, en FANUC, con 'O' seguida de dígitos) es cabecera y se pinta
	como comentario. Una palabra es una letra seguida de dígitos, y solo se
	colorea si empieza un token (no las letras dentro de "VITON" o "GOTO").
**/
std::vector<unsigned char> classify_cnc(const std::string &text, int standard);

#endif
