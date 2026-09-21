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
#include <wx/log.h>
#include "resources/icons.xpm"
#include <wx/textdlg.h>
#include "wxActualizaciones.h"
#include <wx/filefn.h>
#include <wx/dir.h>
#include "CncStandard.h"
using namespace std;

MainWindow::MainWindow(wxWindow *parent) : wxMainWindow(parent),
	syntax_version(FAGOR_8025), is_loading(false), srch(NULL), FtpWindow(NULL) {
	m_textCtrl->SetBackgroundColour(wxColour( 0, 30, 60));
	m_textCtrl->SetFont( wxFont( 12, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxT("Courier New") ) );
	m_textCtrl->SetDefaultStyle(wxTextAttr(*wxYELLOW));
	m_statusBar->SetLabel("Programa iniciado");
	m_statusBar->SetStatusText("8025 -> 8035 / 8037 / FANUC", 1);
	m_statusBar->SetStatusText("AXIA", 2);
	this->window_title = wxT("Traductor código CNC 8025 a 8035 / 8037 / FANUC");
	
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
	wxFileDialog OpenDialog(this, wxT("Abrir programa para Fagor 8025"), wxEmptyString, wxEmptyString, wxT("Programa de mecanizado (*.NC, *.PIT)|*.NC;*.PIT;*.nc;*.pit|Archivo de texto (*.txt, *.TXT)|*.txt|Todos los archivos|*.*"), wxFD_OPEN, wxDefaultPosition);
	if (OpenDialog.ShowModal() == wxID_OK) // if the user click "Open" instead of "Cancel"
	{
		path = OpenDialog.GetPath();
		filename = OpenDialog.GetFilename();
		this->SetTitle(this->window_title + " - " + filename);
		// El archivo original 8025 no se usa como destino de "Guardar": se
		// pide un nombre nuevo para no pisar el original con la traduccion.
		FM = FileManager();
		FileManager source(path);
		bool flag = source.readFile(this->text_program);
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
	}
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
		wxMessageBox(wxT("El código ya se encuentra en la versión 8035"),
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
	m_statusBar->SetStatusText(wxT("Programa convertido a versión 8035"), 0);
}

void MainWindow::translateFanuc( wxCommandEvent& event )  {
	is_loading = true;
	if (syntax_version == KIA_FANUC) {
		wxMessageBox(wxT("El código ya se encuentra en la versión FANUC"),
					 "Traducir a FANUC",
					 wxOK);
		is_loading = false;
		return;
	}
	m_statusBar->SetStatusText("Analizando...", 0);
	int ip = m_textCtrl->GetInsertionPoint();
	m_syntax_slection->SetSelection(1);
	syntax_version = KIA_FANUC;
	translate_8025_to_Fanuc(m_textCtrl);
	syntax_highlight(m_textCtrl, syntax_version, settings);
	m_textCtrl->SetFocus();
	m_textCtrl->SetInsertionPoint(ip);
	is_loading = false;
	enum_lines(event);
	m_statusBar->SetStatusText(wxT("Programa convertido a versión Fanuc"), 0);
}

void MainWindow::open_options( wxCommandEvent& event )  {
	wxOptions opt(this);
	opt.ShowModal();
	loadSettings();
}

void MainWindow::search_window( wxCommandEvent& event )  {
	srch->ShowModal();
}

void MainWindow::search_next( wxCommandEvent& event )  {
	srch->search_next();
}

void MainWindow::search_replace_window( wxCommandEvent& event )  {
	wxSearchReplace snr(this);
	snr.assignTextField(m_textCtrl);
	snr.ShowModal();
}

/** SAVE PROGRAM GENERATED **/
void MainWindow::save_program( wxCommandEvent& event )  {
	if (!FM.isDefined()) {
		wxString base = filename.BeforeLast('.');
		if (base.IsEmpty()) base = filename;
		wxString suggested = base + "_35";
		wxFileDialog SaveDialog(this, wxT("Guardar programa"), wxEmptyString, suggested, wxT("Programa de mecanizado PIT (*.PIT)|*.PIT|Programa de mecanizado NC (*.NC)|*.NC|Archivo de texto (*.txt)|*.txt|Todos los archivos|*.*"), wxFD_SAVE | wxFD_OVERWRITE_PROMPT, wxDefaultPosition);
		if (SaveDialog.ShowModal() != wxID_OK) {
			return;   // cancelado: no guardar ni enviar nada
		}
		path = SaveDialog.GetPath();
		filename = SaveDialog.GetFilename();
		// Se asigna al miembro (antes se declaraba un FM local que lo ocultaba,
		// por lo que cada "Guardar" volvia a pedir el nombre)
		FM = FileManager(path);
		this->SetTitle(this->window_title + " - " + filename);
	}
	
	this->text_program = m_textCtrl->GetValue();
	if (!FM.writeFile(this->text_program)) {
		wxMessageBox( "No se pudo guardar el archivo:\n" + FM.getFullPath(), "Error al guardar", wxICON_ERROR);
		return;
	}
	m_statusBar->SetStatusText("Guardado: " + filename, 0);
	
	// Si hay un control conectado, ademas se envia el programa por FTP
	sf::Ftp::DirectoryResponse directory = ftp.getWorkingDirectory();
	if (directory.isOk()) {
		ftp.keepAlive();
		if (!ensureTmpDir()) return;
		FileManager tmpFile("tmp", filename);
		if (!tmpFile.writeFile(this->text_program)) return;
		
		ftp.deleteFile(tmpFile.getFilename());
		sf::Ftp::Response response = ftp.upload(tmpFile.getFullPath(), "", sf::Ftp::Binary);
		if (response.isOk()) {
			m_statusBar->SetStatusText("Guardado y transferido al control como " + filename, 0);
			refreshFtpFileList();
		} else {
			wxMessageBox( wxT("Se guardó localmente pero no se pudo transferir al control"), "Error de transferencia", wxICON_ERROR);
		}
	}
}

bool MainWindow::ensureTmpDir() {
	if (wxDir::Exists("tmp")) return true;
	if (wxMkdir("tmp")) return true;
	wxMessageBox( "No se pudo crear el directorio temporal \"tmp\"", "Error", wxICON_ERROR);
	return false;
}

bool MainWindow::readClipboardText(wxString &out) {
	bool ok = false;
	if (wxTheClipboard->Open()) {
		if (wxTheClipboard->IsSupported( wxDF_TEXT )) {
			wxTextDataObject data;
			wxTheClipboard->GetData( data );
			out = data.GetText();
			ok = true;
		}
		wxTheClipboard->Close();
	}
	return ok;
}

bool MainWindow::writeClipboardText(const wxString &text) {
	if (!wxTheClipboard->Open()) return false;
	// The data object is owned by the clipboard, do not delete it
	wxTheClipboard->SetData( new wxTextDataObject(text));
	wxTheClipboard->Close();
	return true;
}

void MainWindow::about( wxCommandEvent& event )  {
	wxAbout a(this);
	a.ShowModal();
}

/**  Dady's re-enumerator lines library call **/
void MainWindow::enum_lines( wxCommandEvent& event )  {
	int pos = m_textCtrl->GetInsertionPoint();
	bool partial = false;
	long x = -1;
	
	// If the code is on 8035, we need to convert only the program and dismiss the comments section
	wxString original = m_textCtrl->GetValue();
	text_program = original;
	if (syntax_version == WAS_8035) {
		x = original.Find("N0010");   // search the initial line
		if (x < 0) {
			wxMessageBox( wxT("No se detectó la primera línea del programa (N0010).\nNo se puede reenumerar parcialmente."), "Inicio de programa no encontrado", wxICON_ERROR);
			return;
		}
		partial = true;
		text_program = "%tmp\n" + original.Mid(x); // temporal line
	}
	
	if (!partial && text_program.Find("%") < 0) {
		wxMessageBox( wxT("No se detectó el inicio de programa.\nRecuerde iniciar el programa con el caracter %"), "Inicio de programa no encontrado", wxICON_ERROR);
		return;
	}
	
	// El reenumerador es una aplicacion externa que trabaja sobre el portapapeles
	if (!writeClipboardText(text_program)) {
		wxMessageBox( "No se pudo acceder al portapapeles", "Error", wxICON_ERROR);
		return;
	}
	long rc;
	{
		wxLogNull noLog;   // evita el dialogo de error propio de wx si no existe el ejecutable
		rc = wxExecute("Num2.exe", wxEXEC_SYNC);
	}
	if (rc == -1) {
		wxMessageBox( "No se pudo ejecutar el reenumerador externo (Num2.exe).\nEl programa queda sin reenumerar.", "Reenumerador no disponible", wxICON_ERROR);
		return;
	}
	
	wxString renumbered;
	if (!readClipboardText(renumbered) || renumbered.IsEmpty()) {
		wxMessageBox( wxT("El reenumerador no devolvió ningún resultado"), "Error", wxICON_ERROR);
		return;
	}
	
	if (!partial) {
		text_program = renumbered;
	} else {
		// Se conserva lo anterior a "N0010" y se descarta la linea temporal "%tmp"
		text_program = original.Left(x) + renumbered.AfterFirst('\n');
	}
	
	// FANUC identifica el programa con "O" en lugar de "%"
	if (syntax_version == KIA_FANUC) {
		int p = text_program.Find(wxT('%'));
		if (p >= 0) text_program[p] = 'O';
	}
	
	m_textCtrl->SetValue(text_program);
	m_textCtrl->SetFocus();
	m_textCtrl->SetInsertionPoint(pos);
}

void MainWindow::loadSettings() {
	FileManager F;
	F.loadSettings(settings);   // sin archivo quedan los valores por defecto
	m_textCtrl->SetBackgroundColour(settings.colour_textCtrl);
	if (settings.maximize_on_startup) {
		this->Maximize(true);
	}
}

void MainWindow::copy_program_clipboard( wxCommandEvent& event )  {
	writeClipboardText(m_textCtrl->GetValue());
}

void MainWindow::paste_program_clipboard( wxCommandEvent& event )  {
	if (readClipboardText(text_program)) {
		m_textCtrl->SetValue(text_program);
		m_textCtrl->SetFocus();
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
			aux.Replace(wxT(','), wxT('.'));
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
		wxMessageBox( wxT("Solo puede simular programas de 8025.\nEl código G actual es 8035"), wxT("Versión G no compatible"), wxICON_ERROR);
		return;
	}
	wxExecute("Canalesw.exe");
}

void MainWindow::simulate( wxCommandEvent& event )  {
	// Execution of external program
	// Not included on this repository due copyright restrictions
	if (syntax_version != FAGOR_8025) {
		wxMessageBox( wxT("Solo puede simular programas de 8025.\nEl código G actual es 8035"), wxT("Versión G no compatible"), wxICON_ERROR);
		return;
	}
	FileManager F("tmp.txt");
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
	if (!item.IsOk() || item == m_treeCtrl1->GetRootItem()) return;
	filename = m_treeCtrl1->GetItemText(item);
	
	if (!ensureTmpDir()) return;
	
	m_statusBar->SetStatusText("Descargando " + filename, 0);
	
	sf::Ftp::Response r = ftp.download(filename.ToStdString(), "tmp", sf::Ftp::Binary);
	if (!r.isOk()) {
		wxMessageBox( wxString::Format(wxT("No se pudo descargar el archivo (código %d)"), (int) r.getStatus()), "Error de descarga", wxICON_ERROR);
		m_statusBar->SetStatusText("Error al descargar " + filename, 0);
		return;
	}
	
	this->SetTitle(this->window_title + " - " + filename);
	FM = FileManager("tmp", filename);
	bool flag = FM.readFile(this->text_program);
	if (flag) {
		syntax_version = WAS_8035;
		m_syntax_slection->SetSelection(1);
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
	if (directory.isOk() && item.IsOk() && item != m_treeCtrl1->GetRootItem()) {
		string file = m_treeCtrl1->GetItemText(item).ToStdString();
		if (wxMessageBox(wxString::Format(wxT("¿Eliminar \"%s\" del control?"), wxString(file)), "Confirmar", wxYES_NO | wxICON_QUESTION) != wxYES) {
			return;
		}
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
		if (!fname.Lower().EndsWith(".pit")) {
			fname += ".pit";
		}
		fname.Replace(wxT(' '), wxT('_'), true);
		sf::Ftp::Response r = ftp.renameFile(m_treeCtrl1->GetItemText(item).ToStdString(), fname.ToStdString());
		if (!r.isOk()) {
			wxMessageBox( "No se pudo renombrar el archivo", "Error", wxICON_ERROR);
		}
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

void MainWindow::openFormSendProgram( wxCommandEvent& event ) {
	if (!ensureTmpDir()) return;
	FileManager tmpFile("tmp", "tmp.pit");
	this->text_program = m_textCtrl->GetValue();
	if (tmpFile.writeFile(this->text_program)) {
		m_statusBar->SetStatusText("Guardado. Abriendo EnvioCNC...",0);
		if (wxExecute("EnvioCNC.exe") == 0) {
			wxMessageBox( wxT("No se pudo ejecutar la aplicación externa EnvioCNC.exe"), wxT("Aplicación no disponible"), wxICON_ERROR);
		}
	}
}

/** Enviar programa al vuelo (sin nombre de archivo de destino) */
void MainWindow::sendProgramOnFly( wxCommandEvent& event ) {
	if (!ensureTmpDir()) return;
	FileManager tmpFile("tmp", "tmp.pit");
	this->text_program = m_textCtrl->GetValue();
	if (tmpFile.writeFile(this->text_program)) {
		m_statusBar->SetStatusText("Programa guardado. Por enviar...",0);
		if (FtpWindow->checkConnection()) {
			FtpWindow->ShowModal();
		}
		this->refreshFtpFileList();
	} else {
		wxMessageBox( wxT("Ocurrió un error al guardar el archivo en forma temporal"), wxT("¡Ocurrió un error!"), wxICON_ERROR);
	}
}



