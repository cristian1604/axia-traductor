## Axia Traductor 3.0 (20261002)

Versión mayor respecto de la 2.1. El programa se compila ahora con CMake en
Windows y en Linux, con tests automáticos en cada cambio.

### Traductor

- La traducción a Fagor 8035 / 8037 escribe el inicio de programa como
  `%NOMBRE,MX--,`, con el nombre del archivo cargado (o la descripción del
  encabezado, o el número original).
- La pausa `G04 K` del 8025, en segundos, pasa a FANUC como `G04 P` en
  milisegundos (`G04 K.2` → `G04 P200`).
- Redondeo de esquina `G36 R` a FANUC como `,R` al final del bloque.
- Opción para dejar el programa traducido y reenumerado en el portapapeles,
  como lo hacía el reenumerador anterior (Parámetros, apagada por defecto).
- Reenumerador propio, integrado: ya no depende de un programa externo.

### Editor

- Editor nuevo con coloreado de sintaxis instantáneo, números de línea,
  deshacer y rehacer, y zoom con Ctrl + rueda.
- Aviso de cambios sin guardar al cerrar, abrir o reemplazar el programa.
- Búsqueda, y búsqueda con reemplazo, corregidas.
- Interfaz nítida en pantallas con escalado (125 %, 150 %).

### Graficador de trayectorias

- Menú Graficador → Mostrar graficador (F6): dibuja el recorrido de la
  herramienta junto al editor, a la derecha o debajo. Rápidos en punteado,
  avances continuos, líneas de referencia del encabezado y tubo en bruto.
- Sigue al cursor del editor: resalta el tramo de la línea actual. Muestra
  las coordenadas del puntero con X en diámetro. Zoom con la rueda sobre el
  punto bajo el cursor.
- Interpreta programas 8025, 8035 / 8037 y FANUC, y marca en el editor las
  líneas con errores.

### Tornos

- Envío al torno desde un diálogo propio (F2), por FTP a los Fagor y por el
  puente UDP al FANUC, sin programas externos. El nombre propuesto ya no
  trae la extensión `.pit`.
- Explorador FTP con la lista de tornos de `machines.json`, editable sin
  recompilar.

### Instalación

Descomprimir `traductor-windows-3.0.zip` y ejecutar `traductor.exe`: la
carpeta incluye todo lo necesario. `machines.json` y `templates.json` se
crean la primera vez. Las herramientas externas `ABsim.exe` y `Canalesw.exe`
no se distribuyen y, si se usan, se copian a la misma carpeta.

Verificar siempre el programa convertido antes de ejecutarlo en un torno.
