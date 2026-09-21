#ifndef CNCSTANDARD_H
#define CNCSTANDARD_H

// Identificadores de control numérico / sintaxis. Compartidos por toda la
// aplicación (antes MainWindow.cpp y SyntaxColor.cpp definían valores
// distintos para los mismos nombres y el coloreado nunca coincidía).
enum CncStandard {
	FAGOR_8025 = 1,
	WAS_8037   = 2,
	WAS_8035   = 3,
	TAKI_8037  = 4,
	KIA_FANUC  = 5
};

#endif
