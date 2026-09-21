#include "Machines.h"
#include "json.hpp"
#include <fstream>

using json = nlohmann::json;

std::vector<Machine> default_machines() {
	std::vector<Machine> v;
	v.push_back({ "WASINO 8037",   "ftp",       "192.168.100.82",  21,    "/disk/prg/" });
	v.push_back({ "WASINO 8035",   "ftp",       "192.168.100.81",  21,    "/disk/prg/" });
	v.push_back({ "TAKISAWA 8037", "ftp",       "192.168.100.80",  21,    "/disk/prg/" });
	v.push_back({ "KIA FANUC",     "fanuc-udp", "192.168.100.182", 27182, "" });
	return v;
}

std::vector<Machine> load_machines(const std::string &path, std::string *error) {
	if (error) error->clear();
	std::ifstream in(path.c_str());
	if (!in.is_open()) {
		std::vector<Machine> v = default_machines();
		save_machines(path, v);   // crea el archivo para que se pueda editar
		return v;
	}
	json j;
	try {
		in >> j;
		std::vector<Machine> v;
		for (const json &m : j.at("machines")) {
			Machine mc;
			mc.name = m.at("name").get<std::string>();
			mc.protocol = m.value("protocol", "ftp");
			mc.ip = m.at("ip").get<std::string>();
			mc.port = m.value("port", mc.protocol == "ftp" ? 21 : 27182);
			mc.directory = m.value("directory", "");
			v.push_back(mc);
		}
		if (v.empty()) throw std::runtime_error("la lista de tornos está vacía");
		return v;
	} catch (const std::exception &e) {
		if (error) *error = std::string("No se pudo leer ") + path + ": " + e.what();
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
