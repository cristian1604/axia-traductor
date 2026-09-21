#ifndef FILEMANAGER_H
#define FILEMANAGER_H
#include <wx/string.h>
#include <wx/filename.h>
#include <string>
#include "wxOptions.h"

class FileManager {
private:
	wxFileName file;
	bool defined;
public:
	FileManager(const wxString &dir, const wxString &filename);
	explicit FileManager(const wxString &fullPath);
	FileManager();
	bool readFile(wxString &content);
	bool writeFile(const wxString &content);
	bool saveSettings(s_Settings &settings);
	bool loadSettings(s_Settings &settings);
	std::string getFilename() const;
	std::string getPath() const;
	std::string getFullPath() const;
	bool isDefined() const;
};

#endif
