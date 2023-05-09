#ifndef FILEMANAGER_H
#define FILEMANAGER_H
#include <wx/string.h>
#include <fstream>
#include "wxOptions.h"
#include <string>
using namespace std;

class FileManager {
private:
	wxString path;
	wxString filename;
	wxString content;
	bool defined;
public:
	FileManager(wxString path, wxString filename);
	FileManager();
	bool readFile(wxString &content);
	bool writeFile(wxString &content);
	bool saveSettings(s_Settings &settings);
	bool loadSettings(s_Settings &settings);
	string getFilename();
	string getPath();
	bool isDefined();
};


#endif

