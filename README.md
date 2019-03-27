# Axia-Traductor

![Generic badge](https://img.shields.io/badge/made%20with-C++-blue.svg) ![Generic badge](https://img.shields.io/badge/status-FUNCTIONAL-yellow.svg) ![Maintenance](https://img.shields.io/badge/Maintained%3F-yes-green.svg)

Text editor and conversor from CNC Fagor 8025 code to 8035 / 8037 versions.

This software was designed for internal and specific translation requirements. Probably don't work on 100% of the cases. In general, you can convert almost any program from Fagor CNC 8025  into Fagor 8035 NC code.
_Axia-Traductor_ performs the following conversions and replacements:

##### Replacements
| Type | Character on 8025 | Character on 8035 |
| ------ | ------ | ------ |
| Comment line | `(`| `;` |
| Tool selection | `T`| `D` |
| Angle | `A`| `Q` |

##### Code Conversion
 - On 8035 syntax, always starts the program with `%` symbol
 - Prevent any blank line inserting a comment character (`;`)
 - Perform a "prologue" and "epilogue" replacement (blocks of code at the beginning or at the end of the CNC program)

### Software
This software was developed on C++ with wxWidgets, using the following IDEs
 - [Zinjai] - An open source C/C++ IDE (integrated development environment)
 - [wxFormBuilder] -  An open source GUI designer application for wxWidgets toolkit
 
### Features
This software is not only a translator. It's a totally functional text editor with syntax highlight (implemented _ad hoc_).

### Current status
The software is totally functional. Only convert portion of static code.
Notice: Assign sentences `=` are not converted.

Explaination:

Since variable enumeration on 8035 and 8025 are both different, the sentence are not converted directly. Is mapped internally and are replaced on execution time.

| Math expression | 8025 syntax | 8035 syntax |
| ------ | ------ | ------ |
| `A = A + B` | `P1 = P1 F1 P2`| `(P101 = P101 + P102)` |
| `A = A - 15` | `P1 = P1 F2 K15`| `(P101 = P101 - K15)` |
| Store the Z pos on a variable | `P1 = Z`| `(P101 = PPOSZ)` |

### Changelog
 - Paste formatting text to NC code (replacing commas `,` by dots `.`)
 - BugFix: Add comment character `;` on blank lines or non-enumerated lines.

#### Disclaimer
This software is provided **_as is_**. [Axia] and I won't provide any warranty of use, or support.
Always make sure that the converted CNC program is correct before execute it. Otherwise it can result on any damage or injuries.
This program on this development status is absolutely experimental. **Please, be careful if you use the converted program on a CNC lathe.**

[Zinjai]: <http://zinjai.sourceforge.net/>
[wxFormBuilder]: <https://github.com/wxFormBuilder/wxFormBuilder>
[Axia]: <https://axia.com.ar/>