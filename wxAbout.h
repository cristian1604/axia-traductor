#ifndef WXABOUT_H
#define WXABOUT_H
#include "wxfb_project.h"

class wxAbout : public about {
	
private:
	
protected:
	void evt_key_up( wxKeyEvent& event ) ;
	
public:
	wxAbout(wxWindow *parent=NULL);
	~wxAbout();
};

#endif

