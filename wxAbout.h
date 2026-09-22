#ifndef WXABOUT_H
#define WXABOUT_H
#include "about_base.h"

class wxAbout : public about {
	
private:
	
protected:
	void searchUpdate( wxCommandEvent& event ) ;
	void evt_key_up( wxKeyEvent& event ) ;
	
public:
	wxAbout(wxWindow *parent=NULL);
	~wxAbout();
};

#endif

