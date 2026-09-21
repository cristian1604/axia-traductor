// Tests del envío por UDP al FANUC (con un receptor local) y de nombres remotos.
#include "../Sender.h"
#include <SFML/Network.hpp>
#include <cstdio>
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

	if (failures == 0) printf("OK: todos los tests de envío pasaron\n");
	return failures == 0 ? 0 : 1;
}
