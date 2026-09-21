#ifndef RENUMBER_H
#define RENUMBER_H
#include <string>
#include <vector>

/**
	Reenumeración de líneas de un programa CNC.

	Reimplementación nativa del antiguo Num2.exe (Borland C++ 4, Win32), que
	trabajaba a través del portapapeles. Comportamiento reproducido:

	- Todo lo anterior a la línea que contiene el primer '%' (inclusive) se
	  copia sin cambios. Si no hay '%', no se reenumera.
	- Cada línea posterior (también las vacías y los comentarios) se emite como
	  "Nxxxx  <contenido>", descartando la etiqueta N original y los espacios
	  iniciales.
	- El paso y el número inicial dependen de la cantidad de líneas:
	      < 800 líneas: inicio 10, paso 10
	     < 1600 líneas: inicio 10, paso 5
	     < 4000 líneas: inicio 2,  paso 2
	    < 10000 líneas: inicio 0,  paso 1
	  Con 10000 o más líneas no se reenumera.
	- Los saltos "G25".."G29" seguidos de "N<a>" o "N<a>.<b>" se actualizan con
	  el nuevo número de la línea destino. Si algún destino no existe, no se
	  reenumera y se informa la lista de destinos indefinidos.
**/
struct RenumberResult {
	bool ok = false;
	std::string text;                 // programa reenumerado (si ok)
	std::string error;                // descripción del problema (si !ok)
	std::vector<int> undefined;       // destinos de salto no encontrados (si !ok)
};

RenumberResult renumber_program(const std::string &input);

/**
	Reenumera solo a partir de la primera aparición de `marker` (por ejemplo
	"N0010"), conservando intacto todo lo anterior. Se usa con programas 8035
	ya traducidos, cuyos comentarios de cabecera no deben numerarse.
**/
RenumberResult renumber_program_from(const std::string &input, const std::string &marker);

#endif
