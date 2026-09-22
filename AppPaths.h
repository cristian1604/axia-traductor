#ifndef APPPATHS_H
#define APPPATHS_H
#include <wx/string.h>

// Ubicación de los archivos de configuración.
// - Por usuario (preferencias): carpeta de configuración del perfil
//   (AppData\Roaming\axia-traductor en Windows, ~/.config/axia-traductor en Linux).
// - Compartida (lista de tornos): junto al ejecutable, que suele estar en un
//   servidor de archivos común a todos los usuarios.
wxString user_settings_file();
wxString shared_config_file(const wxString &name);

// Carpeta temporal local de cada PC (%TEMP%\axia-traductor en Windows,
// /tmp/axia-traductor en Linux) para las copias que se suben al torno o se
// pasan al simulador. Antes iba a "tmp/" junto al ejecutable: en el servidor
// de archivos la compartían todas las PC y exigía permiso de escritura.
wxString temp_dir();
wxString temp_file(const wxString &name);

#endif
