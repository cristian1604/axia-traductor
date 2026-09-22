#ifndef WXABOUT_H
#define WXABOUT_H
#include "wxAboutBase.h"

class wxAbout : public wxAboutBase {
	
private:
	
protected:
	void searchUpdate( wxCommandEvent& event ) ;
	void evt_key_up( wxKeyEvent& event ) ;
	
public:
	wxAbout(wxWindow *parent=NULL);
	~wxAbout();
};

#endif

