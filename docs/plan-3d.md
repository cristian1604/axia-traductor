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
| B | Sección con material en el 2D y aviso de rápidos dentro del material | 0,5 sesión |
| C | `wxTurnView3D`: malla, cámara, corte, luz, herramienta | 1 a 1,5 sesiones |
| D | `tools.json`, edición desde Opciones, colores del graficador en Opciones | 0,5 sesión |

## Decisiones tomadas (24-09-2026)

- Herramientas: no hace falta cargarlas. La geometría de la pieza la define la
  trayectoria de la punta; solo las cuchillas de corte y ranurado necesitan
  un ancho, supuesto en 3 mm y ajustable por T. Convención del taller: T1,
  T4 y T5 corte; T2 exterior; T3 y T7 interior; T6 frenteo; el resto se
  deduce. Los correctores D no intervienen: el programa ya está en cotas de
  pieza.
- El encabezado del generador trae el bruto: `T4 70 110` es cuchilla de
  corte y tubo interior/exterior; `P05A 6 84.5 100` es nombre, largo y
  diámetros de la pieza. La cara del bruto es Z0 (la dejó el tronzado
  anterior); lo programado por encima de Z0 es aire.
- Cotas medidas: ΔX en diámetro primero y en radio entre paréntesis.
- Tras el tronzado se muestra solo la pieza (configurable más adelante).
- Proyecto en C++17 por Clipper2.
