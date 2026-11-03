## Axia Traductor 3.3 (20261103)

Corrección de la cabecera en la traducción a Fagor 8035 / 8037.

### Traductor

- La línea de cabecera termina en `%NOMBRE,MX--,`: desde la 3.0 se le pegaba
  un `;` cuando el programa no traía el encabezado del generador
  (`%00001,MX--,;`), y con encabezado la última línea de comentarios quedaba
  `;;`.
- Si la línea del `%` original trae un comentario, ya no entra en el nombre
  del programa: el nombre toma solo el número y el comentario pasa a una
  línea propia con `;`.

Los cambios de la 3.2 (simulador de pieza, medición, vista 3D, logo) y de la
3.1 (dirección del puente FANUC leída de `ab.ini` en cada envío) están en
las notas de esos releases.

### Instalación

Descomprimir `traductor-windows-3.3.zip` sobre la carpeta de la versión
anterior. `machines.json`, `templates.json` y `tools.json` se conservan.

Verificar siempre el programa convertido antes de ejecutarlo en un torno.
