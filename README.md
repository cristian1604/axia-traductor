# Axia-Traductor

![Generic badge](https://img.shields.io/badge/made%20with-C++-blue.svg) ![Generic badge](https://img.shields.io/badge/status-ACTIVE-green.svg)

Editor y conversor de programas de mecanizado para tornos CNC. Toma código
escrito para un control **Fagor 8025** y lo convierte a **Fagor 8035 / 8037**
o a **FANUC**. Además reenumera líneas, colorea la sintaxis, grafica la
trayectoria de la herramienta, explora por FTP los programas guardados en
cada torno y envía el programa al torno elegido.

Este software se desarrolló para necesidades internas y específicas. No cubre
el 100 % de los casos, pero convierte la gran mayoría de los programas 8025.

## Compilar

Requiere CMake 3.16, un compilador C++17, wxWidgets 3.2 y SFML 2 (network y system).
[Clipper2] (booleanas de polígonos del simulador de pieza) viene incluido en
`third_party/`, igual que `json.hpp`.

Ubuntu / Debian:

```
sudo apt install cmake g++ libwxgtk3.2-dev libsfml-dev
cmake -S . -B build
cmake --build build -j
./build/traductor
```

Windows (MSYS2, terminal "MSYS2 MinGW 64-bit"):

```
pacman -S mingw-w64-x86_64-toolchain mingw-w64-x86_64-cmake mingw-w64-x86_64-ninja mingw-w64-x86_64-wxwidgets3.2-msw
cmake -S . -B build -G Ninja
cmake --build build
```

MSYS2 ya distribuye SFML 3, cuya API es distinta; si no encuentra un SFML 2
instalado, CMake descarga y compila SFML 2.6.2 (solo `network` y `system`)
durante la configuración, por lo que hace falta conexión a internet la
primera vez. Para distribuir el ejecutable hay que acompañarlo de las DLL de
wxWidgets y MinGW que usa (`ldd build/traductor.exe` las lista).

## Compilación automática

Cada push ejecuta el flujo de GitHub Actions (`.github/workflows/build.yml`),
que compila y corre los tests en Ubuntu y en Windows (MSYS2) y publica dos
artefactos descargables desde la pestaña *Actions*: `traductor-linux` y
`traductor-windows`, este último ya con las DLL necesarias y la carpeta
`resources/`.

El ejecutable fija su directorio de trabajo en la carpeta donde está instalado,
que debe contener `resources/` (la compilación la copia junto al ejecutable y
el paquete del CI también la incluye), así que se puede lanzar desde cualquier
lugar, por ejemplo desde un acceso directo sin "Iniciar en".

## Tests

Los módulos sin interfaz (traductor, reenumerador, envío, coloreado e
intérprete de trayectorias) tienen tests que se ejecutan con:

```
ctest --test-dir build --output-on-failure
```

`tests/P05A.NC` y `tests/P05A_35.NC` son un programa 8025 real y su
traducción a 8035 verificada en el torno; el test del traductor comprueba que
la salida coincide línea por línea. El test del intérprete comprueba además
que el programa 8025 y sus traducciones a 8035 y a FANUC producen los mismos
tramos de avance. `build/cncpath_dump <programa> [8025|8035|fanuc]` lista los
tramos y avisos de cualquier programa, para depurar, y
`build/plot_preview <programa> [8025|8035|fanuc]` los dibuja con el panel del
graficador (flechas para recorrer el programa línea por línea).

Los programas reales adicionales van en `tests/programas/` (ignorada por git:
suelen llevar datos de clientes).

## Configuración

- **`machines.json`**, junto al ejecutable: lista de tornos con nombre, protocolo
  (`ftp` para Fagor, `fanuc-udp` para el puente UDP a serie del FANUC), IP y
  puerto. Se crea con valores por defecto la primera vez. Es compartido por
  todos los usuarios cuando el programa se ejecuta desde un servidor de archivos.
- **`settings.json`**, por usuario, en `%APPDATA%\axia-traductor` (Windows) o
  `~/.config/axia-traductor` (Linux): colores, opciones de conversión, último
  torno y último nombre de archivo usados.

## Conversiones 8025 → 8035

| Elemento | 8025 | 8035 | Ejemplo |
| ------ | ------ | ------ | ------ |
| Comentario | `(` | `;` | `(Frente)` → `;(Frente)` |
| Herramienta y corrector | `T` | `T` + `D` | `T02.03` → `T02 D03` |
| Ángulo | `A` | `Q` | `A315.000` → `Q315.000` |
| Temporización | segundos | centésimas | `G04 K0.3` → `G04 K30` |
| Posición Z en variable | `P1=Z` | `(P100=PPOSZ)` | |
| Salto | `G29 N0090` | `(GOTO N0100)` | |

Además se reemplazan el prólogo (desde `P2 = K` hasta `G53`) y el epílogo
(desde `P1 = P1 F2 P2` hasta `M30`) por los bloques equivalentes del 8035, y
las líneas vacías se convierten en `;` para que el control no las rechace.

Las asignaciones aritméticas (`P1 = P1 F1 P2`, etc.) **no se convierten**,
porque la numeración de variables difiere entre controles. Revisar siempre el
programa convertido antes de ejecutarlo.

## Reenumeración

Reenumera desde la línea del `%`: todas las líneas posteriores reciben `Nxxxx`
con un paso que depende de la cantidad de líneas (10, 5, 2 o 1), y los saltos
`G25` a `G29` se actualizan al nuevo número de su línea destino.

## Graficador

`Graficador → Mostrar graficador` (F6) abre junto al editor, a la derecha o
debajo según se elija en el mismo menú, el dibujo de la trayectoria del
programa: rápidos punteados, avances continuos, arcos, líneas de referencia
del encabezado (`#DN`, `#DA`, `#HN`, `#HA`) y la sección del tubo en bruto
(`#ODR`, `#IDR`). X se muestra en diámetro. El dibujo sigue al cursor del
editor: el tramo de la línea actual se resalta y lo posterior se atenúa. Las
líneas con errores o avisos del intérprete quedan marcadas en el editor y su
mensaje aparece en la barra de estado al pasar por ellas. Rueda: zoom sobre
el punto bajo el cursor; arrastrar: desplazar; doble clic o Ctrl+E:
encuadrar. Se actualiza solo al editar. Interpreta 8025, 8035/8037 y FANUC
(`CncPath.cpp`); los ciclos fijos y las cotas paramétricas se marcan como no
soportados. La disposición y el estado se recuerdan en `settings.json`.

Con "Mostrar la pieza" dibuja además la sección de la pieza que va quedando
hasta la línea actual (`TurnStock.cpp`, booleanas de polígonos exactas con
Clipper2): el bruto sale del encabezado del generador (`#ODR`/`#IDR` o la
línea `T4 70 110`) o se estima; las herramientas se modelan por su función
según la convención del taller (T1, T4 y T5 corte; T2 exterior; T3, T7 y
correctores 21 a 29 interior; T6 y T8 a T19 angostas de punta redonda para
ranurado frontal, respetando G41/G42), sin cargar catálogos; las ranuras
hechas con cuchilla se dibujan con un ancho supuesto de 3 mm y se marcan. Tras el tronzado queda
solo la pieza. Un rápido que atraviesa material se marca como error en el
editor. Para medir: el puntero se engancha a los vértices del perfil y
muestra sus X y Z; un clic fija un punto y el segundo deja una cota con ΔX
en diámetro y en radio, ΔZ y distancia. Retroceso borra la última cota y
Escape todas.

## Envío a tornos

`Enviar programa a torno` (F2) abre un diálogo con la lista de `machines.json`.
Los Fagor reciben el programa por FTP en `/disk/prg/`. El FANUC lo recibe a
través de una PC puente que corre un servidor UDP conectado al torno por
puerto serie: el programa se envía con el protocolo de lectora de cinta
(`DC2` de inicio, una línea por datagrama, `%` + `DC4` de fin).

## Herramientas externas (no incluidas)

- `ABsim.exe`: simulador gráfico de programas 8025 (menú Simular, F4).
- `Canalesw.exe`: calculadora de canales para sellos (menú Canales, F5).

Son herramientas privadas de la empresa y no se publican en este repositorio.

## Desarrollo

C++14 con [wxWidgets] para la interfaz (el editor es un `wxStyledTextCtrl`, Scintilla,
declarado como *CustomControl* en el diseñador; las reglas de coloreado están en
`CncSyntax.cpp` y el intérprete de trayectorias del graficador en `CncPath.cpp`,
ambos sin dependencias de la interfaz; el graficador se dibuja en `wxPlotPanel.cpp`
con `wxGraphicsContext`), [SFML] para FTP y UDP y
[nlohmann/json] para la configuración. La interfaz se diseña con [wxUiEditor]
(`traductor.wxui`): al generar el código escribe los `wx*Base.cpp/.h`, que no se
editan a mano, y `wxui_code.cmake` con su lista, que CMake incluye. Las imágenes
de la interfaz quedan embebidas en ese código. `MiProyecto.zpr` es el proyecto
para el IDE [ZinjaI].

## Aviso

Este software se provee **tal cual**. [Axia] y el autor no ofrecen garantía
ni soporte. Verificar siempre el programa convertido antes de ejecutarlo en
un torno: un error de conversión puede causar daños o lesiones.

[ZinjaI]: <http://zinjai.sourceforge.net/>
[wxUiEditor]: <https://github.com/KeyWorksRW/wxUiEditor>
[Axia]: <https://axia.com.ar/>
[SFML]: <https://www.sfml-dev.org/>
[wxWidgets]: <https://www.wxwidgets.org/>
[nlohmann/json]: <https://github.com/nlohmann/json>
[Clipper2]: <https://github.com/AngusJohnson/Clipper2>
