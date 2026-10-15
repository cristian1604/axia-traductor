// Envía un programa al puente UDP del FANUC con el mismo código que usa la
// aplicación, para probar el enlace desde la línea de comandos (sin interfaz):
//   udp_send <ip> <puerto> <programa>
#include "../Sender.h"
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <sstream>

int main(int argc, char **argv) {
	if (argc != 4) {
		fprintf(stderr, "Uso: udp_send <ip> <puerto> <programa>\n");
		return 2;
	}
	std::ifstream in(argv[3], std::ios::binary);
	if (!in.is_open()) {
		fprintf(stderr, "No se pudo abrir %s\n", argv[3]);
		return 2;
	}
	std::stringstream ss;
	ss << in.rdbuf();

	Machine m;
	m.name = argv[1];
	m.protocol = "fanuc-udp";
	m.ip = argv[1];
	m.port = atoi(argv[2]);
	SendResult r = send_fanuc_udp(m, ss.str());
	printf("%s\n", r.message.c_str());
	return r.ok ? 0 : 1;
}
