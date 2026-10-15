## Axia Traductor 3.2 (20261016)

Simulador de pieza, medición y vista 3D en el graficador, y el logo nuevo de
la empresa. Incluye la corrección de la 3.1 (dirección del puente FANUC leída
de `ab.ini` en cada envío).

### Graficador

- **Mostrar la pieza**: a partir del programa se simula el material que
  arranca cada herramienta y se dibuja la sección de la pieza tal como queda
  hasta la línea donde está el cursor. El tubo en bruto se toma del
  encabezado (`#ODR`, `#IDR`) o se deduce del programa; tras el tronzado
  queda solo la pieza.
- **Medición**: un clic marca un punto (se engancha a los vértices de la
  sección); un segundo clic acota ΔX en diámetro (y el radio entre
  paréntesis), ΔZ y la distancia. Retroceso borra la última cota, Escape
  cancela, y el menú tiene "Borrar las cotas medidas".
- **Vista 3D** (Ctrl+3): la pieza girada, cortada para ver el perfil, con la
  cámara apuntando a la cara mecanizada. Arrastrar orbita, botón derecho
  desplaza, rueda acerca; Inicio recentra, C alterna el corte, T muestra la
  trayectoria.
- Aviso cuando un rápido (G00) atraviesa material.
- Las herramientas se identifican por su función según la convención del
  taller (T1/T4/T5 corte, T2 exterior, T3/T7 y D21–D29 interior, T6 y
  T8–T19 ranurado frontal de punta redonda) y se pueden ajustar en
  `tools.json`, que se crea junto al ejecutable la primera vez.

### Traductor

- A FANUC, el encabezado del generador (cliente, `#DN`, herramienta de
  corte, perfil) se conserva como comentarios después del número de programa.

### Interfaz

- Logo nuevo de la empresa, nítido en pantallas con escalado.

### Instalación

Descomprimir `traductor-windows-3.2.zip` sobre la carpeta de la versión
anterior. `machines.json`, `templates.json` y `tools.json` se conservan o se
crean la primera vez. La vista 3D usa OpenGL 1.1, disponible en cualquier
Windows.

Verificar siempre el programa convertido antes de ejecutarlo en un torno.
