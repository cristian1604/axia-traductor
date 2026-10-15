#include "Sender.h"
#include <SFML/Network.hpp>
#include <SFML/System.hpp>
#include <cctype>
#include <cstdlib>
#include <fstream>

static SendResult fail(const std::string &msg) {
	SendResult r;
	r.message = msg;
	return r;
}

static std::string trim(const std::string &s) {
	size_t b = s.find_first_not_of(" \t\r\n");
	size_t e = s.find_last_not_of(" \t\r\n");
	return (b == std::string::npos) ? "" : s.substr(b, e - b + 1);
}

static std::string upper(std::string s) {
	for (size_t i = 0; i < s.size(); ++i) s[i] = (char) toupper((unsigned char) s[i]);
	return s;
}

SendResult send_ftp(const Machine &machine, const std::string &localFile) {
	sf::IpAddress addr(machine.ip);
	if (addr == sf::IpAddress::None) return fail("Dirección IP inválida: " + machine.ip);

	sf::Ftp ftp;
	sf::Ftp::Response r = ftp.connect(addr, (unsigned short) machine.port, sf::seconds(5));
	if (!r.isOk()) return fail("No se pudo conectar con " + machine.name + ": " + r.getMessage());
	r = ftp.login();
	if (!r.isOk()) return fail("Error de login FTP: " + r.getMessage());
	if (!machine.directory.empty()) {
		r = ftp.changeDirectory(machine.directory);
		if (!r.isOk()) return fail("No se pudo abrir " + machine.directory + ": " + r.getMessage());
	}
	// Se borra el destino si existe para poder sobreescribirlo
	std::string remoteName = localFile;
	size_t slash = remoteName.find_last_of("/\\");
	if (slash != std::string::npos) remoteName = remoteName.substr(slash + 1);
	ftp.deleteFile(remoteName);

	r = ftp.upload(localFile, "", sf::Ftp::Binary);
	if (!r.isOk()) return fail("Error de transferencia: " + r.getMessage());
	ftp.disconnect();

	SendResult ok;
	ok.ok = true;
	ok.message = "Programa transferido a " + machine.name + " como " + remoteName;
	return ok;
}

bool read_bridge_ini(const std::string &path, std::string &ip, int &port, std::string &error) {
	std::ifstream in(path.c_str());
	if (!in.is_open()) {
		error = "no se pudo abrir " + path;
		return false;
	}
	// Mismas reglas que GetPrivateProfileString, que es lo que usa UDPcli:
	// secciones entre corchetes y claves sin distinguir mayúsculas
	std::string line, section, ip_found, port_found;
	while (std::getline(in, line)) {
		line = trim(line);
		if (line.empty() || line[0] == ';' || line[0] == '#') continue;
		if (line[0] == '[') {
			size_t end = line.find(']');
			section = upper(trim(line.substr(1, end == std::string::npos ? std::string::npos : end - 1)));
			continue;
		}
		if (section != "SERVER") continue;
		size_t eq = line.find('=');
		if (eq == std::string::npos) continue;
		std::string key = upper(trim(line.substr(0, eq)));
		std::string value = trim(line.substr(eq + 1));
		if (key == "IP") ip_found = value;
		else if (key == "PORT") port_found = value;
	}
	if (ip_found.empty() || port_found.empty()) {
		error = "falta IP= o PORT= en la sección [SERVER] de " + path;
		return false;
	}
	int p = atoi(port_found.c_str());
	if (p <= 0 || p > 65535) {
		error = "puerto inválido en " + path + ": " + port_found;
		return false;
	}
	ip = ip_found;
	port = p;
	error.clear();
	return true;
}

SendResult send_fanuc_udp(const Machine &machine, const std::string &program) {
	std::string ip = machine.ip;
	int port = machine.port;
	std::string source;
	if (!machine.ini.empty()) {
		std::string err;
		if (!read_bridge_ini(machine.ini, ip, port, err)) {
			return fail("No se pudo leer la dirección del puente de " + machine.name + ": " + err);
		}
		source = " (de " + machine.ini + ")";
	}
	sf::IpAddress addr(ip);
	if (addr == sf::IpAddress::None) return fail("Dirección IP inválida para " + machine.name + ": " + ip + source);
	// El destino va en todos los mensajes: UDP no confirma la recepción, y es lo
	// primero que hay que verificar cuando el torno no recibe nada
	std::string dest = ip + ":" + std::to_string(port) + source;

	sf::UdpSocket socket;
	int datagrams = 0;
	auto send = [&](const std::string &data) {
		++datagrams;
		return socket.send(data.data(), data.size(), addr, (unsigned short) port) == sf::Socket::Done;
	};

	// DC2 + "% " + LF: el control comienza a leer
	if (!send(std::string("\x12" "% \n"))) return fail("No se pudo enviar al puente UDP de " + machine.name + " en " + dest);
	sf::sleep(sf::milliseconds(500));

	// Una línea por datagrama, sin líneas vacías, terminada en LF
	std::string line;
	for (size_t i = 0; i <= program.size(); ++i) {
		char c = i < program.size() ? program[i] : '\n';
		if (c == '\r' || c == '\n') {
			if (!line.empty()) {
				if (!send(line + "\n")) return fail("Fallo el envío en el datagrama " + std::to_string(datagrams) + " a " + dest);
				line.clear();
			}
		} else {
			line += c;
		}
	}

	// "%" + DC4: fin de la cinta
	sf::sleep(sf::milliseconds(100));
	if (!send(std::string("%\x14"))) return fail("No se pudo enviar el fin de programa a " + dest);
	sf::sleep(sf::milliseconds(1000));

	SendResult ok;
	ok.ok = true;
	ok.message = "Programa enviado a " + machine.name + " en " + dest + ", " + std::to_string(datagrams) + " datagramas";
	return ok;
}

std::string normalize_remote_name(const std::string &name) {
	std::string n = name;
	// recorta espacios en los extremos
	size_t b = n.find_first_not_of(" \t");
	size_t e = n.find_last_not_of(" \t");
	n = (b == std::string::npos) ? "" : n.substr(b, e - b + 1);
	for (size_t i = 0; i < n.size(); ++i) if (n[i] == ' ') n[i] = '_';
	// extensión .pit (reemplaza cualquier otra)
	size_t dot = n.find_last_of('.');
	std::string ext = (dot == std::string::npos) ? "" : n.substr(dot + 1);
	for (size_t i = 0; i < ext.size(); ++i) ext[i] = (char) tolower((unsigned char) ext[i]);
	if (dot == std::string::npos) n += ".pit";
	else if (ext != "pit") n = n.substr(0, dot) + ".pit";
	return n;
}

std::string strip_program_extension(const std::string &name) {
	size_t dot = name.find_last_of('.');
	if (dot == std::string::npos) return name;
	std::string ext = name.substr(dot + 1);
	for (size_t i = 0; i < ext.size(); ++i) ext[i] = (char) tolower((unsigned char) ext[i]);
	if (ext == "pit" || ext == "nc" || ext == "txt") return name.substr(0, dot);
	return name;
}
