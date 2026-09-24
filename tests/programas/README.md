# Programas reales para el graficador

Dejar acá programas tal como salen de los tornos o del taller, para definir qué
códigos cubre el intérprete de trayectorias y para armar sus tests.

Esta carpeta está **ignorada por git** salvo este archivo: el repositorio es
público y los programas suelen llevar datos de clientes en el encabezado. Los
que se conviertan en tests se copian a `tests/` con el encabezado limpio.

Estructura sugerida:

```
tests/programas/
  absim/     programas de muestra del simulador ABsim con su captura .png
  8025/      programas para Fagor 8025 (.NC, .PIT)
  8035/      programas guardados por los Fagor 8035 / 8037 (.pit)
  fanuc/     programas para el KIA FANUC
```

Conviene que haya de cada tipo:

- programas simples (solo G00/G01/G02/G03),
- alguno con programación por ángulo (`A` en 8025, `Q` en 8035),
- alguno con redondeo o chaflán (G36, G39, `,R` en FANUC),
- alguno con ciclos fijos, si se usan (G66/G68/G69 en Fagor, G70/G71 en FANUC),
- alguno con saltos o repeticiones (G25 a G29, `GOTO`),
- alguno con coordenadas paramétricas (`X P1`, aritmética con F-códigos).

Si para algún programa 8025 hay una captura de `ABsim.exe`, guardarla al lado
con el mismo nombre y extensión `.png`: sirve para contrastar la trayectoria.
