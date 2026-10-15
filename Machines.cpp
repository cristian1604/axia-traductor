#include "Machines.h"
#include "json.hpp"
#include <fstream>
#include <stdexcept>

using json = nlohmann::json;

std::vector<Machine> default_machines() {
	std::vector<Machine> v;
	v.push_back({ "WASINO 8037",   "ftp",       "192.168.100.82",  21,    "/disk/prg/", "" });
	v.push_back({ "WASINO 8035",   "ftp",       "192.168.100.81",  21,    "/disk/prg/", "" });
	v.push_back({ "TAKISAWA 8037", "ftp",       "192.168.100.80",  21,    "/disk/prg/", "" });
	// Instalado en W:\AB\EXE32\axia-traductor, el ini queda en W:\AB\Inis
	v.push_back({ "KIA FANUC",     "fanuc-udp", "192.168.100.188", 27182, "",           "..\\..\\Inis\\ab.ini" });
	return v;
}

static bool is_absolute_path(const std::string &p) {
	return (p.size() >= 2 && p[1] == ':') || (!p.empty() && (p[0] == '\\' || p[0] == '/'));
}

// Carpeta de `file` con el separador final, o "" si no tiene carpeta
static std::string directory_of(const std::string &file) {
	size_t slash = file.find_last_of("/\\");
	return slash == std::string::npos ? "" : file.substr(0, slash + 1);
}

// Lee la lista de `path`. Si el archivo contiene solo {"path": "..."} la lista
// se lee de esa otra ruta (una carpeta de red común a todas las PC); se sigue
// un único salto. Los errores llevan la ruta del archivo que falló.
static std::vector<Machine> read_machines(const std::string &path, bool follow) {
	std::ifstream in(path.c_str());
	if (!in.is_open()) throw std::runtime_error("no se pudo abrir " + path);
	try {
		json j;
		in >> j;
		if (j.is_object() && j.contains("path") && !j.contains("machines")) {
			std::string target = j.at("path").get<std::string>();
			if (!follow) throw std::runtime_error("apunta a su vez a " + target);
			return read_machines(target, false);
		}
		std::vector<Machine> v;
		for (const json &m : j.at("machines")) {
			Machine mc;
			mc.name = m.at("name").get<std::string>();
			mc.protocol = m.value("protocol", "ftp");
			mc.ini = m.value("ini", "");
			if (!mc.ini.empty() && !is_absolute_path(mc.ini)) mc.ini = directory_of(path) + mc.ini;
			mc.ip = mc.ini.empty() ? m.at("ip").get<std::string>() : m.value("ip", "");
			mc.port = m.value("port", mc.protocol == "ftp" ? 21 : 27182);
			mc.directory = m.value("directory", "");
			v.push_back(mc);
		}
		if (v.empty()) throw std::runtime_error("la lista de tornos está vacía");
		return v;
	} catch (const std::exception &e) {
		throw std::runtime_error(path + ": " + e.what());
	}
}

std::vector<Machine> load_machines(const std::string &path, std::string *error) {
	if (error) error->clear();
	if (!std::ifstream(path.c_str()).is_open()) {
		std::vector<Machine> v = default_machines();
		save_machines(path, v);   // crea el archivo para que se pueda editar
		return v;
	}
	try {
		return read_machines(path, true);
	} catch (const std::exception &e) {
		if (error) *error = std::string("No se pudo leer ") + e.what();
		return default_machines();
	}
}

bool save_machines(const std::string &path, const std::vector<Machine> &machines) {
	json list = json::array();
	for (const Machine &m : machines) {
		json j;
		j["name"] = m.name;
		j["protocol"] = m.protocol;
		j["ip"] = m.ip;
		j["port"] = m.port;
		if (!m.directory.empty()) j["directory"] = m.directory;
		if (!m.ini.empty()) j["ini"] = m.ini;
		list.push_back(j);
	}
	json root;
	root["machines"] = list;
	std::ofstream out(path.c_str());
	if (!out.is_open()) return false;
	out << root.dump(4) << std::endl;
	return out.good();
}

const Machine *find_machine(const std::vector<Machine> &machines, const std::string &name) {
	for (size_t i = 0; i < machines.size(); ++i) {
		if (machines[i].name == name) return &machines[i];
	}
	return nullptr;
}
