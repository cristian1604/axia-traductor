# Graficador de trayectorias y simulador de torneado

Plan de trabajo acordado el 22-09-2026. Rama: `feature/graficador`.

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
| 1 | Intérprete: G00/G01/G02/G03, G90/G91 (U/W en FANUC), X en diámetro, arcos por I/K y por R, programación por ángulo (A en 8025, Q en 8035), redondeos y chaflanes. Tests con programas reales | 1 a 2 sesiones |
| 2 | Panel 2D: trazo discontinuo para rápidos y continuo para avances, ejes y grilla con cotas, coordenadas del puntero en unidades de máquina, zoom con la rueda fijando el punto bajo el cursor, desplazamiento, ajustar a la pieza, resaltado del tramo de la línea actual. Modelo de material y sección sombreada | 1 sesión |
| 3 | Integración: formulario en wxUiEditor, sincronía con el cursor y con las ediciones, menú y tecla, colores en Opciones | 0,5 a 1 sesión |
| 4 | Cobertura de dialecto: ciclos fijos, saltos con repetición, coordenadas paramétricas | abierta |
| 3D | Malla por revolución, iluminación, cámara orbital, avance paso a paso, herramienta con radio de punta, plato, vista en corte | 2 a 3 sesiones |

Una sesión equivale a media jornada de trabajo conjunto.

## Convenciones fijadas

- X se muestra en diámetro, tal como se programa. Z con origen en la cara de
  la pieza según el programa.
- Cada segmento conserva el número de línea del programa que lo generó.
- El bruto se define por diámetro exterior, diámetro interior (0 si es
  macizo) y largo. Se intentará leerlo del encabezado del programa (ver abajo).
- Las coordenadas que no se pueden resolver (parámetros P, aritmética) generan
  un tramo marcado como no resuelto, no un error.

## Decisiones pendientes

- Ubicación: panel dividido junto al editor (preferido) o ventana aparte.
- Qué ciclos fijos usan los programas reales (G66/G68/G69 en Fagor, G70/G71 en
  FANUC) y con qué frecuencia; en la primera versión se dibuja el perfil y se
  marca el ciclo.
- Significado de los campos del encabezado de los programas, por ejemplo en
  `tests/P05A.NC`: `#DN`, `#DA`, `#HN`, `Secc`, `Oring`, `#ODR`, `#IDR` y las
  líneas `T4 70 110` y `P05A 6 84.5 100`. Hipótesis: `#ODR`/`#IDR` son los
  diámetros exterior e interior del tubo en bruto.

## Necesario antes de la fase 1

1. Programas reales de los tres controles en `tests/programas/` (carpeta
   ignorada por git; ver su README). Con ellos se decide qué códigos cubre la
   fase 1 y de ellos salen los tests.
2. Confirmar los campos del encabezado.
3. Acceso a `ABsim.exe` en una PC de producción para contrastar trayectorias
   de programas 8025.

## Referencias en el código actual

- `TranslatorCore.cpp` y `CncSyntax.cpp`: cómo se recorren bloques y palabras;
  el intérprete reutiliza la clasificación para saltear comentarios y cabecera.
- `wxCncEditor`: el panel se sincroniza con su posición de cursor
  (`GetCurrentLine`) y con sus cambios de texto.
- `traductor.wxui`: el panel entra como CustomControl, igual que el editor.
