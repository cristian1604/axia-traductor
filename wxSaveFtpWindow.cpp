#include "wxSaveFtpWindow.h"

using namespace std;

wxSaveFtpWindow::wxSaveFtpWindow(wxWindow *parent) : saveFtpWindow(parent) {
	
}

void wxSaveFtpWindow::evt_key_up( wxKeyEvent& event )  {
	event.Skip();
}

void wxSaveFtpWindow::saveFtpCommand( wxCommandEvent& event )  {
	sf::Ftp::DirectoryResponse directory = ftp->getWorkingDirectory();
	if (directory.isOk()) {
		ftp->keepAlive();
		string origin = "tmp\\" + (save_filename->GetValue()).ToStdString() + ".pit";
		FileManager FM(origin, save_filename->GetValue());
		wxString text(textCtrl->GetValue());
		FM.writeFile(text);
		
		sf::Ftp::Response response = ftp->deleteFile(FM.getFilename());
		response = ftp->upload(origin, "", sf::Ftp::Binary);
		cout<<"OK"<<endl;
	} else {
		// Show no connection error
		cout<<"ERROR"<<endl;
		wxMessageBox( "No está conectado al control", "No conectado", wxICON_ERROR);
	}
	
	this->Close();
}

bool wxSaveFtpWindow::checkConnection( )  {
	sf::Ftp::DirectoryResponse directory = ftp->getWorkingDirectory();
	if (directory.isOk()) {
		return true;
	} else {
		wxMessageBox( "Antes debe conectarse a algún control numérico", "No conectado", wxICON_ERROR);
		return false;
	}
	this->Close();
}

void wxSaveFtpWindow::inheritFtpConnection( sf::Ftp *ftpInh, wxTextCtrl *x )  {
	ftp = ftpInh;
	textCtrl = x;
}

wxSaveFtpWindow::~wxSaveFtpWindow() {
	
}

