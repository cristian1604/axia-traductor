#include "MainWindow.h"
#include <wx/filedlg.h>
#include "SyntaxColor.h"
#include "Translator.h"
#include "wxOptions.h"
#include "wxSearch.h"
#include "wxSearchReplace.h"
#include "wxAbout.h"
#include <wx/clipbrd.h>
#include <wx/msgdlg.h>
#include <wx/utils.h>
#include <wx/wx.h>
#include "resources/icons.xpm"
#include <wx/textdlg.h>
#include "wxActualizaciones.h"
#include <unistd.h>
using namespace std;

#define FAGOR_8025 1
#define WAS_8037 2
#define WAS_8035 3
#define TAKI_8037 4

MainWindow::MainWindow(wxWindow *parent) : wxMainWindow(parent) {
	m_textCtrl->SetBackgroundColour(wxColour( 0, 30, 60));
	m_textCtrl->SetFont( wxFont( 12, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxT("Courier New") ) );
	m_textCtrl->SetDefaultStyle(wxTextAttr(*wxYELLOW));
	m_statusBar->SetLabel("Programa iniciado");
	m_statusBar->SetStatusText("8025 -> 8035", 1);
	m_statusBar->SetStatusText("AXIA", 2);
	this->window_title = "Traductor código CNC 8025 / 8035";
	
	//search window
	srch = new wxSearch(this);
	
	// saveFtpWindow
	FtpWindow = new wxSaveFtpWindow(this);
	FtpWindow->inheritFtpConnection(&ftp, m_textCtrl);
	
	srch->assignSearchField(m_textCtrl);
	loadSettings();
}

MainWindow::~MainWindow() {
	ftp.disconnect();
}

/** LOAD PROGRAM FOR 8025 FROM FILE **/
void MainWindow::loadProgramFromFile( wxCommandEvent& event )  {
	wxFileDialog* OpenDialog = new wxFileDialog(this, wxT("Abrir programa para Fagor 8025"), wxEmptyString, wxEmptyString, wxT("Programa de mecanizado (*.NC, *.PIT)|*.NC;*.PIT;*.nc;*.pit|Archivo de texto (*.txt, *.TXT)|*.txt|Todos los archivos|*.*"), wxFD_OPEN, wxDefaultPosition);
	if (OpenDialog->ShowModal() == wxID_OK) // if the user click "Open" instead of "Cancel"
	{
		path = OpenDialog->GetPath();
		filename = OpenDialog->GetFilename();
		this->SetTitle(this->window_title + " - " + filename);
		FileManager FM(path, filename);
		bool flag = FM.readFile(this->text_program);
		if (flag) {
			syntax_version = FAGOR_8025;
			m_syntax_slection->SetSelection(0);
			is_loading = true;
			m_statusBar->SetStatusText("Leyendo archivo...", 0);
			m_textCtrl->SetValue("");
			m_textCtrl->SetValue(this->text_program);
			syntax_highlight(m_textCtrl, syntax_version, settings);
			m_textCtrl->SetInsertionPoint(0);
			is_loading = false;
			m_statusBar->SetStatusText("Archivo cargado: " + filename, 0);
		}
		// Sets our current document to the file the user selected
		//MainEditBox->LoadFile(CurrentDocPath); //Opens that file
		//SetTitle(wxString("Edit - ") << OpenDialog->GetFilename()); // Set the Title to reflect the file open
	}
	OpenDialog->Destroy();
	
}

void MainWindow::edit_text( wxKeyEvent& event )  {
	event.Skip();
}

void MainWindow::update_syntax_highlight( wxCommandEvent& event )  {
	if (is_loading) return;
	switch (m_syntax_slection->GetSelection()) {
	case 0:
		syntax_version = FAGOR_8025;
		break;
	case 1:
		syntax_version = WAS_8035;
		break;
	}
	is_loading = true;
	//int ip = m_textCtrl->GetInsertionPoint();
	syntax_highlight(m_textCtrl, syntax_version, settings);
	m_textCtrl->SetFocus();
	// m_textCtrl->SetInsertionPoint(ip);
	is_loading = false;
}

/**  TRANSLATION  **/
void MainWindow::translate( wxCommandEvent& event )  {
	is_loading = true;
	if (syntax_version == WAS_8035) {
		wxMessageBox("El código ya se encuentra en la versión 8035",
					 "Traducir a 8025",
					 wxOK);
		is_loading = false;
		return;
	}
	m_statusBar->SetStatusText("Analizando...", 0);
	int ip = m_textCtrl->GetInsertionPoint();
	m_syntax_slection->SetSelection(1);
	syntax_version = WAS_8035;
	translate_8025_to_8035(m_textCtrl);
	syntax_highlight(m_textCtrl, syntax_version, settings);
	m_textCtrl->SetFocus();
	m_textCtrl->SetInsertionPoint(ip);
	is_loading = false;
	enum_lines(event);
	m_statusBar->SetStatusText("Programa convertido a versión 8035", 0);
}

void MainWindow::open_options( wxCommandEvent& event )  {
	wxOptions *opt = new wxOptions(this);
	opt->ShowModal();
	loadSettings();
}

void MainWindow::search_window( wxCommandEvent& event )  {
	srch->ShowModal();
}

void MainWindow::search_next( wxCommandEvent& event )  {
	srch->search_next();
}

void MainWindow::search_replace_window( wxCommandEvent& event )  {
	wxSearchReplace *snr = new wxSearchReplace(this);
	snr->assignTextField(m_textCtrl);
	snr->ShowModal();
}

/** SAVE PROGRAM GENERATED **/
void MainWindow::save_program( wxCommandEvent& event )  {
	if (!FM.isDefined()) {
		wxString suggested = filename.SubString(0,filename.Find('.') - 1) + "_35";
		wxFileDialog* OpenDialog = new wxFileDialog(this, wxT("Guardar programa"), wxEmptyString, suggested, wxT("Programa de mecanizado PIT (*.PIT)|*.PIT|Programa de mecanizado NC (*.NC)|*.NC|Archivo de texto (*.txt)|*.txt|Todos los archivos|*.*"), wxFD_SAVE, wxDefaultPosition);
		if (OpenDialog->ShowModal() == wxID_OK) {
			this->text_program = m_textCtrl->GetValue();
			path = OpenDialog->GetPath();
			FileManager FM(path, OpenDialog->GetFilename());
			if (FM.writeFile(this->text_program)) {
				m_statusBar->SetStatusText("Guardado.",0);
			}
		}
	} else {
		this->text_program = m_textCtrl->GetValue();
		if (FM.writeFile(this->text_program)) {
			m_statusBar->SetStatusText("Guardado.",0);
		}
	}
	
	// Now check and send file through FTP
	sf::Ftp::DirectoryResponse directory = ftp.getWorkingDirectory();
	if (directory.isOk()) {
		char tmp[256];
		getcwd(tmp, 256);
		string aa(tmp);
		
		string origin = "tmp\\" + FM.getFilename();
		ftp.keepAlive();
		
		sf::Ftp::Response response = ftp.deleteFile(FM.getFilename());
		if (response.isOk()) {
			std::cout<<response.getStatus()<<std::endl;
			response = ftp.upload(origin, "", sf::Ftp::Binary);
			
			if (response.isOk()) {
				m_statusBar->SetStatusText("Guardado y enviado al control como " + FM.getFilename(),0);
			}
		}
	}
	
}

void MainWindow::about( wxCommandEvent& event )  {
	wxAbout *a = new wxAbout(this);
	a->ShowModal();
}

/**  Dady's re-enumerator lines library call **/
void MainWindow::enum_lines( wxCommandEvent& event )  {
	int pos = m_textCtrl->GetInsertionPoint();
	bool partial = false;
	long x;
	
	// If the code is on 8035, we need to convert only the program and dismiss the comments section
	text_program = m_textCtrl->GetValue();
	if (syntax_version == WAS_8035) {
		partial = true;
		x = text_program.Find("N0010");   // search the initial line
		text_program = text_program.SubString(x, text_program.Length());
		
		text_program = "%tmp\n" + text_program; // temporal line
	}
	
	if (x < 1 && text_program.Find("%") < 0) {
		wxMessageBox( "No se detectó el inicio de programa.\nRecuerde iniciar el programa con el caracter %", "Inicio de programa no encontrado", wxICON_ERROR);
		return;
	}
	
	// Write some text to the clipboard
	if (wxTheClipboard->Open())	{
		// This data objects are held by the clipboard,
		// so do not delete them in the app.
		wxTheClipboard->SetData( new wxTextDataObject(text_program));
		wxTheClipboard->Close();
		
		wxExecute("Num2.exe",wxEXEC_SYNC);
	}
	
	if (!partial) {
		paste_program_clipboard(event);
	} else {
		// Store on a temp variable the content before the beginning "N0010"
		wxString tmp = m_textCtrl->GetValue();
		tmp = tmp.SubString(0, x-1);

		wxTextDataObject data;
		wxTheClipboard->GetData( data );
		text_program = data.GetText();
		text_program = tmp + text_program.SubString(5, text_program.Length());
		m_textCtrl->SetValue(text_program);
		m_textCtrl->SetFocus();
	}
	
	m_textCtrl->SetInsertionPoint(pos);
}

void MainWindow::loadSettings() {
	FileManager F;
	if (F.loadSettings(settings)) {
		wxColour col = settings.colour_textCtrl;
		m_textCtrl->SetBackgroundColour(col);
		this->Maximize(settings.maximize_on_startup);
	}
}

void MainWindow::copy_program_clipboard( wxCommandEvent& event )  {
	if (wxTheClipboard->Open())	{
		// This data objects are held by the clipboard,
		// so do not delete them in the app.
		wxTheClipboard->SetData( new wxTextDataObject(m_textCtrl->GetValue()));
		wxTheClipboard->Close();
	}
}

void MainWindow::paste_program_clipboard( wxCommandEvent& event )  {
	if (wxTheClipboard->Open()) {
		if (wxTheClipboard->IsSupported( wxDF_TEXT )) {
			wxTextDataObject data;
			wxTheClipboard->GetData( data );
			text_program = data.GetText();
			m_textCtrl->SetValue(text_program);
			m_textCtrl->SetFocus();
		}
		wxTheClipboard->Close();
	}
}

void MainWindow::paste_formatting( wxCommandEvent& event )  {
	text_program = m_textCtrl->GetValue();
	long aaa = m_textCtrl->GetScrollPos(wxVERTICAL);
	long x = m_textCtrl->GetInsertionPoint();
	text_program = text_program.SubString(0, x);

	if (wxTheClipboard->Open()) {
		if (wxTheClipboard->IsSupported( wxDF_TEXT )) {
			wxTextDataObject data;
			wxTheClipboard->GetData( data );
			wxString aux = data.GetText();
			aux.Replace(',', '.');
			text_program += aux + (m_textCtrl->GetValue()).SubString(x, (m_textCtrl->GetValue()).Length());
			
			m_textCtrl->SetValue(text_program);
			m_textCtrl->SetInsertionPoint(x + aux.Length());
			m_textCtrl->SetScrollPos(wxVERTICAL, aaa*2);
			m_textCtrl->SetFocus();
		}
		wxTheClipboard->Close();
	}
}

void MainWindow::channels( wxCommandEvent& event )  {
	// Execution of external program
	// Not included on this repository due copyright restrictions
	if (syntax_version != FAGOR_8025) {
		wxMessageBox( "Solo puede simular programas de 8025.\nEl código G actual es 8035", "Versión G no compatible", wxICON_ERROR);
		return;
	}
	wxExecute("Canalesw.exe");
}

void MainWindow::simulate( wxCommandEvent& event )  {
	// Execution of external program
	// Not included on this repository due copyright restrictions
	if (syntax_version != FAGOR_8025) {
		wxMessageBox( "Solo puede simular programas de 8025.\nEl código G actual es 8035", "Versión G no compatible", wxICON_ERROR);
		return;
	}
	FileManager F("tmp.txt", "tmp.txt");
	text_program = m_textCtrl->GetValue();
	if (F.writeFile(text_program)) {
		wxExecute("ABsim.exe tmp.txt");
	}
}

///**  FTP OPTIONS  ** ///

void MainWindow::connectFTP( int idMachine )  {
	wxString conn;
	ftp.disconnect();
	m_treeCtrl1->DeleteAllItems();
	
	switch (idMachine) {
	case WAS_8035:
		ftp.connect("192.168.100.81", 21, sf::seconds(5));
		conn = "WASINO 8035";
		break;
	case TAKI_8037:
		ftp.connect("192.168.100.80", 21, sf::seconds(5));
		conn = "TAKISAWA 8037";
		break;
	case WAS_8037:
		ftp.connect("192.168.100.82", 21, sf::seconds(5));
		conn = "WASINO 8037";
		break;
	}
	ftp.login();
	ftp.changeDirectory("/disk/prg/");
	
	m_statusBar->SetStatusText("Conectado a " + conn, 1);
	ftp.keepAlive();
	refreshFtpFileList();
}

/** Double click on FTP file**/
void MainWindow::openFtpFile( wxMouseEvent& event)  {
	wxTreeItemId item = m_treeCtrl1->GetSelection();
	filename = m_treeCtrl1->GetItemText(item);
	m_statusBar->SetLabel("Descargando " + m_treeCtrl1->GetItemText(item));
	
	sf::Ftp::ListingResponse response = ftp.getDirectoryListing();
	sf::Ftp::Response r = ftp.download(std::string((m_treeCtrl1->GetItemText(item)).mb_str()), "tmp\\", sf::Ftp::Binary);
	
	if (r.isOk()) {
		m_statusBar->SetLabel("Archivo descargado.");
	} else {
		m_statusBar->SetLabel("Error " + r.getStatus());
	}
	
	m_statusBar->SetLabel("Abriendo archivo");
	
	this->SetTitle(this->window_title + " - " + filename);
	FM = FileManager("tmp\\" + filename, filename);
	bool flag = FM.readFile(this->text_program);
	if (flag) {
		syntax_version = WAS_8035;
		m_syntax_slection->SetSelection(0);
		is_loading = true;
		m_statusBar->SetStatusText("Leyendo archivo...", 0);
		m_textCtrl->SetValue("");
		m_textCtrl->SetValue(this->text_program);
		syntax_highlight(m_textCtrl, syntax_version, settings);
		m_textCtrl->SetInsertionPoint(0);
		is_loading = false;
		m_statusBar->SetStatusText("Archivo abierto del torno: " + filename, 0);
	}
}

void MainWindow::FtpDisconnect( wxCommandEvent& event )  {
	ftp.disconnect();
	m_treeCtrl1->DeleteAllItems();
	m_statusBar->SetStatusText("Local. 8025 -> 8035", 1);
	m_statusBar->SetStatusText("Desconectado", 0);
}

void MainWindow::connectFtpMenu( wxCommandEvent& event )  {
	PopupMenu(ftpOptions);
}

void MainWindow::FtpConnectWas8035( wxCommandEvent& event )  {
	connectFTP(WAS_8035);
}

void MainWindow::FtpConnectTaki8037( wxCommandEvent& event )  {
	connectFTP(TAKI_8037);
}

void MainWindow::FtpConnectWas8037( wxCommandEvent& event )  {
	connectFTP(WAS_8037);
}

void MainWindow::deleteFtpFile( wxCommandEvent& event )  {
	sf::Ftp::DirectoryResponse directory = ftp.getWorkingDirectory();
	wxTreeItemId item = m_treeCtrl1->GetSelection();
	if (directory.isOk()) {
		string file(m_treeCtrl1->GetItemText(item));
		sf::Ftp::Response r = ftp.deleteFile(file);
		if (r.isOk()) {
			m_treeCtrl1->Delete(item);
			refreshFtpFileList();
			m_statusBar->SetLabel("Archivo eliminado");
		} else {
			m_statusBar->SetLabel("Error al intentar borrar el archivo");
		}
	}
}

void MainWindow::RenameFtpFile( wxCommandEvent& event )  {
	sf::Ftp::DirectoryResponse directory = ftp.getWorkingDirectory();
	wxTreeItemId item = m_treeCtrl1->GetSelection();
	if (!directory.isOk()) {
		m_statusBar->SetLabel("FTP no conectado");
		return;
	}
	
	wxString fname = wxGetTextFromUser ("Renombrar",
					   "Renombrar archivo",
					   m_treeCtrl1->GetItemText(item));
	if (!fname.IsEmpty()) {
		if (fname.Find(wxT(".pit")) == -1) {
			fname += ".pit";
		}
		fname.Replace(' ', '_', true);
		ftp.renameFile(std::string((m_treeCtrl1->GetItemText(item)).mb_str()), std::string(fname.mb_str()));
		refreshFtpFileList();
	}
}

void MainWindow::ftpFileOptions( wxTreeEvent& event )  {
	event.Skip();
}

void MainWindow::FtpRefresh( wxCommandEvent& event )  {
	refreshFtpFileList();
}

void MainWindow::refreshFtpFileList() {
	sf::Ftp::DirectoryResponse directory = ftp.getWorkingDirectory();
	wxTreeItemId item = m_treeCtrl1->GetSelection();
	if (!directory.isOk()) {
		m_statusBar->SetLabel("FTP no conectado");
		return;
	}
	
	m_treeCtrl1->DeleteAllItems();
	sf::Ftp::ListingResponse response = ftp.getDirectoryListing();
	wxTreeItemId raiz;
	wxString aux = m_statusBar->GetStatusText(1);
	aux = aux.SubString(12,aux.Length());							// Substract string "conectado a"
	if (response.isOk()) {
		const std::vector<std::string>& listing = response.getListing();
		raiz = m_treeCtrl1->AddRoot(aux, 1);
		for (std::vector<std::string>::const_iterator it = listing.begin(); it != listing.end(); ++it) {
			m_treeCtrl1->AppendItem(raiz, *it, 2);
		}
		
		m_treeCtrl1->SortChildren(raiz);
		m_treeCtrl1->Expand(raiz);
		
		wxImageList* imageList = new wxImageList(16, 16);
		imageList->Add(wxIcon(folder_xpm));								// 0
		imageList->Add(wxIcon(server_xpm));								// 1
		imageList->Add(wxIcon(pit_extension_xpm));						// 2
		m_treeCtrl1->AssignImageList(imageList);
		
		ftp.keepAlive();
		//m_statusBar->SetStatusText("Conectado a " + aux, 1);
		m_statusBar->SetStatusText("Directorio listado correctamente", 0);
	}
}

void MainWindow::checkUpdates( wxCommandEvent& event )  {
	wxActualizaciones* w = new wxActualizaciones(this);
	w->CenterOnParent();
	w->Show(true);
}

void MainWindow::openFormSendProgram( wxCommandEvent& event )  {
	wxExecute("EnvioCNC.exe");
}

/** Enviar programa al vuelo (sin nombre de archivo de destino) */
void MainWindow::sendProgramOnFly( wxCommandEvent& event )  {
	if (FtpWindow->checkConnection()) {
		FtpWindow->ShowModal();
	}
	this->refreshFtpFileList();
}

