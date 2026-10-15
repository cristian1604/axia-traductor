#ifndef SENDER_H
#define SENDER_H
#include <string>
#include "Machines.h"

/**
	Envío de programas a los tornos. Sin dependencias de wxWidgets.
**/
struct SendResult {
	bool ok = false;
	std::string message;     // descripción del resultado (éxito o error)
};

// Fagor 8035/8037: sube localFile por FTP al directorio de la máquina. El
// nombre remoto es el nombre del archivo local.
SendResult send_ftp(const Machine &machine, const std::string &localFile);

// FANUC a través del puente UDP -> serie (UDPserv). Reproduce el protocolo
// del antiguo UDPcli.exe: DC2 "% \n" de inicio, una línea por datagrama y
// "%" DC4 de fin. Tarda al menos 1,6 segundos por las pausas del protocolo.
// Si la máquina tiene `ini`, la IP y el puerto del puente se leen de ese
// archivo en cada envío (el mismo ab.ini que usan UDPserv y UDPcli); si no se
// puede leer, el envío falla y el mensaje lo dice, en lugar de mandar el
// programa a una dirección vieja.
SendResult send_fanuc_udp(const Machine &machine, const std::string &program);

// Lee la dirección del puente de un .ini con el formato de UDPcli/UDPserv:
//   [SERVER]
//   IP=192.168.100.188
//   PORT=27182
// Devuelve false y describe el problema en `error` si falta el archivo o
// alguna de las dos claves.
bool read_bridge_ini(const std::string &path, std::string &ip, int &port, std::string &error);

// Nombre de archivo normalizado para el torno: sin espacios y con extensión .pit
std::string normalize_remote_name(const std::string &name);

// Nombre para mostrar y editar: sin la extensión de programa (.pit, .nc, .txt,
// en cualquier combinación de mayúsculas), que normalize_remote_name vuelve a
// poner al enviar. Los operarios la borraban a mano cada vez.
std::string strip_program_extension(const std::string &name);

#endif
