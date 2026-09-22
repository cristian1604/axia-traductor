# Axia-Traductor

![Generic badge](https://img.shields.io/badge/made%20with-C++-blue.svg) ![Generic badge](https://img.shields.io/badge/status-ACTIVE-green.svg)

Editor y conversor de programas de mecanizado para tornos CNC. Toma código
escrito para un control **Fagor 8025** y lo convierte a **Fagor 8035 / 8037**
o a **FANUC**. Además reenumera líneas, colorea la sintaxis, explora por FTP
los programas guardados en cada torno y envía el programa al torno elegido.

Este software se desarrolló para necesidades internas y específicas. No cubre
el 100 % de los casos, pero convierte la gran mayoría de los programas 8025.

## Compilar

Requiere CMake 3.16, un compilador C++14, wxWidgets 3.2 y SFML 2 (network y system).

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

Los módulos sin interfaz (traductor, reenumerador, envío y coloreado) tienen tests que
se ejecutan con:

```
ctest --test-dir build --output-on-failure
```

`tests/P05A.NC` y `tests/P05A_35.NC` son un programa 8025 real y su
traducción a 8035 verificada en el torno; el test del traductor comprueba que
la salida coincide línea por línea.

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
`CncSyntax.cpp`, sin dependencias de la interfaz), [SFML] para FTP y UDP y
[nlohmann/json] para la configuración. La interfaz se diseña con [wxUiEditor]
(`traductor.wxui`): al generar el código escribe los `*_base.cpp/.h`, que no se
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
