# Axia-Traductor

![Generic badge](https://img.shields.io/badge/made%20with-C++-blue.svg) ![Generic badge](https://img.shields.io/badge/status-FUNCTIONAL-yellow.svg) ![Maintenance](https://img.shields.io/badge/Maintained%3F-NO-red.svg)

Text editor and conversor from CNC Fagor 8025 code to 8035 / 8037 versions.

This software was designed for internal and specific translation requirements. Probably don't work on 100% of the cases. In general, you can convert almost any program from Fagor CNC 8025  into Fagor 8035 NC G-Code. So, if you need to convert from Fagor CNC 8025 to 8035 file, you can use it.

This version is no longer maintained, due corporate restrictions. But I'll leave this repository to everyone that could need it.

_Axia-Traductor_ performs the following conversions and replacements:

##### Replacements and conversions
| Type | Character on 8025 | Character on 8035 | Example |
| ------ | ------ | ------ | ------ |
| Comment line | `(`| `;` | `(I'm a comment)` -> `;(I'm a comment)` |
| Tool selection | `T`| `D` | `T02.03` -> `T02 D03` |
| Angle | `A`| `Q` | `A315.000` -> `Q315.000` |
| Delay | Time in seconds | time: 1/100 seconds | `G04 K0.3` -> `G04 K30` |

##### Code Conversion
 - On 8035 syntax, always starts the program with `%` symbol
 - Prevent any blank line inserting a comment character (`;`)
 - Perform a "prologue" and "epilogue" replacement (blocks of code at the beginning or at the end of the CNC program)

### Software
This software was developed on C++ with [wxWidgets], using the following IDEs
 - [Zinjai] - An open source C/C++ IDE (integrated development environment)
 - [wxFormBuilder] -  An open source GUI designer application for wxWidgets toolkit

### Libraries
 - [wxWidgets]
 - [SFML] to FTP connection utility
 
### Features
This software is not just a translator. It's a totally functional text editor with syntax highlight (implemented _ad hoc_).

### Current status
The software is totally functional.
Notice: Not all assign sentences `=` are converted.

Explaination:

Since variable enumeration on 8035 and 8025 are both different, the sentence are not converted directly.

| Math expression | 8025 syntax | 8035 syntax |
| ------ | ------ | ------ |
| `A = A + B` | `P1 = P1 F1 P2`| `(P101 = P101 + P102)` |
| `A = A - 15` | `P1 = P1 F2 K15`| `(P101 = P101 - K15)` |
| Store the Z pos on a variable | `P1 = Z`| `(P101 = PPOSZ)` |

### Changelog
 - FTP connection utility


#### Disclaimer
This software is provided **_as is_**. [Axia] and I won't provide any warranty of use, or support.
Always make sure that the converted CNC program is correct before execute it. Otherwise it can result on any damage or injuries. Remember: the translation process jumps all the assignations.
This program on this development status is absolutely experimental. **Please, be careful if you use the converted program on a CNC lathe.**

[Zinjai]: <http://zinjai.sourceforge.net/>
[wxFormBuilder]: <https://github.com/wxFormBuilder/wxFormBuilder>
[Axia]: <https://axia.com.ar/>
[SFML]: <https://www.sfml-dev.org/index.php>
[wxWidgets]: <https://www.wxwidgets.org/>