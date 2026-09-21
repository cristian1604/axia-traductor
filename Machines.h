#ifndef MACHINES_H
#define MACHINES_H
#include <string>
#include <vector>

/**
	Lista de tornos (controles numéricos) a los que se envían programas.
	Se guarda en machines.json junto al ejecutable, compartida por todos los
	usuarios, para poder cambiar una IP sin recompilar.
**/
struct Machine {
	std::string name;        // nombre visible, ej. "WASINO 8035"
	std::string protocol;    // "ftp" (Fagor 8035/8037) o "fanuc-udp" (puente UDP -> serie)
	std::string ip;
	int port = 21;
	std::string directory;   // directorio remoto (solo FTP), ej. "/disk/prg/"
};

std::vector<Machine> default_machines();

// Carga la lista. Si el archivo no existe, lo crea con los valores por defecto.
// Si está dañado, devuelve los valores por defecto y describe el problema en `error`.
std::vector<Machine> load_machines(const std::string &path, std::string *error = nullptr);
bool save_machines(const std::string &path, const std::vector<Machine> &machines);

// Busca por nombre; devuelve nullptr si no existe.
const Machine *find_machine(const std::vector<Machine> &machines, const std::string &name);

#endif
