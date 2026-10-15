## Axia Traductor 3.1 (20261015)

Corrección del envío al KIA FANUC. Los cambios de la 3.0 (graficador,
encabezado 8035, portapapeles, `G04 P`) están en las notas de ese release.

### Envío al torno

- El puente UDP → serie del FANUC toma su IP por DHCP y cambió de dirección.
  La 3.0 tenía la IP copiada en `machines.json`, así que los envíos iban a
  una dirección sin equipo aunque el diálogo dijera "enviado". Ahora la
  dirección se lee **en cada envío (F2)** de `..\..\Inis\ab.ini` (sección
  `[SERVER]`, claves `IP=` y `PORT=`), el mismo archivo que usan `UDPserv`
  y `UDPcli`, y los campos `ip` y `port` de esa entrada se ignoran.
- Al elegir el torno, el diálogo muestra la dirección a la que va a enviar;
  si no puede leer `ab.ini`, lo dice y no envía.
- Todos los mensajes del envío incluyen el destino.
- `machines.json` puede contener solo
  `{"path": "\\\\servidor\\carpeta\\machines.json"}` para leer la lista desde
  una carpeta de red.
- El protocolo UDP se verificó byte a byte y en tiempos contra el antiguo
  `UDPcli.exe`: es idéntico. Para diagnosticar el enlace, el paquete incluye
  `udp_send.exe <ip> <puerto> <programa>` (envía con el mismo código que la
  aplicación) y el receptor `udp-capture.ps1`, que registra cada datagrama
  con su tiempo.

### Instalación

Descomprimir `traductor-windows-3.1.zip` sobre la carpeta de la versión
anterior. Si ya existe `machines.json`, agregar a la entrada del FANUC la
línea `"ini": "..\\..\\Inis\\ab.ini"` (en la instalación de planta ya está
hecho); si no existe, se crea con ese valor.

Verificar siempre el programa convertido antes de ejecutarlo en un torno.
