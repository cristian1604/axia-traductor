#ifndef WXACTUALIZACIONES_H
#define WXACTUALIZACIONES_H
#include "wxfb_project.h"

class wxActualizaciones : public ComprobarActualizaciones {
	
private:
	
protected:
	void close( wxCommandEvent& event ) ;
	
public:
	wxActualizaciones(wxWindow *parent=NULL);
	~wxActualizaciones();
};

#endif

