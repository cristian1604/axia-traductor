#ifndef TOOLSFILE_H
#define TOOLSFILE_H
#include <string>
#include "TurnStock.h"

/**
	Tabla de herramientas del simulador de pieza en tools.json, junto al
	ejecutable y compartida por todos los usuarios como machines.json. Solo
	hace falta tocarla si una cuchilla tiene otro ancho o una herramienta
	especial otra función que la de la convención del taller. Formato:

	{
	    "default_width": 3.0,          ancho supuesto de las cuchillas
	    "default_nose_radius": 0.4,    radio de punta de las angostas sin arcos
	    "tools": {
	        "1": { "role": "corte", "width": 2.5 },
	        "2": { "role": "exterior" },
	        "6": { "role": "angosta", "nose_radius": 0.8 }
	    }
	}
	Roles: "exterior", "interior", "frenteo", "corte", "angosta" y "auto"
	(se deduce de la posición de la primera pasada).
**/

// Carga la tabla. Si el archivo no existe, lo crea con la convención del
// taller. Si está dañado, devuelve la tabla por defecto y describe el
// problema en `error`.
ToolTable load_tool_table(const std::string &path, std::string *error = nullptr);
bool save_tool_table(const std::string &path, const ToolTable &table);

// Las mismas conversiones sobre texto JSON, para los tests
bool parse_tool_table(const std::string &json_text, ToolTable &out, std::string *error = nullptr);
std::string tool_table_to_json(const ToolTable &table);

const char *tool_role_name(ToolRole role);
bool tool_role_from_name(const std::string &name, ToolRole &out);

#endif
