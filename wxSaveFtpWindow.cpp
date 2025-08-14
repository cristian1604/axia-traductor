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
		
		FileManager FM("tmp", "tmp.pit");
		
		wxString fileToCopy = "tmp\\" + (save_filename->GetValue()).ToStdString() + ".pit";
		wxCopyFile("tmp\\tmp.pit", fileToCopy, true);
		
		sf::Ftp::Response response = ftp->deleteFile(FM.getFilename());
		response = ftp->upload(fileToCopy.ToStdString(), "", sf::Ftp::Binary);
		if (response.isOk()) {
			wxMessageBox( "Programa transferido como " + (save_filename->GetValue()).ToStdString() + ".pit", "OK", wxICON_INFORMATION);
		}
	} else {
		// Show no connection error
		cout<<"ERROR"<<endl;
		wxMessageBox( "No está conectado al control numérico", "No conectado", wxICON_ERROR);
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

