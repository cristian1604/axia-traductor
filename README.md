# Axia-Traductor

![Generic badge](https://img.shields.io/badge/made%20with-C++-blue.svg) ![Generic badge](https://img.shields.io/badge/status-FUNCTIONAL-yellow.svg) ![Maintenance](https://img.shields.io/badge/Maintained%3F-yes-green.svg)

Text editor and conversor from CNC Fagor 8025 code to 8035 / 8037 versions.

This software was developed for internal use, but in general, you can convert any program from Fagor CNC 8025  into Fagor 8035 code.
_Axia-Traductor_ performs the following conversions and replacements:

##### Replacements
| Type | Character on 8025 | Character on 8035 |
| ------ | ------ | ------ |
| Comment line | `(`| `;` |
| Tool selection | `T`| `D` |
| Angle | `A`| `Q` |

##### Analysis
 - On 8035 code version, starts the program with `%` symbol
 - Prevent any blank line inserting a comment character (`;`)

### Software
This software was developed on C++ with wxWidgets, using the following IDEs
 - [Zinjai] - An open source C/C++ IDE (integrated development environment)
 - [wxFormBuilder] -  An open source GUI designer application for wxWidgets toolkit
 
##### Features
This software is not only a translator. It's a totally functional text editor with syntax highlight (implemented _ad hoc_).

### Current status
The software is totally functional with some issues. Not on production yet.

**Notice: Assign sentences `=` are not converted yet.**

Explain:

| Math expression | 8025 syntax | 8035 syntax |
| ------ | ------ | ------ |
| `A = A - 15` | `P1 = P1 F2 K15`| `(P101 = P101 - K15)` |
| Store the Z pos on a variable | `P1 = Z`| `(P101 = PPOSZ)` |

### Changelog
 - Applying settings on translation
 - Auto-detect code epilogue

##### Disclaimer
This software is provided **_as is_**. Axia and I won't provide any warranty of use, or support.
Always make sure that the converted CNC program is correct before execute it. Otherwise it can result on any damage or injuries.
This program on this development status is absolutely experimental. **Please, be careful if you use the converted program on a CNC lathe.**

[Zinjai]: <http://zinjai.sourceforge.net/>
[wxFormBuilder]: <https://github.com/wxFormBuilder/wxFormBuilder>