#ifndef WXACTUALIZACIONES_H
#define WXACTUALIZACIONES_H
#include "ComprobarActualizaciones_base.h"
#include <thread>

struct UpdateCheck;

class wxActualizaciones : public ComprobarActualizaciones {

private:
	std::thread worker;   // consulta HTTP en segundo plano
	void showResult(const UpdateCheck &r);

protected:
	void close( wxCommandEvent& event ) ;

public:
	wxActualizaciones(wxWindow *parent=NULL);
	~wxActualizaciones();
};

#endif
