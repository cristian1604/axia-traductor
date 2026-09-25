#include "ToolsFile.h"
#include "json.hpp"
#include <fstream>
#include <sstream>
#include <cstdlib>

using json = nlohmann::json;

const char *tool_role_name(ToolRole role) {
	switch (role) {
	case TOOL_EXTERNAL: return "exterior";
	case TOOL_INTERNAL: return "interior";
	case TOOL_FACING:   return "frenteo";
	case TOOL_CUTOFF:   return "corte";
	case TOOL_NARROW:   return "angosta";
	default:            return "auto";
	}
}

bool tool_role_from_name(const std::string &name, ToolRole &out) {
	static const ToolRole roles[] = { TOOL_UNKNOWN, TOOL_EXTERNAL, TOOL_INTERNAL, TOOL_FACING, TOOL_CUTOFF, TOOL_NARROW };
	for (size_t k = 0; k < sizeof(roles) / sizeof(roles[0]); ++k) {
		if (name == tool_role_name(roles[k])) { out = roles[k]; return true; }
	}
	return false;
}

std::string tool_table_to_json(const ToolTable &table) {
	json root;
	root["default_width"] = table.default_width;
	root["default_nose_radius"] = table.default_nose_radius;
	json tools = json::object();
	for (std::map<int, ToolDef>::const_iterator it = table.tools.begin(); it != table.tools.end(); ++it) {
		json t;
		t["role"] = tool_role_name(it->second.role);
		if (it->second.width > 0) t["width"] = it->second.width;
		if (it->second.nose_radius > 0) t["nose_radius"] = it->second.nose_radius;
		tools[std::to_string(it->first)] = t;
	}
	root["tools"] = tools;
	return root.dump(4) + "\n";
}

bool parse_tool_table(const std::string &json_text, ToolTable &out, std::string *error) {
	if (error) error->clear();
	try {
		json j = json::parse(json_text);
		ToolTable t;   // sin la convención por defecto: el archivo la reemplaza entera
		t.default_width = j.value("default_width", 3.0);
		t.default_nose_radius = j.value("default_nose_radius", 0.4);
		if (t.default_width <= 0 || t.default_nose_radius <= 0) throw std::runtime_error("default_width y default_nose_radius deben ser positivos");
		const json &tools = j.at("tools");
		if (!tools.is_object()) throw std::runtime_error("\"tools\" debe ser un objeto con el número de herramienta como clave");
		for (json::const_iterator it = tools.begin(); it != tools.end(); ++it) {
			char *end = NULL;
			long number = strtol(it.key().c_str(), &end, 10);
			if (!end || *end != 0 || number < 0) throw std::runtime_error("número de herramienta no válido: " + it.key());
			ToolDef d;
			std::string role = it.value().value("role", "auto");
			if (!tool_role_from_name(role, d.role)) throw std::runtime_error("herramienta " + it.key() + ": role \"" + role + "\" desconocido");
			d.width = it.value().value("width", 0.0);
			d.nose_radius = it.value().value("nose_radius", 0.0);
			if (d.width < 0 || d.nose_radius < 0) throw std::runtime_error("herramienta " + it.key() + ": width y nose_radius no pueden ser negativos");
			t.tools[(int) number] = d;
		}
		out = t;
		return true;
	} catch (const std::exception &e) {
		if (error) *error = e.what();
		return false;
	}
}

ToolTable load_tool_table(const std::string &path, std::string *error) {
	if (error) error->clear();
	std::ifstream in(path.c_str());
	if (!in.is_open()) {
		ToolTable t = default_tool_table();
		save_tool_table(path, t);   // crea el archivo para que se pueda editar
		return t;
	}
	std::stringstream ss;
	ss << in.rdbuf();
	ToolTable t;
	std::string why;
	if (!parse_tool_table(ss.str(), t, &why)) {
		if (error) *error = "No se pudo leer " + path + ": " + why;
		return default_tool_table();
	}
	return t;
}

bool save_tool_table(const std::string &path, const ToolTable &table) {
	std::ofstream out(path.c_str());
	if (!out.is_open()) return false;
	out << tool_table_to_json(table);
	return out.good();
}
