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
		
		wxString targetName = save_filename->GetValue() + ".pit";
		FileManager source("tmp", "tmp.pit");
		FileManager target("tmp", targetName);
		wxCopyFile(source.getFullPath(), target.getFullPath(), true);
		
		// Se borra el archivo de destino en el control (si existe) para poder
		// sobreescribirlo. Antes se borraba "tmp.pit", que no es el destino.
		ftp->deleteFile(target.getFilename());
		sf::Ftp::Response response = ftp->upload(target.getFullPath(), "", sf::Ftp::Binary);
		if (response.isOk()) {
			wxMessageBox( "Programa transferido como " + targetName, "OK", wxICON_INFORMATION);
		} else {
			wxMessageBox( "No se pudo transferir el programa al control", "Error", wxICON_ERROR);
		}
	} else {
		// Show no connection error
		cout<<"ERROR"<<endl;
		wxMessageBox( wxT("No está conectado al control numérico"), "No conectado", wxICON_ERROR);
	}
	
	this->Close();
}

bool wxSaveFtpWindow::checkConnection( )  {
	sf::Ftp::DirectoryResponse directory = ftp->getWorkingDirectory();
	if (directory.isOk()) {
		return true;
	} else {
		wxMessageBox( wxT("Antes debe conectarse a algún control numérico"), "No conectado", wxICON_ERROR);
		return false;
	}
}

void wxSaveFtpWindow::inheritFtpConnection( sf::Ftp *ftpInh, wxTextCtrl *x )  {
	ftp = ftpInh;
	textCtrl = x;
}

wxSaveFtpWindow::~wxSaveFtpWindow() {
	
}

