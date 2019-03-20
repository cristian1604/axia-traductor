#ifndef WXOPTIONS_H
#define WXOPTIONS_H
#include "wxfb_project.h"

class wxOptions : public wxParameters {
	
private:
	
protected:
	void evt_key_up( wxKeyEvent& event ) ;
	
public:
	wxOptions(wxWindow *parent=NULL);
	~wxOptions();
};

#endif

