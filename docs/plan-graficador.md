# Graficador de trayectorias y simulador de torneado

Plan de trabajo acordado el 22-09-2026. Rama: `feature/graficador`.
Actualizado el 24-09-2026 con el análisis del simulador actual
(`absim-analisis.md`): de ahí salen las reglas exactas del dialecto y las
capturas de referencia.

## Objetivo

Ver, dentro del traductor, el recorrido de la herramienta de un programa CNC
(8025, 8035/8037 o FANUC) a medida que se avanza por el código, y más adelante
el material de la pieza consumiéndose, para detectar errores antes de
mecanizar. Reemplaza a `ABsim.exe` (privado, externo y solo 8025) y permite
revisar el programa **traducido** antes de enviarlo al torno.

## Arquitectura

La pieza torneada es un sólido de revolución: su media sección en el plano
X/Z (radio exterior e interior en función de Z) la describe por completo. Por
eso el modelo es 2D y sirve tanto al dibujo plano como al render 3D futuro.

| Pieza | Depende de wx | Tests | Fase |
| --- | --- | --- | --- |
| `CncPath`: intérprete de bloques a segmentos (rápido, avance, arco) con la línea de origen de cada uno | no | sí | 1 |
| `StockProfile`: sección r(z) exterior e interior recortada tramo a tramo | no | sí | 2 |
| `wxPlotPanel`: dibujo con `wxGraphicsContext` (Direct2D en Windows, Cairo en Linux) | sí | no | 2 |
| Integración en la ventana principal y sincronía con el editor | sí | no | 3 |
| `wxTurnView3D`: `wxGLCanvas` + OpenGL, malla por revolución del perfil | sí | no | posterior |

Decisión tomada: el 2D se hace con `wxGraphicsContext`, no con OpenGL. Lo que
comparten ambos modos es el intérprete y el modelo de material, no el dibujo;
OpenGL queda para el 3D, opcional, porque las PC de planta pueden no tener
drivers adecuados.

## Fases y esfuerzo

| Fase | Contenido | Esfuerzo estimado |
| --- | --- | --- |
| 1 | **Hecha (24-09-2026).** Intérprete `CncPath.cpp`: G00/G01/G02/G03/G33, G90/G91 (U/W en FANUC), X en diámetro, arcos por I/K y por R, programación por ángulo (A en 8025 y FANUC, Q en 8035, A1 A2 diferido), G36/G39 y `,R`/`,C`, encabezado, avisos no bloqueantes. Tests: `tests/test_path.cpp`; herramienta `cncpath_dump` | 1 a 2 sesiones |
| 2 | **Hecha (24-09-2026).** `wxPlotPanel.cpp`: rápidos punteados y avances continuos, grilla con rótulos (X en diámetro, Z), coordenadas del puntero, zoom con la rueda sobre el punto bajo el cursor, desplazamiento arrastrando, encuadre (doble clic o Inicio), línea actual resaltada con lo posterior atenuado y marcador de la herramienta, líneas de referencia del encabezado y tubo en bruto (#ODR/#IDR) como sección. Vista previa: `tests/plot_preview.cpp`. El material que se va quitando queda para la etapa 3D: necesita la forma de cada herramienta (ancho de la de corte, radio de punta), sin la cual un punto no arranca nada | 1 sesión |
| 3 | **Hecha (24-09-2026).** El panel entra en `traductor.wxui` como CustomControl dentro de un divisor (`m_plotSplitter`) junto al editor, a la derecha o debajo a elección; menú Graficador armado en código (F6 mostrar, disposición, rápidos, Ctrl+E encuadrar); reinterpretación con retardo de 300 ms al editar y al cambiar de sintaxis; línea actual desde el cursor; errores y avisos como fondo de línea en el editor y en la barra de estado; estado y divisores en `settings.json`. Pendiente menor: colores de `PlotColours` en Opciones | 0,5 a 1 sesión |
| 4 | Cobertura de dialecto: ciclos fijos, saltos con repetición, coordenadas paramétricas | abierta |
| 3D | Malla por revolución, iluminación, cámara orbital, avance paso a paso, herramienta con radio de punta, plato, vista en corte | 2 a 3 sesiones |

Una sesión equivale a media jornada de trabajo conjunto.

## Convenciones fijadas

- X se muestra en diámetro, tal como se programa; internamente se trabaja en
  radio. Z con origen en la cara de la pieza según el programa. Vista: Z hacia
  la derecha, X hacia arriba; G03 antihorario, G02 horario en esa vista.
- Ángulo `A` (8025) o `Q` (8035) medido desde +Z hacia +X. `A1 A2` en un
  bloque sin X ni Z queda pendiente hasta el bloque siguiente.
- `I` en radio y `K` en Z, incrementales desde el inicio del arco.
- Una letra sin número vale 0 (`Z` = Z0, `G` = G00, `GG40` = G00 G40).
- Posición inicial de la herramienta X400 Z830, excluida del encuadre.
- Encabezado `#DN=`, `#DA=`, `#HN=`, `#HA=`: líneas de referencia (diámetro y
  largo nominal en trazo-punto, auxiliares en trazo).
- Cada segmento conserva el número de línea del programa que lo generó.
- El bruto se define por diámetro exterior, diámetro interior (0 si es
  macizo) y largo. Se intentará leerlo del encabezado del programa (ver abajo).
- Las coordenadas que no se pueden resolver (parámetros P, aritmética) generan
  un tramo marcado como no resuelto, no un error. Los errores nunca bloquean:
  se marca la línea y se sigue.

## Decisiones pendientes

- Ubicación: panel dividido junto al editor (preferido) o ventana aparte.
- Qué ciclos fijos usan los programas reales (G66/G68/G69 en Fagor, G70/G71 en
  FANUC) y con qué frecuencia; en la primera versión se dibuja el perfil y se
  marca el ciclo.
- Campos del encabezado que ABsim no usa: `#ODR`, `#IDR` (hipótesis:
  diámetros exterior e interior del tubo en bruto), `Secc`, `Oring` y las
  líneas `T4 70 110` y `P05A 6 84.5 100`.
- Si los programas en modo Sinumerik (`%MPF`, `B±`, `R100=`) siguen en uso o
  son históricos. Si son históricos, el intérprete no los cubre.
- Si `G` sin número equivale a G00 en el 8025 real o solo lo toleraba el
  simulador (los programas `A2_*` y `polypak` lo usan).

## Corpus de prueba

- `_Sim2/`: fuentes y binario de ABsim más seis programas (ignorado por git).
  `ABsim.exe programa.NC` corre en esta PC.
- `tests/programas/absim/`: esos programas más `tests/P05A.NC` y
  `tests/O0001.NC`, cada uno con la captura de ABsim al lado (ignorado por
  git salvo el README).
- `tests/P05A.NC`, `tests/P05A_35.NC`, `tests/O0001.NC`, `tests/O0001_FANUC.NC`:
  versionados, sirven para los tests unitarios del intérprete.

## Necesario antes de la fase 1

1. Más programas reales, sobre todo 8035 y FANUC, en `tests/programas/`
   (ver su README). Con ellos se decide qué códigos cubre la fase 1.
2. Respuestas a las decisiones pendientes de arriba.

## Referencias en el código actual

- `TranslatorCore.cpp` y `CncSyntax.cpp`: cómo se recorren bloques y palabras;
  el intérprete reutiliza la clasificación para saltear comentarios y cabecera.
- `wxCncEditor`: el panel se sincroniza con su posición de cursor
  (`GetCurrentLine`) y con sus cambios de texto.
- `traductor.wxui`: el panel entra como CustomControl, igual que el editor.
