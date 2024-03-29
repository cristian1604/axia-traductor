#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "wxfb_project.h"
#include "wxSearch.h"
#include "wxOptions.h"
#include "FileManager.h"
#include <SFML/Network.hpp>
#include "wxSaveFtpWindow.h"

class MainWindow : public wxMainWindow {
	
private:
	wxString text_program;        	// Text contained on the input box
	wxString window_title;        	// Window title
	wxString filename;            	// Opened file name
	int syntax_version;           	// 8025, 8035
	bool is_loading;              	// When this variable is on TRUE, the syntax highlight is turned off
	wxSearch *srch;               	// Search Window
	wxString path;                	// current path of loaded file
	s_Settings settings;		  	// Settings object
	sf::Ftp ftp;				  	// FTP Connection
	FileManager FM;				 	// FileManager object
	wxSaveFtpWindow *FtpWindow;		// Save Ftp Window
protected:
	void sendProgramOnFly( wxCommandEvent& event ) ;
	void FtpConnectWas8037( wxCommandEvent& event ) ;
	void FtpConnectWas8035( wxCommandEvent& event ) ;
	void FtpConnectTaki8037( wxCommandEvent& event ) ;
	void openFormSendProgram( wxCommandEvent& event ) ;
	void checkUpdates( wxCommandEvent& event ) ;
	void refreshFtpFileList();
	void FtpRefresh( wxCommandEvent& event ) ;
	void deleteFtpFile( wxCommandEvent& event ) ;
	void RenameFtpFile( wxCommandEvent& event ) ;
	void ftpFileOptions( wxTreeEvent& event ) ;
	void connectFtpMenu( wxCommandEvent& event ) ;
	void FtpConnect8035( wxCommandEvent& event ) ;
	void FtpConnect8037( wxCommandEvent& event ) ;
	void FtpDisconnect( wxCommandEvent& event ) ;
	void openFtpFile( wxMouseEvent& event ) ;
	void connectFTP( int idMachine ) ;
	void simulate( wxCommandEvent& event ) ;
	void paste_formatting( wxCommandEvent& event ) ;
	void channels( wxCommandEvent& event ) ;
	void copy_program_clipboard( wxCommandEvent& event ) ;
	void paste_program_clipboard( wxCommandEvent& event ) ;
	void enum_lines( wxCommandEvent& event ) ;
	void about( wxCommandEvent& event ) ;
	void save_program( wxCommandEvent& event ) ;
	void search_replace_window( wxCommandEvent& event ) ;
	void loadSettings();
	void search_next( wxCommandEvent& event )  override;
	void search_window( wxCommandEvent& event )  override;
	void open_options( wxCommandEvent& event )  override;
	void translate( wxCommandEvent& event )  override;
	void update_syntax_highlight( wxCommandEvent& event )  override;
	void edit_text( wxKeyEvent& event )  override;
	void loadProgramFromFile( wxCommandEvent& event )  override;
public:
	MainWindow(wxWindow *parent=NULL);
	~MainWindow();
};

#endif

