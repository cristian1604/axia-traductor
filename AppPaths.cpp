#include "AppPaths.h"
#include <wx/stdpaths.h>
#include <wx/filename.h>
#include <wx/filefn.h>

static wxString app_config_dir() {
	wxString dir = wxStandardPaths::Get().GetUserConfigDir()
		+ wxFileName::GetPathSeparator() + wxT("axia-traductor");
	if (!wxDirExists(dir)) wxMkdir(dir);
	return dir;
}

wxString user_settings_file() {
	return app_config_dir() + wxFileName::GetPathSeparator() + wxT("settings.json");
}

wxString shared_config_file(const wxString &name) {
	wxFileName exe(wxStandardPaths::Get().GetExecutablePath());
	return exe.GetPath() + wxFileName::GetPathSeparator() + name;
}

wxString temp_dir() {
	wxString dir = wxStandardPaths::Get().GetTempDir()
		+ wxFileName::GetPathSeparator() + wxT("axia-traductor");
	if (!wxDirExists(dir)) wxMkdir(dir);
	return dir;
}

wxString temp_file(const wxString &name) {
	return temp_dir() + wxFileName::GetPathSeparator() + name;
}
