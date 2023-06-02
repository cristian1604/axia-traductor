#ifndef WXSAVEFTPWINDOW_H
#define WXSAVEFTPWINDOW_H
#include "wxfb_project.h"
#include <SFML/Network.hpp>
#include "FileManager.h"
#include <wx/textctrl.h>
#include <wx/msgdlg.h>
#include <string>

class wxSaveFtpWindow : public saveFtpWindow {
	
private:
	wxString fname;					// FileName
	sf::Ftp *ftp;				  	// FTP Connection
	wxTextCtrl *textCtrl;			// Source code (CNC program)
protected:
	void evt_key_up( wxKeyEvent& event ) ;
	void saveFtpCommand( wxCommandEvent& event ) ;
public:
	void inheritFtpConnection(sf::Ftp *ftpInh, wxTextCtrl *x);
	bool checkConnection();
	wxSaveFtpWindow(wxWindow *parent=NULL);
	~wxSaveFtpWindow();
};

#endif

