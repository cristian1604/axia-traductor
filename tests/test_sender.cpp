// Tests del envío por UDP al FANUC (con un receptor local) y de nombres remotos.
#include "../Sender.h"
#include <SFML/Network.hpp>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <string>
#include <vector>

static int failures = 0;
#define CHECK(cond) do { if (!(cond)) { ++failures; fprintf(stderr, "FALLO %s:%d: %s\n", __FILE__, __LINE__, #cond); } } while (0)

int main() {
	// Receptor UDP local que hace de UDPserv
	sf::UdpSocket rx;
	CHECK(rx.bind(sf::Socket::AnyPort, sf::IpAddress::LocalHost) == sf::Socket::Done);

	Machine fanuc;
	fanuc.name = "FANUC de prueba";
	fanuc.protocol = "fanuc-udp";
	fanuc.ip = "127.0.0.1";
	fanuc.port = rx.getLocalPort();

	SendResult r = send_fanuc_udp(fanuc, "O0001\r\nN10 G00 X1\r\n\r\nN20 M30\n");
	CHECK(r.ok);

	std::vector<std::string> got;
	rx.setBlocking(false);
	char buf[4096];
	std::size_t n;
	sf::IpAddress from;
	unsigned short fromPort;
	while (rx.receive(buf, sizeof(buf), n, from, fromPort) == sf::Socket::Done) {
		got.push_back(std::string(buf, n));
	}
	CHECK(got.size() == 5);
	if (got.size() == 5) {
		CHECK(got[0] == std::string("\x12" "% \n"));
		CHECK(got[1] == "O0001\n");
		CHECK(got[2] == "N10 G00 X1\n");
		CHECK(got[3] == "N20 M30\n");
		CHECK(got[4] == std::string("%\x14"));
	}

	// IP inválida
	fanuc.ip = "no-es-una-ip";
	CHECK(!send_fanuc_udp(fanuc, "x").ok);

	// Nombres remotos
	CHECK(normalize_remote_name("programa") == "programa.pit");
	CHECK(normalize_remote_name(" mi programa.NC ") == "mi_programa.pit");
	CHECK(normalize_remote_name("P05A.PIT") == "P05A.PIT");
	CHECK(normalize_remote_name("P05A.pit") == "P05A.pit");

	// Nombre propuesto en el diálogo: sin la extensión que se agrega al enviar
	CHECK(strip_program_extension("000030.pit") == "000030");
	CHECK(strip_program_extension("P05A.PIT") == "P05A");
	CHECK(strip_program_extension("P05A.NC") == "P05A");
	CHECK(strip_program_extension("P05A.txt") == "P05A");
	CHECK(strip_program_extension("P05A") == "P05A");
	CHECK(strip_program_extension("P05A.RIG") == "P05A.RIG");   // no es extensión de programa
	CHECK(strip_program_extension("P05A.RIG.pit") == "P05A.RIG");
	CHECK(normalize_remote_name(strip_program_extension("000030.pit")) == "000030.pit");

	// Lista de tornos: machines.json puede contener solo {"path": "..."} y la
	// lista se lee de esa otra ruta (carpeta de red)
	{
		const char *tmp = getenv("TEMP");
		std::string dir = tmp ? tmp : "/tmp";
		for (size_t i = 0; i < dir.size(); ++i) if (dir[i] == '\\') dir[i] = '/';   // válido en Windows y sin escapes en JSON
		std::string local = dir + "/test_machines_local.json";
		std::string shared = dir + "/test_machines_red.json";
		{
			std::ofstream s(shared.c_str());
			s << "{\"machines\":[{\"name\":\"KIA\",\"protocol\":\"fanuc-udp\",\"ip\":\"192.168.100.188\",\"port\":27182}]}";
		}
		{ std::ofstream l(local.c_str()); l << "{\"path\":\"" << shared << "\"}"; }
		std::string err;
		std::vector<Machine> m = load_machines(local, &err);
		CHECK(err.empty());
		CHECK(m.size() == 1 && m[0].name == "KIA" && m[0].ip == "192.168.100.188" && m[0].port == 27182);

		// la ruta apuntada no existe: valores por defecto y aviso que la nombra
		{ std::ofstream l(local.c_str()); l << "{\"path\":\"" << dir << "/no_existe.json\"}"; }
		m = load_machines(local, &err);
		CHECK(!err.empty() && err.find("no_existe.json") != std::string::npos);
		CHECK(m.size() == default_machines().size());

		// un archivo que apunta a otro que vuelve a apuntar: se sigue un solo salto
		{ std::ofstream s(shared.c_str()); s << "{\"path\":\"" << local << "\"}"; }
		{ std::ofstream l(local.c_str()); l << "{\"path\":\"" << shared << "\"}"; }
		m = load_machines(local, &err);
		CHECK(!err.empty());

		// "ini" relativo: se resuelve desde la carpeta del machines.json
		{ std::ofstream l(local.c_str()); l << "{\"machines\":[{\"name\":\"K\",\"protocol\":\"fanuc-udp\",\"ini\":\"test_ab.ini\"}]}"; }
		m = load_machines(local, &err);
		CHECK(err.empty());
		CHECK(m.size() == 1 && m[0].ini == dir + "/test_ab.ini" && m[0].ip.empty());
		remove(local.c_str());
		remove(shared.c_str());
	}

	// Dirección del puente leída de ab.ini (formato de UDPcli/UDPserv)
	{
		const char *tmp = getenv("TEMP");
		std::string ini = std::string(tmp ? tmp : "/tmp") + "/test_ab.ini";
		{
			std::ofstream f(ini.c_str());
			f << "[SERVER]\r\nIP=127.0.0.1\r\nPORT=" << rx.getLocalPort() << "\r\n\r\n[SERIAL]\r\nCOM=5\r\nConfig=9600,n,8,1\r\n";
		}
		std::string ip, err;
		int port = 0;
		CHECK(read_bridge_ini(ini, ip, port, err));
		CHECK(ip == "127.0.0.1" && port == rx.getLocalPort() && err.empty());
		CHECK(!read_bridge_ini(ini + ".no", ip, port, err) && err.find("test_ab.ini.no") != std::string::npos);

		// El envío toma IP y puerto del ini, no de la entrada, y lo dice
		Machine viaIni;
		viaIni.name = "KIA";
		viaIni.protocol = "fanuc-udp";
		viaIni.ip = "192.0.2.1";    // dirección vieja: no debe usarse
		viaIni.port = 1;
		viaIni.ini = ini;
		r = send_fanuc_udp(viaIni, "O0002\nM30\n");
		CHECK(r.ok);
		CHECK(r.message.find("127.0.0.1:" + std::to_string(rx.getLocalPort())) != std::string::npos);
		CHECK(r.message.find(ini) != std::string::npos);
		got.clear();
		while (rx.receive(buf, sizeof(buf), n, from, fromPort) == sf::Socket::Done) got.push_back(std::string(buf, n));
		CHECK(got.size() == 4);

		// Sin poder leer el ini no se envía nada
		viaIni.ini = ini + ".no";
		r = send_fanuc_udp(viaIni, "O0002\nM30\n");
		CHECK(!r.ok && r.message.find(ini + ".no") != std::string::npos);
		remove(ini.c_str());
	}

	if (failures == 0) printf("OK: todos los tests de envío pasaron\n");
	return failures == 0 ? 0 : 1;
}
