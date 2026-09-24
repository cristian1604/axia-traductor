#include "FileManager.h"
#include "AppPaths.h"
#include "json.hpp"
#include <fstream>
using namespace std;
using json = nlohmann::json;

static string colour_to_string(const wxColour &c) {
	return c.GetAsString(wxC2S_HTML_SYNTAX).ToStdString();
}

static wxColour colour_from_json(const json &j, const char *key, const wxColour &fallback) {
	if (!j.contains(key) || !j[key].is_string()) return fallback;
	wxColour c(wxString::FromUTF8(j[key].get<string>().c_str()));
	return c.IsOk() ? c : fallback;
}

FileManager::FileManager(const wxString &dir, const wxString &filename)
	: file(dir, filename), defined(false) {
}

FileManager::FileManager(const wxString &fullPath)
	: file(fullPath), defined(false) {
}

FileManager::FileManager() : defined(false) {
}

bool FileManager::readFile(wxString &content) {
	content.Clear();
	ifstream in(file.GetFullPath().ToStdString(), ios::in);
	if (!in.is_open()) return false;
	string line;
	while (getline(in, line)) {
		// Normaliza finales de línea CRLF (archivos generados en Windows)
		if (!line.empty() && line.back() == '\r') line.pop_back();
		content.Append(line);
		content.Append('\n');
	}
	this->defined = true;
	return true;
}

bool FileManager::writeFile(const wxString &content) {
	ofstream out(file.GetFullPath().ToStdString(), ios::out);
	if (!out.is_open()) return false;
	out << content.ToStdString();
	this->defined = out.good();
	return this->defined;
}

/**
	Configuracion por usuario en settings.json (ver AppPaths). Si el archivo no
	existe o esta danado, s queda con los valores por defecto y se devuelve false.
	(El formato anterior, settings.dat, volcaba los bytes de objetos wxColour
	y no era portable entre plataformas ni versiones de wxWidgets.)
**/
bool FileManager::loadSettings(s_Settings &s) {
	s = default_settings();
	ifstream in(user_settings_file().ToStdString());
	if (!in.is_open()) return false;
	json j;
	try {
		in >> j;
	} catch (const json::exception &) {
		return false;
	}
	if (!j.is_object()) return false;
	s.colour_textCtrl     = colour_from_json(j, "colour_background", s.colour_textCtrl);
	s.colour_text         = colour_from_json(j, "colour_text", s.colour_text);
	s.colour_comments     = colour_from_json(j, "colour_comments", s.colour_comments);
	s.colour_command_m    = colour_from_json(j, "colour_command_m", s.colour_command_m);
	s.colour_command_tool = colour_from_json(j, "colour_command_tool", s.colour_command_tool);
	s.colour_line_number  = colour_from_json(j, "colour_line_number", s.colour_line_number);
	s.maximize_on_startup = j.value("maximize_on_startup", s.maximize_on_startup);
	s.remove_m08          = j.value("remove_m08", s.remove_m08);
	s.replace_from        = wxString::FromUTF8(j.value("replace_from", string()).c_str());
	s.replace_to          = wxString::FromUTF8(j.value("replace_to", string()).c_str());
	s.last_machine        = wxString::FromUTF8(j.value("last_machine", string()).c_str());
	s.last_filename       = wxString::FromUTF8(j.value("last_filename", string()).c_str());
	s.close_after_transfer = j.value("close_after_transfer", s.close_after_transfer);
	s.plot_visible        = j.value("plot_visible", s.plot_visible);
	s.plot_below          = j.value("plot_below", s.plot_below);
	s.plot_sash_right     = j.value("plot_sash_right", s.plot_sash_right);
	s.plot_sash_below     = j.value("plot_sash_below", s.plot_sash_below);
	s.plot_rapids         = j.value("plot_rapids", s.plot_rapids);
	return true;
}

bool FileManager::saveSettings(s_Settings &s) {
	json j;
	j["colour_background"]   = colour_to_string(s.colour_textCtrl);
	j["colour_text"]         = colour_to_string(s.colour_text);
	j["colour_comments"]     = colour_to_string(s.colour_comments);
	j["colour_command_m"]    = colour_to_string(s.colour_command_m);
	j["colour_command_tool"] = colour_to_string(s.colour_command_tool);
	j["colour_line_number"]  = colour_to_string(s.colour_line_number);
	j["maximize_on_startup"] = s.maximize_on_startup;
	j["remove_m08"]          = s.remove_m08;
	j["replace_from"]        = string(s.replace_from.ToUTF8());
	j["replace_to"]          = string(s.replace_to.ToUTF8());
	j["last_machine"]        = string(s.last_machine.ToUTF8());
	j["last_filename"]       = string(s.last_filename.ToUTF8());
	j["close_after_transfer"] = s.close_after_transfer;
	j["plot_visible"]        = s.plot_visible;
	j["plot_below"]          = s.plot_below;
	j["plot_sash_right"]     = s.plot_sash_right;
	j["plot_sash_below"]     = s.plot_sash_below;
	j["plot_rapids"]         = s.plot_rapids;
	ofstream out(user_settings_file().ToStdString());
	if (!out.is_open()) return false;
	out << j.dump(4) << endl;
	return out.good();
}

bool FileManager::isDefined() const {
	return this->defined;
}

string FileManager::getFilename() const {
	return file.GetFullName().ToStdString();
}

string FileManager::getPath() const {
	return file.GetPath().ToStdString();
}

string FileManager::getFullPath() const {
	return file.GetFullPath().ToStdString();
}
