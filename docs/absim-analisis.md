# ABsim: análisis del simulador existente

Fuentes recibidas el 24-09-2026 en `_Sim2/` (carpeta ignorada por git: código y
binario privados). Este documento resume qué hace ABsim y qué reglas heredamos
para el intérprete `CncPath` del graficador. Ver `plan-graficador.md`.

## Qué es

- Borland C++ Builder 3 (1998), VCL, 32 bits, ~1.500 líneas. Corre en esta PC
  porque `vcl35.bpl` está en `C:\Windows\SysWOW64`. Se invoca
  `ABsim.exe programa.NC`; lee `C:\Windows\ABSetup.ini` (`[Tools]` Editor y
  NCxfer) para los botones Edit y Tx.
- Módulos:
  - `GraphicsAux.cpp`: lector de texto (`LeoCode`, `LeoFloat`, `LeoWord`,
    `Busco`) y cola de primitivas de dibujo (`PutDraw`, `LeoDraw`).
  - `NCAux.cpp`: detección de formato, líneas de referencia del encabezado,
    intersecciones recta/recta y recta/arco.
  - `NCFagor.cpp`, `NCSinumerik.cpp`, `NCABCode.cpp`: decodificador por bloque
    (`DecodeLine*`) y generador de primitivas (`XlateLine*`). ABCode está vacío.
  - `Graphics.cpp`: escala pantalla/mundo, trazado de líneas, arcos, redondeos
    y chaflanes, autoescala. El `.asm` es la salida del compilador, no fuente.
  - `Main.cpp`: formulario (PaintBox, memo con el programa, Run, Single Block,
    Reset, Autoscale), zoom y desplazamiento.

## Flujo

1. Lee el archivo completo en un buffer de 32 KB (límite duro) y 2.048
   primitivas de dibujo como máximo.
2. `LineasAuxiliares`: busca `#XX=valor` en todo el archivo y genera líneas de
   referencia (ver encabezado).
3. `SelectCNC`: `%SPF`/`%MPF` → Sinumerik; `%` seguido de número > 0 → Fagor;
   `#` seguido de número → ABCode. Todo lo anterior al `%` se ignora (por eso
   `Cliente:`, `R100=…`, `T4 70 110`, etc. no molestan).
4. Por bloque: `DecodeLine` lee palabras hasta el CR y `XlateLine` encola
   primitivas: posición, línea (con marca de redondeo o chaflán), arco, línea
   de referencia. `PlotBinaryLine` las dibuja.
5. Con Autoscale: decodifica todo, calcula límites (`SetLimits`) y recién
   entonces dibuja. Single Block ejecuta un bloque por clic y selecciona la
   línea en el memo.

## Sistema de coordenadas y presentación

- Interno: horizontal = Z, vertical = X/2 (radio). La barra de estado muestra
  `X` = 2·y (diámetro) y `Z`. Confirma la convención X en diámetro.
- Posición inicial de la herramienta: X400 Z830. El autoescalado ignora los
  puntos con Z ≥ 800 para no incluirla.
- Autoescala: Z máximo fijo en +2 (la cara está en Z0), Z mínimo = menor Z
  alcanzado; radio entre el menor y el mayor alcanzados; factor 0,9 y centrado.
- Pantalla: `u = Uwo + kw·(z − Zwo)`, `v = Vwo − kw·(r − Rwo)`.
  - Zoom: el punto bajo el mouse pasa a ser el origen (Zwo, Rwo)/(Uwo, Vwo) y
    `kw` se multiplica o divide por 1,2 (menú Zoom+ Alt+A, Zoom− Alt+Z).
    Doble clic ×2, Ctrl+doble clic ÷2. Es exactamente el "zoom que conserva el
    punto bajo el cursor" pedido.
  - Desplazamiento: arrastre con botón derecho.
- Estilos: fondo azul marino, todo amarillo; G00 en punteado, resto continuo.
  La posición inicial y el retorno a Z830 se dibujan como punteado largo.
- Líneas de referencia del encabezado en plateado: nominal trazo-punto,
  auxiliar trazo.
- No dibuja herramienta ni material; no hay compensación de radio.

## Encabezado (`#XX=valor`)

| Campo | Efecto | Significado |
| --- | --- | --- |
| `#DN=` | horizontal en r = DN/2, trazo-punto | diámetro nominal |
| `#DA=` | horizontal en r = DA/2, trazo | diámetro auxiliar |
| `#HN=` | vertical en z = HN, trazo-punto | altura (largo) nominal |
| `#HA=` | vertical en z = HA, trazo | altura auxiliar |

`#ODR=` y `#IDR=` tienen tres letras y ABsim los ignora; siguen sin
confirmarse (hipótesis: diámetros exterior e interior del tubo en bruto).
Pueden repetirse (`#DN=170` y `#DN=150` dibujan dos líneas).

## Léxico (común a Fagor y Sinumerik)

- Separadores: espacio y tabulador; fin de bloque: CR. Solo mayúsculas; una
  minúscula o un carácter desconocido produce "Error Linea N".
- Número: dígitos, `.` y `-`; no acepta `+`. Una letra sin número vale 0:
  `Z` = Z0, `G` = G00, `GX234` = G00 X234, `GG40` = G00 G40, `MM5` = M0 M5,
  `F.13` = F0.13. Los programas de muestra usan todas estas formas.
- `(` … `)` comentario, puede abarcar varios bloques. `/` marca de salto
  condicional, se ignora (el bloque se dibuja igual). `P…` descarta el resto del
  bloque (paramétricas ignoradas). `N` número de bloque, solo para mensajes.
- Palabras: `X` `Z` modales; `A` hasta dos por bloque; `R` (Fagor: radio de
  arco o de G36/G39); `I` `K` centro del arco (con G4 activo, `K` es tiempo);
  `G` `M` `T` (T2.2 se lee como float) `S` `F`; `D` solo Sinumerik; `B` solo
  Sinumerik (±radio/chaflán).
- `G`: 0/1/2/3/33 modales; 36/39 por bloque; 40/41/42 se guardan pero no se
  aplican; 4 permanencia. Cualquier otra G descarta el resto del bloque:
  `G91 G00 Z30` entero se ignora, `G53 ZP1` también; en cambio
  `G00 G42 X.. Z..` se dibuja porque G42 es conocida.
- `M`: 3/4/5, 7/8, 0/1/2/30 se guardan; el resto se ignora.

## Geometría (modo Fagor, `NCFagor.cpp`)

- Coordenadas absolutas siempre; G90/G91 no están implementados.
- Solo X o solo Z: recta al nuevo punto conservando la otra coordenada.
- Ángulo `A` medido desde +Z, positivo hacia +X (radio):
  - `A` + `X`: `z = zo + ((x − xo)/2) / tan A`
  - `A` + `Z`: `x = xo + 2·(z − zo)·tan A`
  - Ej.: `A315 Z1` desde la cara baja el diámetro al avanzar en +Z (chaflán).
- `A1 A2` solos (sin X ni Z): queda pendiente. El bloque siguiente da el punto
  final; se traza hasta la intersección de la recta por (xo, zo) con ángulo A1
  y la recta por (x, z) con ángulo A2, y luego al punto final (`IntersecAA`).
- G02/G03 con `R`: exige X y Z (por el bug `case 0x41,0x42,0x43:` solo la
  máscara 0x43 funciona). G02 invierte el signo de R. El centro sale del punto
  medio de la cuerda: siempre arco menor (≤ 180°).
- G02/G03 con `I K`: I en radio (no en diámetro), K en Z, incrementales desde
  el punto inicial. Solo se usa |R| = √(I²+K²) y el centro se recalcula por la
  cuerda; un arco mayor que 180° se dibujaría mal.
- Sentido: G03 antihorario y G02 horario en la vista Z a la derecha, X hacia
  arriba. Verificado con `@@@11K1` (`G03 X160 Z-3.868 I-1.25 K1.25`).
- G33 (rosca) con I/K: se dibuja como recta continua.
- `G36 R` (redondeo): la línea lleva la marca y al dibujar se mira el tramo
  siguiente; se recorta `L = R·tan(θ/2)` a cada lado de la esquina y se traza
  el arco tangente (`Radio`). `G39 R` (chaflán): se recorta R a cada lado y se
  une con recta (`Chaflan`). Si el tramo siguiente no es una recta, se dibuja
  hasta la esquina sin empalme.
- Bloque sin X/Z/A/I/K (solo G, M, F, S, T) no genera trazo.

## Modo Sinumerik (`NCSinumerik.cpp`)

Mismo léxico con tres agregados:

- `B±`: B > 0 radio de empalme, B < 0 chaflán, en la esquina final del bloque;
  hasta dos (B1 para la intersección A1/A2, B2 para el final).
- `A1 A2` con X Z en el mismo bloque: la intersección se resuelve en el mismo
  bloque.
- `A` + `R` con X Z: recta a un ángulo empalmada con arco tangente hasta el
  punto final (`IntersecAR` si A precede a B, `IntersecRA` si B precede a A).
- `G4 K2` produce "Error Linea" en este modo (K no se trata como tiempo).

## Comportamiento sobre los programas de muestra (verificado ejecutándolo)

| Programa | Modo | Resultado |
| --- | --- | --- |
| `_Sim2/polypak.NC` | Fagor | dibuja completo |
| `_Sim2/A2_171.NC` | Fagor | dibuja completo |
| `_Sim2/A2_209.NC` | Sinumerik | "Error Linea550" (`G4 K2`), no dibuja |
| `_Sim2/A2_98.NC` | Sinumerik | error equivalente (`G4 K2`) |
| `_Sim2/@@@11K1`, `@@@11K2` | Sinumerik | dibujan completo, con líneas `#DN` |
| `tests/P05A.NC` (8025 actual) | Fagor | "Error Linea 780" por `.M00 M05`; el resto dibuja |
| `tests/O0001.NC` (8025 actual) | Fagor | dibuja completo |

Capturas en `tests/programas/absim/` junto a cada programa.

## Defectos conocidos (para no heredarlos)

- Límites de 32 KB de programa y 2.048 primitivas.
- Arcos con R y una sola coordenada dan error.
- Autoescala: cadena `else if` mal armada en mínimos/máximos; Z máximo fijo.
- Cada error es un `ShowMessage` modal que bloquea la simulación.
- Compensación de radio ignorada; arcos I/K mayores a 180° incorrectos.
- No soporta G91, parámetros P, saltos (G25 a G29) ni `Q` de 8035 ni FANUC.

## Qué heredamos para `CncPath`

Confirmado por ABsim y por los programas reales:

- X en diámetro en la entrada y en la barra de estado; radio internamente.
- Ángulo desde +Z hacia +X; fórmulas de `A`+`X` y `A`+`Z`; `A1 A2` diferido
  al bloque siguiente.
- G36/G39 como recorte tangente sobre la esquina con el tramo siguiente.
- G02 horario / G03 antihorario en la vista estándar; `I` en radio.
- Letra sin número = 0; `G` sola = G00.
- Encabezado: DN/DA/HN/HA como líneas de referencia.
- Posición inicial X400 Z830 fuera del autoescalado; margen de Z +2.
- Estilos por defecto: punteado para G00, continuo para el resto; zoom sobre el
  punto bajo el cursor; arrastre para desplazar.

Mejoras respecto de ABsim: centro real por I/K (arcos > 180°), R con una sola
coordenada, sin límites de tamaño, errores no modales (marcar la línea y
seguir), G90/G91 reales, `Q` (8035) y U/W, R, `,R`/`,C` (FANUC), asignaciones
de parámetros constantes (fase 4).
