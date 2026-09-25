# Simulador de torneado: material y vista 3D

Plan de la etapa 3D del graficador (ver `plan-graficador.md`, fases 1 a 3
hechas). Rama: `feature/graficador-3d`, creada desde `master` el 24-09-2026.

## Objetivo

Ver el tubo en bruto convertirse en la pieza a medida que avanza el programa,
en 3D y también como sección sombreada en el graficador 2D, para detectar
pasadas en vacío, rápidos dentro del material y errores de cota antes de
mecanizar. Reemplaza la "sección fija" que hoy dibuja el 2D.

## Modelo de material (`TurnStock`, sin wxWidgets, con tests)

- La pieza es un sólido de revolución: alcanza con su media sección en el
  plano Z/R. Se representa por **columnas en Z** de paso fijo (0,05 mm por
  defecto); cada columna guarda la lista de intervalos de radio `[ra, rb]`
  con material. Varios intervalos por columna permiten ranuras frontales.
- Se inicializa con `#ODR`/`#IDR` y un largo estimado de los avances (el
  encabezado no trae el largo del tubo). Sin encabezado, con los límites de
  los avances más un margen.
- La **herramienta** es un polígono de corte en Z/R con el punto programado
  en el origen. Un tramo de avance barre el polígono a lo largo de la recta
  (casco convexo de las dos posiciones, el polígono es convexo) o del arco
  (muestreado cada pocos grados); de cada columna que toca se quita el
  intervalo de radios cubierto.
- Los rápidos no quitan material, pero si atraviesan material se genera el
  aviso "rápido dentro del material", que es de los más útiles.
- Estado por línea: recalcular desde cero hasta la línea N cuesta columnas ×
  tramos (del orden de 10⁵ operaciones), así que el paso a paso no necesita
  caché.
- Cada límite de intervalo recuerda si lo dejó una herramienta, para pintar
  distinto el bruto y lo mecanizado.

## Herramientas (`tools.json`, compartido como `machines.json`)

- Por número T: tipo (exterior, interior, tronzado o ranurado, roscado),
  ancho (tronzado y ranurado), radio de punta y lado del cuerpo (+X
  exterior, −X interior).
- Sin definición se usa un inserto genérico con radio de punta 0,4 y cuerpo
  hacia el lado desde donde se acerca (se deduce del primer rápido de cada
  T). Eso ya da una simulación razonable sin configurar nada.
- **Necesario del usuario:** la lista real de herramientas de cada torno (T,
  tipo, ancho, radio de punta).

## Vista 2D con material

El graficador pinta la sección según `TurnStock` (relleno por columnas) en
lugar del rectángulo fijo, y la actualiza con la línea actual. Es la
ganancia más inmediata y vale por sí sola.

## Vista 3D (`wxTurnView3D`, wxGLCanvas)

- OpenGL 1.1 con arrays de vértices e iluminación fija: corre en cualquier
  PC de planta, incluso con render por software. `wxUSE_GLCANVAS` está
  disponible en el wxWidgets de MSYS2 y en el de Ubuntu.
- Malla por revolución: por columna, bandas cilíndricas exterior e interior,
  y coronas donde cambian los radios entre columnas vecinas. 600 columnas ×
  96 pasos angulares son unos 200 k triángulos: sobra.
- Cámara orbital (arrastre), zoom (rueda), desplazamiento (botón derecho) y
  vista en corte con plano de recorte (media pieza) para ver el interior.
- Herramienta dibujada en su posición actual (polígono extruido) y, opcional,
  la trayectoria en el plano de corte.
- Va en el mismo lugar que el graficador, alternando 2D/3D desde el menú;
  sigue la misma línea actual. Si no se puede crear el contexto GL, queda el
  2D y el menú lo informa.

## Fases y esfuerzo

| Fase | Contenido | Esfuerzo |
| --- | --- | --- |
| A | **Hecha (24-09-2026).** `TurnStock.cpp` con Clipper2 (C++17): sección exacta por booleanas de polígonos; herramienta por función (cuña abierta hacia +Z con filo principal radial y secundario a 52° para exterior e interior, cuadrante para frenteo, franja de ancho supuesto hacia el plato para corte y ranurado); tronzado detectado y resto del tubo descartado; aviso de rápido dentro del material; simulación hasta una línea dada. Tests en `tests/test_stock.cpp`: con P05A la sección final da exactamente las cotas de terminación. `CncPath` lee la herramienta por tramo (T o corrector) y el encabezado del generador en sus tres formas; la traducción a FANUC conserva el encabezado como comentarios | 1 sesión |
| B | **Hecha (24-09-2026).** `wxPlotPanel` simula la pieza (completa para los avisos, hasta la línea actual para dibujar) y pinta la sección sobre el bruto tenue; ranuras de ancho supuesto marcadas; menú "Mostrar la pieza" (`plot_stock` en settings.json) y "Borrar las cotas medidas"; rápidos dentro del material como error en el editor. Medición sin modo: enganche a los vértices del perfil (o de los avances si la pieza está oculta), clic fija un punto, segundo clic deja la cota ΔX (diám. y radio), ΔZ y distancia; Retroceso y Escape borran | 0,5 sesión |
| C | **Hecha (25-09-2026).** `wxTurnView3D.cpp` (wxGLCanvas, OpenGL 1.1 de función fija): cada lado de la sección se gira en una banda de triángulos con normales hacia afuera; en corte se conserva la mitad de atrás y las caras de la sección se triangulan con Clipper2 (`Triangulate`); recorrido y herramienta en el plano de corte; cámara orbital (arrastre), desplazamiento (botón derecho o central), zoom (rueda), Inicio encuadra; C corte, T trayectoria. Es hijo de `wxPlotPanel` (`SetView3D`), que le pasa la escena de la línea actual; menú "Vista 3D" (Ctrl+3, `plot_3d` en settings.json); el contexto se crea en el primer dibujo y sin OpenGL queda el 2D | 1 a 1,5 sesiones |
| D | **Parcial (25-09-2026).** `tools.json` junto al ejecutable (`ToolsFile.cpp`): se crea con la convención del taller y se edita a mano; roles, ancho de cuchilla y radio de punta por número. Pendiente: edición desde Opciones y colores del graficador en Opciones, si hacen falta | 0,5 sesión |

## Pendientes fuera del graficador

- **Pausa en la traducción a FANUC** (anotado por el usuario el 25-09-2026):
  el 8025 programa `G04 K.2` (segundos) y la traducción la dejaba como
  `K20` (centésimas, formato 8035). Corregido el mismo día: ahora sale
  `G04 P200` (milisegundos), con test. Falta probar en el KIA junto con los
  comentarios de encabezado.

## Decisiones tomadas (24-09-2026)

- Herramientas: no hace falta cargarlas. La geometría de la pieza la define la
  trayectoria de la punta; solo las cuchillas de corte y ranurado necesitan
  un ancho, supuesto en 3 mm y ajustable por T. Los correctores D no
  intervienen como offsets: el programa ya está en cotas de pieza; sí sirven
  para identificar la herramienta cuando T es 0.
- Convención del taller aplicada (24-09-2026, tras los programas VARISEAL y
  V01F): T1, T4 y T5 corte radial; T2 exterior; T3, T7 y los correctores 21
  a 29 interiores; **T6 y T8 a T19 angostas de punta redonda** (ranurado
  frontal y perfilado de labios: en los programas reales T6 hace la ranura
  en U de los sellos, no el frenteo, que se hace con T2). Una angosta quita
  un disco del radio de punta más su vástago hacia +Z; con G41/G42 el disco
  va tangente al contorno del lado que indica la compensación, y sin
  compensación el punto programado es el fondo del disco en Z y su centro en
  X. El radio de punta se toma del menor arco que hace esa herramienta (el
  fondo redondeado de la ranura lo deja la propia punta) o 0,4 por defecto.
  Con cuña, esas herramientas se llevaban medio labio.
- El encabezado del generador trae el bruto: `T4 70 110` es cuchilla de
  corte y tubo interior/exterior; `P05A 6 84.5 100` es nombre, largo y
  diámetros de la pieza. La cara del bruto es Z0 (la dejó el tronzado
  anterior); lo programado por encima de Z0 es aire.
- Cotas medidas: ΔX en diámetro primero y en radio entre paréntesis.
- Tras el tronzado se muestra solo la pieza (configurable más adelante).
- Proyecto en C++17 por Clipper2.
