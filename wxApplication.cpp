#include <wx/image.h>
#include <wx/stdpaths.h>
#include <wx/filename.h>
#include <wx/filefn.h>
#include "wxApplication.h"
#include "wxMainWindow.h"

IMPLEMENT_APP(wxApplication)

bool wxApplication::OnInit() {
	SetAppName(wxT("axia-traductor"));
#ifndef __WXMSW__
	// Configuración en ~/.config en lugar de directamente en el home
	wxStandardPaths::Get().SetFileLayout(wxStandardPaths::FileLayout_XDG);
#endif
	// Los iconos (resources/), las plantillas y la carpeta temporal se buscan
	// relativos al directorio de trabajo: se fija en la carpeta del ejecutable
	// para que dé igual desde dónde se lance (acceso directo sin "Iniciar en",
	// línea de comandos, otro directorio). Sin esto no aparecía ningún icono.
	wxFileName exe(wxStandardPaths::Get().GetExecutablePath());
	wxSetWorkingDirectory(exe.GetPath());
	wxInitAllImageHandlers();
	wxMainWindow *win = new wxMainWindow(NULL);
	win->Show();
	return true;
}
