#ifndef FILEMANAGER_H
#define FILEMANAGER_H
#include <wx/string.h>
#include <fstream>
using namespace std;

class FileManager {
private:
	wxString path;
	wxString filename;
	wxString content;
public:
	FileManager(wxString path, wxString filename);
	bool readFile(wxString &content);
	bool writeFile(wxString &content);
};


#endif

