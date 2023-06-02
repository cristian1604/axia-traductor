#ifndef WXSAVEFTP_H
#define WXSAVEFTP_H
#include "wxfb_project.h"

class wxSaveFtp : public saveFtp {
	
private:
	
protected:
	void saveFtpCommand( wxCommandEvent& event ) ;
	void saveFtp( wxCommandEvent& event ) ;
	void evt_key_up( wxKeyEvent& event ) ;
	void search( wxCommandEvent& event ) ;
	
public:
	wxSaveFtp(wxWindow *parent=NULL);
	~wxSaveFtp();
};

#endif

