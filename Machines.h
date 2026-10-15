#ifndef MACHINES_H
#define MACHINES_H
#include <string>
#include <vector>

/**
	Lista de tornos (controles numéricos) a los que se envían programas.
	Se guarda en machines.json junto al ejecutable, compartida por todos los
	usuarios, para poder cambiar una IP sin recompilar. Si ese archivo contiene
	solo {"path": "\\\\servidor\\carpeta\\machines.json"}, la lista se lee de
	esa ruta (una carpeta de red): una IP se cambia en un único lugar aunque el
	programa esté instalado en cada PC.
**/
struct Machine {
	std::string name;        // nombre visible, ej. "WASINO 8035"
	std::string protocol;    // "ftp" (Fagor 8035/8037) o "fanuc-udp" (puente UDP -> serie)
	std::string ip;
	int port = 21;
	std::string directory;   // directorio remoto (solo FTP), ej. "/disk/prg/"
	// Solo fanuc-udp: .ini del que se leen la IP y el puerto del puente al
	// enviar ([SERVER] IP= y PORT=), el mismo ab.ini que usan UDPserv y UDPcli,
	// para que haya una sola dirección que mantener. Si es relativo se toma
	// desde la carpeta del machines.json. Con ini, `ip` y `port` se ignoran.
	std::string ini;
};

std::vector<Machine> default_machines();

// Carga la lista. Si el archivo no existe, lo crea con los valores por defecto.
// Si está dañado, o la ruta a la que apunta no se puede leer, devuelve los
// valores por defecto y describe el problema (con la ruta) en `error`.
std::vector<Machine> load_machines(const std::string &path, std::string *error = nullptr);
bool save_machines(const std::string &path, const std::vector<Machine> &machines);

// Busca por nombre; devuelve nullptr si no existe.
const Machine *find_machine(const std::vector<Machine> &machines, const std::string &name);

#endif
