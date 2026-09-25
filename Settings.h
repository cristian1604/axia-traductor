#ifndef SETTINGS_H
#define SETTINGS_H
#include <wx/colour.h>
#include <wx/string.h>

// Configuración por usuario (settings.json, ver FileManager). Independiente
// del diálogo de opciones para que el editor y el resto no dependan de él.
struct s_Settings {
	wxColour colour_textCtrl;
	wxColour colour_text;
	wxColour colour_comments;
	wxColour colour_command_m;
	wxColour colour_command_tool;
	wxColour colour_line_number;

	bool maximize_on_startup;
	bool remove_m08;
	wxString replace_from;
	wxString replace_to;

	// Envío a torno (se recuerdan por usuario)
	wxString last_machine;
	wxString last_filename;
	bool close_after_transfer;

	// Graficador (se cambia desde su menú, no desde el diálogo de opciones)
	bool plot_visible;       // mostrar el graficador junto al editor
	bool plot_below;         // debajo del editor (si no, a la derecha)
	int  plot_sash_right;    // posición del divisor en píxeles lógicos (0 = automática)
	int  plot_sash_below;
	bool plot_rapids;        // dibujar los rápidos
	bool plot_stock;         // simular y dibujar la pieza (sección con material)
	bool plot_3d;            // vista 3D en lugar del dibujo 2D
};

// Valores por defecto de la configuracion
s_Settings default_settings();

#endif
