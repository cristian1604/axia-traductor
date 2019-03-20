#ifndef FILEMANAGER_H
#define FILEMANAGER_H
#include <wx/string.h>
#include <fstream>
#include "wxOptions.h"
using namespace std;

class FileManager {
private:
	wxString path;
	wxString filename;
	wxString content;
public:
	FileManager(wxString path, wxString filename);
	FileManager();
	bool readFile(wxString &content);
	bool writeFile(wxString &content);
	bool saveSettings(s_Settings &settings);
	bool loadSettings(s_Settings &settings);
};


#endif

