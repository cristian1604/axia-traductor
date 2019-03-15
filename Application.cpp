#include <wx/image.h>
#include "Application.h"
#include "MainWindow.h"

IMPLEMENT_APP(Application)

bool Application::OnInit() {
	wxInitAllImageHandlers();
	MainWindow *win = new MainWindow(NULL);
	win->Show();
	return true;
}

