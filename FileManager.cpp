#include "FileManager.h"
#include <iostream>
using namespace std;

FileManager::FileManager(wxString path, wxString filename) {
	this->path = path;
	this->filename = filename;
	this->defined = false;
}

FileManager::FileManager() {
}

bool FileManager::readFile(wxString &content) {
	content.Clear();
	fstream file(this->path, ios::in);
	string line;
	if (file.is_open()) {
		while (getline(file,line)) {
			content.Append(line);
			content.Append('\n');
		}
		this->defined = true;
		file.close();
		return true;
	}
	return false;
}


bool FileManager::writeFile(wxString &content) {
	fstream file(this->path + "\\" + this->filename, ios::out);
	if (file.is_open()) {
		file<<content;
		file.close();
		return true;
	}
	return false;
}

bool FileManager::loadSettings(s_Settings &s) {
	fstream file("settings.dat", ios::binary|ios::in);
	if (file.is_open()) {
		while (!file.eof()) {
			file.read((char *) &s, sizeof(s_Settings));
		}
		file.sync();
		file.close();
		return true;
	}
	return false;
}


bool FileManager::saveSettings(s_Settings &s) {
	fstream file("settings.dat", ios::binary|ios::out);
	if (file.is_open()) {
		file.write((char *) &s, sizeof(s_Settings));
		file.sync();
		file.close();
		return true;
	}
	return false;
}

bool FileManager::isDefined() {
	return this->defined;
}

string FileManager::getFilename() {
	return (string) this->filename.mb_str();
}

string FileManager::getPath() {
	return (string) this->path.mb_str();
}
