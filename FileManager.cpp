#include "FileManager.h"
#include <iostream>
using namespace std;

FileManager::FileManager(wxString path, wxString filename) {
	this->path = path;
	this->filename = filename;
}

bool FileManager::readFile(wxString &content) {
	fstream file(this->path, ios::in);
	string line;
	if (file.is_open()) {
		while (getline(file,line)) {
			content.Append(line);
			content.Append('\n');
		}
		file.close();
		return true;
	}
	return false;
}

