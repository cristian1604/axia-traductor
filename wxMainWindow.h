#ifndef WXMAINWINDOW_H
#define WXMAINWINDOW_H
#include "wxMainWindowBase.h"
#include "wxCncEditor.h"
#include "wxPlotPanel.h"
#include "CncPath.h"
#include "wxSearch.h"
#include "wxOptions.h"
#include "FileManager.h"
#include <SFML/Network.hpp>
#include "Machines.h"
#include <wx/timer.h>
#include <wx/splitter.h>
#include <vector>


class wxMainWindow : public wxMainWindowBase {

private:
	wxString text_program;        	// Text contained on the input box
	wxString window_title;        	// Window title
	wxString filename;            	// Opened file name
	int syntax_version;           	// CncStandard activo: 8025, 8035 o FANUC
	wxSearch *srch;               	// Search Window
	wxString path;                	// current path of loaded file
	s_Settings settings;		  	// Settings object
	sf::Ftp ftp;				  	// FTP Connection
	FileManager FM;				 	// Archivo de destino del guardado (definido tras "Guardar como" o al abrir desde FTP)
	std::vector<Machine> machines;	// Lista compartida de tornos (machines.json)
	wxString connected_machine;		// Nombre del torno conectado en el explorador FTP (vacío si ninguno)
	void openSendDialog();
	bool readClipboardText(wxString &out);
	bool writeClipboardText(const wxString &text);
	// Carga un programa en el editor con la sintaxis indicada y descarta el historial de deshacer
	void showProgram(const wxString &program, int standard);
	// Posición inicial del divisor, escalada al DPI del monitor (reemplaza al idle de wxFormBuilder)
	void splitterFirstIdle(wxIdleEvent &event);
	// Cambios sin guardar: "*" en el título y confirmación antes de cerrar o reemplazar el programa
	void updateTitle();
	void onModifiedChanged(wxStyledTextEvent &event);
	void onClose(wxCloseEvent &event);
	bool confirmDiscard(const wxString &action);
	bool saveProgram();

	// Graficador: panel junto al editor (a la derecha o debajo), sincronizado
	// con el texto (con un pequeño retardo) y con la línea del cursor
	wxTimer plot_timer;
	CncPath plot_path;
	bool status_has_plot_message;
	wxMenuItem *m_menuPlotShow, *m_menuPlotRight, *m_menuPlotBelow, *m_menuPlotRapids;
	void buildPlotMenu();
	bool plotVisible() const;
	void applyPlotLayout();
	void updatePlot();
	void savePlotSettings();
	void togglePlot(wxCommandEvent &event);
	void plotLayoutChanged(wxCommandEvent &event);
	void plotRapids(wxCommandEvent &event);
	void plotFit(wxCommandEvent &event);
	void onPlotSash(wxSplitterEvent &event);
	void onPlotTimer(wxTimerEvent &event);
	void onEditorChanged(wxStyledTextEvent &event);
	void onEditorUpdateUI(wxStyledTextEvent &event);
protected:
	void sendProgramOnFly( wxCommandEvent& event ) ;
	void openFormSendProgram( wxCommandEvent& event ) ;
	void checkUpdates( wxCommandEvent& event ) ;
	void refreshFtpFileList();
	void FtpRefresh( wxCommandEvent& event ) ;
	void deleteFtpFile( wxCommandEvent& event ) ;
	void RenameFtpFile( wxCommandEvent& event ) ;
	void ftpFileOptions( wxTreeEvent& event ) ;
	void connectFtpMenu( wxCommandEvent& event ) ;
	void FtpDisconnect( wxCommandEvent& event ) ;
	void openFtpFile( wxMouseEvent& event ) ;
	void connectFTP( const std::string &machineName ) ;
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
	void translateFanuc( wxCommandEvent& event )  override;
	void update_syntax_highlight( wxCommandEvent& event )  override;
	void loadProgramFromFile( wxCommandEvent& event )  override;
public:
	wxMainWindow(wxWindow *parent=NULL);
	~wxMainWindow();
};

#endif
