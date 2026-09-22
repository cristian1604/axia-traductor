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
};

// Valores por defecto de la configuracion
s_Settings default_settings();

#endif
