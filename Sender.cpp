#include "Sender.h"
#include <SFML/Network.hpp>
#include <SFML/System.hpp>

static SendResult fail(const std::string &msg) {
	SendResult r;
	r.message = msg;
	return r;
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

SendResult send_fanuc_udp(const Machine &machine, const std::string &program) {
	sf::IpAddress addr(machine.ip);
	if (addr == sf::IpAddress::None) return fail("Dirección IP inválida: " + machine.ip);
	unsigned short port = (unsigned short) machine.port;

	sf::UdpSocket socket;
	int datagrams = 0;
	auto send = [&](const std::string &data) {
		++datagrams;
		return socket.send(data.data(), data.size(), addr, port) == sf::Socket::Done;
	};

	// DC2 + "% " + LF: el control comienza a leer
	if (!send(std::string("\x12" "% \n"))) return fail("No se pudo enviar al puente UDP de " + machine.name);
	sf::sleep(sf::milliseconds(500));

	// Una línea por datagrama, sin líneas vacías, terminada en LF
	std::string line;
	for (size_t i = 0; i <= program.size(); ++i) {
		char c = i < program.size() ? program[i] : '\n';
		if (c == '\r' || c == '\n') {
			if (!line.empty()) {
				if (!send(line + "\n")) return fail("Fallo el envío en el datagrama " + std::to_string(datagrams));
				line.clear();
			}
		} else {
			line += c;
		}
	}

	// "%" + DC4: fin de la cinta
	sf::sleep(sf::milliseconds(100));
	if (!send(std::string("%\x14"))) return fail("No se pudo enviar el fin de programa");
	sf::sleep(sf::milliseconds(1000));

	SendResult ok;
	ok.ok = true;
	ok.message = "Programa enviado a " + machine.name + " (" + std::to_string(datagrams) + " datagramas)";
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
