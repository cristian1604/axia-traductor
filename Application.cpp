#include <wx/image.h>
#include <wx/stdpaths.h>
#include "Application.h"
#include "MainWindow.h"

IMPLEMENT_APP(Application)

bool Application::OnInit() {
	SetAppName(wxT("axia-traductor"));
#ifndef __WXMSW__
	// Configuración en ~/.config en lugar de directamente en el home
	wxStandardPaths::Get().SetFileLayout(wxStandardPaths::FileLayout_XDG);
#endif
	wxInitAllImageHandlers();
	MainWindow *win = new MainWindow(NULL);
	win->Show();
	return true;
}

