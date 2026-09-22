#include "wxMainWindow.h"
#include <wx/filedlg.h>
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
#include "Renumber.h"
#include "AppPaths.h"
#include "wxSendWindow.h"
#include <wx/iconbndl.h>
#include <wx/display.h>
#include <algorithm>
using namespace std;

// Icono de 16x16 para el árbol del explorador FTP a partir de un XPM. Los XPM
// de icons.xpm tienen tamaños distintos (pit_extension_xpm es de 128x128) y
// wxWidgets 3.2 ya no los adapta al tamaño de la wxImageList: sin reescalar,
// los archivos aparecían como cuadrados negros.
static wxBitmap tree_icon(const char *const *xpm, int size) {
	wxImage img(xpm);
	if (img.GetWidth() != size || img.GetHeight() != size) {
		img.Rescale(size, size, wxIMAGE_QUALITY_HIGH);
	}
	return wxBitmap(img);
}

// Con DPI por monitor (manifest.xml), wxWidgets no agranda los mapas de bits
// de un solo tamaño: al 125 % o 150 % los iconos quedaban chicos. Se reescala
// cada imagen al factor de DPI de la ventana (1.25, 1.5, 2...). Un
// wxBitmapBundle con copias 1x y 2x no sirve: en escalas intermedias wx
// prefiere dibujar la 1x sin escalar.
static wxBitmap hidpi_bitmap(const wxBitmap &bmp, double scale) {
	if (!bmp.IsOk() || scale <= 1.01) return bmp;
	wxImage img = bmp.ConvertToImage();
	img.Rescale((int) (img.GetWidth() * scale + 0.5), (int) (img.GetHeight() * scale + 0.5), wxIMAGE_QUALITY_HIGH);
	return wxBitmap(img);
}

static void scale_toolbar(wxToolBar *tb) {
	double scale = tb->GetDPIScaleFactor();
	if (scale <= 1.01) return;
	for (size_t i = 0; i < tb->GetToolsCount(); ++i) {
		wxToolBarToolBase *t = tb->GetToolByPos((int) i);
		if (!t || !t->IsButton()) continue;
		tb->SetToolNormalBitmap(t->GetId(), hidpi_bitmap(t->GetNormalBitmap(), scale));
	}
	tb->Realize();
}

static void scale_menu(wxMenu *menu, double scale) {
	wxMenuItemList &items = menu->GetMenuItems();
	for (wxMenuItemList::iterator it = items.begin(); it != items.end(); ++it) {
		wxMenuItem *item = *it;
		if (item->GetSubMenu()) scale_menu(item->GetSubMenu(), scale);
		if (item->GetBitmap().IsOk()) item->SetBitmap(hidpi_bitmap(item->GetBitmap(), scale));
	}
}

wxMainWindow::wxMainWindow(wxWindow *parent) : wxMainWindowBase(parent),
	syntax_version(FAGOR_8025), srch(NULL) {
	m_statusBar->SetLabel("Programa iniciado");
	m_statusBar->SetStatusText("8025 -> 8035 / 8037 / FANUC", 1);
	m_statusBar->SetStatusText("AXIA", 2);
	// Icono de la ventana (barra de título y Alt+Tab): el mismo archivo que el
	// icono del ejecutable, con todos sus tamaños. Sin esto wxWidgets muestra
	// su icono genérico.
	wxIconBundle icons(wxT("resources/Webalys.ico"), wxBITMAP_TYPE_ICO);
	if (icons.IsOk()) SetIcons(icons);
	this->window_title = wxT("Traductor código CNC 8025 a 8035 / 8037 / FANUC");

	//search window
	srch = new wxSearch(this);
	srch->assignSearchField(m_editor);
	loadSettings();
	m_editor->SetStandard(syntax_version);

	// Cambios sin guardar: Scintilla avisa al salir del punto de guardado y al volver a él
	m_editor->Bind(wxEVT_STC_SAVEPOINTLEFT, &wxMainWindow::onModifiedChanged, this);
	m_editor->Bind(wxEVT_STC_SAVEPOINTREACHED, &wxMainWindow::onModifiedChanged, this);
	Bind(wxEVT_CLOSE_WINDOW, &wxMainWindow::onClose, this);

	// La aplicación es consciente del DPI (manifest.xml) pero wxWidgets en
	// Windows no escala los tamaños fijos del diseño: se convierten con FromDIP.
	// El tamaño se acota al área visible del monitor (netbooks, escalado alto)
	// y la ventana se centra: con la posición por defecto la barra de estado
	// podía quedar debajo de la barra de tareas.
	wxSize size = FromDIP(wxSize(1270, 730));
	wxRect area = wxDisplay(this).GetClientArea();
	size.x = std::min(size.x, area.width);
	size.y = std::min(size.y, area.height);
	SetSize(size);
	Centre(wxBOTH);
	m_splitter1->Bind(wxEVT_IDLE, &wxMainWindow::splitterFirstIdle, this);
	// Iconos de barras, menús y logo al DPI del monitor
	double scale = GetDPIScaleFactor();
	scale_toolbar(m_toolBar1);
	scale_toolbar(m_toolBar2);
	for (size_t i = 0; i < m_menubar1->GetMenuCount(); ++i) scale_menu(m_menubar1->GetMenu(i), scale);
	m_bitmap1->SetBitmap(hidpi_bitmap(m_bitmap1->GetBitmap(), scale));
	m_bitmap1->SetMinSize(wxSize(-1, FromDIP(42)));
	Layout();

	// Lista de tornos compartida (se crea con valores por defecto si no existe)
	std::string error;
	machines = load_machines(shared_config_file("machines.json").ToStdString(), &error);
	if (!error.empty()) {
		wxMessageBox(wxString::FromUTF8(error.c_str()) + wxT("\n\nSe usa la lista de tornos por defecto."),
		             wxT("Lista de tornos"), wxICON_WARNING);
	}
}

wxMainWindow::~wxMainWindow() {
	ftp.disconnect();
}

void wxMainWindow::splitterFirstIdle(wxIdleEvent &event) {
	m_splitter1->SetSashPosition(FromDIP(242));
	m_splitter1->Unbind(wxEVT_IDLE, &wxMainWindow::splitterFirstIdle, this);
}

void wxMainWindow::showProgram(const wxString &program, int standard) {
	syntax_version = standard;
	m_syntax_slection->SetSelection(standard == FAGOR_8025 ? 0 : 1);
	m_editor->SetStandard(standard);
	m_editor->SetProgram(program, true);
}

/** LOAD PROGRAM FOR 8025 FROM FILE **/
void wxMainWindow::loadProgramFromFile( wxCommandEvent& event )  {
	if (!confirmDiscard(wxT("abrir otro programa"))) return;
	wxFileDialog OpenDialog(this, wxT("Abrir programa para Fagor 8025"), wxEmptyString, wxEmptyString, wxT("Programa de mecanizado (*.NC, *.PIT)|*.NC;*.PIT;*.nc;*.pit|Archivo de texto (*.txt, *.TXT)|*.txt|Todos los archivos|*.*"), wxFD_OPEN, wxDefaultPosition);
	if (OpenDialog.ShowModal() == wxID_OK) // if the user click "Open" instead of "Cancel"
	{
		path = OpenDialog.GetPath();
		filename = OpenDialog.GetFilename();
		updateTitle();
		// El archivo original 8025 no se usa como destino de "Guardar": se
		// pide un nombre nuevo para no pisar el original con la traduccion.
		FM = FileManager();
		FileManager source(path);
		bool flag = source.readFile(this->text_program);
		if (flag) {
			m_statusBar->SetStatusText("Leyendo archivo...", 0);
			showProgram(this->text_program, FAGOR_8025);
			m_statusBar->SetStatusText("Archivo cargado: " + filename, 0);
		}
	}
}

void wxMainWindow::update_syntax_highlight( wxCommandEvent& event )  {
	// Solo el selector cambia la sintaxis; con F11 (menú o barra) se repinta la
	// actual, que puede ser FANUC, ausente del selector.
	if (event.GetEventType() == wxEVT_CHOICE) {
		switch (m_syntax_slection->GetSelection()) {
		case 0:
			syntax_version = FAGOR_8025;
			break;
		case 1:
			syntax_version = WAS_8035;
			break;
		}
	}
	m_editor->SetStandard(syntax_version);
	m_editor->SetFocus();
}

/**  TRANSLATION  **/
void wxMainWindow::translate( wxCommandEvent& event )  {
	if (syntax_version == WAS_8035) {
		wxMessageBox(wxT("El código ya se encuentra en la versión 8035"),
					 "Traducir a 8025",
					 wxOK);
		return;
	}
	m_statusBar->SetStatusText("Analizando...", 0);
	int ip = m_editor->GetCurrentPos();
	m_syntax_slection->SetSelection(1);
	syntax_version = WAS_8035;
	translate_8025_to_8035(m_editor);
	m_editor->SetStandard(syntax_version);
	m_editor->GotoPos(ip);
	enum_lines(event);
	m_statusBar->SetStatusText(wxT("Programa convertido a versión 8035"), 0);
}

void wxMainWindow::translateFanuc( wxCommandEvent& event )  {
	if (syntax_version == KIA_FANUC) {
		wxMessageBox(wxT("El código ya se encuentra en la versión FANUC"),
					 "Traducir a FANUC",
					 wxOK);
		return;
	}
	m_statusBar->SetStatusText("Analizando...", 0);
	int ip = m_editor->GetCurrentPos();
	m_syntax_slection->SetSelection(1);
	syntax_version = KIA_FANUC;
	translate_8025_to_Fanuc(m_editor);
	m_editor->SetStandard(syntax_version);
	m_editor->GotoPos(ip);
	enum_lines(event);
	m_statusBar->SetStatusText(wxT("Programa convertido a versión Fanuc"), 0);
}

void wxMainWindow::open_options( wxCommandEvent& event )  {
	wxOptions opt(this);
	opt.ShowModal();
	loadSettings();
}

void wxMainWindow::search_window( wxCommandEvent& event )  {
	srch->ShowModal();
}

void wxMainWindow::search_next( wxCommandEvent& event )  {
	srch->search_next();
}

void wxMainWindow::search_replace_window( wxCommandEvent& event )  {
	wxSearchReplace snr(this);
	snr.assignTextField(m_editor);
	snr.ShowModal();
}

/** SAVE PROGRAM GENERATED **/
void wxMainWindow::save_program( wxCommandEvent& event )  {
	saveProgram();
}

// Guarda el programa (pidiendo nombre si hace falta) y lo sube si hay un torno
// conectado. Devuelve true si el archivo local quedó guardado.
bool wxMainWindow::saveProgram() {
	if (!FM.isDefined()) {
		wxString base = filename.BeforeLast('.');
		if (base.IsEmpty()) base = filename;
		wxString suggested = base + "_35";
		wxFileDialog SaveDialog(this, wxT("Guardar programa"), wxEmptyString, suggested, wxT("Programa de mecanizado PIT (*.PIT)|*.PIT|Programa de mecanizado NC (*.NC)|*.NC|Archivo de texto (*.txt)|*.txt|Todos los archivos|*.*"), wxFD_SAVE | wxFD_OVERWRITE_PROMPT, wxDefaultPosition);
		if (SaveDialog.ShowModal() != wxID_OK) {
			return false;   // cancelado: no guardar ni enviar nada
		}
		path = SaveDialog.GetPath();
		filename = SaveDialog.GetFilename();
		// Se asigna al miembro (antes se declaraba un FM local que lo ocultaba,
		// por lo que cada "Guardar" volvia a pedir el nombre)
		FM = FileManager(path);
		updateTitle();
	}

	this->text_program = m_editor->GetText();
	if (!FM.writeFile(this->text_program)) {
		wxMessageBox( "No se pudo guardar el archivo:\n" + FM.getFullPath(), "Error al guardar", wxICON_ERROR);
		return false;
	}
	m_editor->SetSavePoint();
	m_statusBar->SetStatusText("Guardado: " + filename, 0);

	// Si hay un control conectado, ademas se envia el programa por FTP
	sf::Ftp::DirectoryResponse directory = ftp.getWorkingDirectory();
	if (directory.isOk()) {
		ftp.keepAlive();
		// El nombre remoto es el del archivo local: copia temporal con ese nombre
		FileManager tmpFile(temp_dir(), filename);
		if (!tmpFile.writeFile(this->text_program)) {
			wxMessageBox( "No se pudo escribir el archivo temporal:\n" + tmpFile.getFullPath(), "Error de transferencia", wxICON_ERROR);
			return true;
		}

		ftp.deleteFile(tmpFile.getFilename());
		sf::Ftp::Response response = ftp.upload(tmpFile.getFullPath(), "", sf::Ftp::Binary);
		if (response.isOk()) {
			// Primero el refresco: si no, su propio mensaje de estado pisa este
			refreshFtpFileList();
			m_statusBar->SetStatusText("Guardado y transferido al control como " + filename, 0);
		} else {
			wxMessageBox( wxT("Se guardó localmente pero no se pudo transferir al control"), "Error de transferencia", wxICON_ERROR);
		}
	}
	return true;
}

void wxMainWindow::updateTitle() {
	wxString title = window_title;
	if (!filename.IsEmpty()) title += " - " + filename;
	if (m_editor->GetModify()) title += wxT(" *");
	SetTitle(title);
}

void wxMainWindow::onModifiedChanged(wxStyledTextEvent &event) {
	updateTitle();
	event.Skip();
}

// true si se puede seguir: no hay cambios, se guardaron, o el usuario los descarta
bool wxMainWindow::confirmDiscard(const wxString &action) {
	if (!m_editor->GetModify()) return true;
	wxMessageDialog dlg(this,
		wxT("El programa tiene cambios sin guardar.\n\n¿Querés guardarlos antes de ") + action + wxT("?"),
		wxT("Cambios sin guardar"), wxYES_NO | wxCANCEL | wxICON_WARNING);
	dlg.SetYesNoCancelLabels(wxT("Guardar"), wxT("No guardar"), wxT("Cancelar"));
	switch (dlg.ShowModal()) {
	case wxID_YES: return saveProgram();
	case wxID_NO:  return true;
	default:       return false;
	}
}

void wxMainWindow::onClose(wxCloseEvent &event) {
	if (event.CanVeto() && !confirmDiscard(wxT("cerrar"))) {
		event.Veto();
		return;
	}
	event.Skip();   // destrucción normal de la ventana
}

bool wxMainWindow::readClipboardText(wxString &out) {
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

bool wxMainWindow::writeClipboardText(const wxString &text) {
	if (!wxTheClipboard->Open()) return false;
	// The data object is owned by the clipboard, do not delete it
	wxTheClipboard->SetData( new wxTextDataObject(text));
	wxTheClipboard->Close();
	return true;
}

void wxMainWindow::about( wxCommandEvent& event )  {
	wxAbout a(this);
	a.ShowModal();
}

/**  Dady's re-enumerator lines library call **/
void wxMainWindow::enum_lines( wxCommandEvent& event )  {
	int pos = m_editor->GetCurrentPos();
	std::string original = m_editor->GetText().ToStdString();

	// Un programa FANUC ya reenumerado empieza con "Onnnn" en lugar de "%": se
	// repone el "%" para que el reenumerador encuentre el inicio (y abajo se
	// vuelve a poner la "O").
	if (syntax_version == KIA_FANUC && original.find('%') == std::string::npos) {
		size_t o = original.find_first_not_of(" \t\r\n");
		if (o != std::string::npos && original[o] == 'O') original[o] = '%';
	}

	// En 8035 la traducción inserta comentarios entre el '%' y N0010 que no
	// deben numerarse: se reenumera solo desde N0010.
	RenumberResult r = (syntax_version == WAS_8035)
		? renumber_program_from(original, "N0010")
		: renumber_program(original);
	if (!r.ok) {
		wxString msg = wxString::FromUTF8(r.error.c_str());
		if (!r.undefined.empty()) {
			msg += "\n\nDestinos no encontrados:";
			for (size_t i = 0; i < r.undefined.size(); ++i) {
				msg += wxString::Format("\n  N%04d", r.undefined[i]);
			}
		}
		wxMessageBox( msg, "No se pudo reenumerar", wxICON_ERROR);
		return;
	}

	wxString renumbered = wxString::FromUTF8(r.text.c_str());
	if (renumbered.IsEmpty()) renumbered = r.text;   // texto no UTF-8 (Latin-1)

	// FANUC identifica el programa con "O" en lugar de "%"
	if (syntax_version == KIA_FANUC) {
		int p = renumbered.Find(wxT('%'));
		if (p >= 0) renumbered[p] = 'O';
	}

	// SetText queda en el historial: la reenumeración se puede deshacer con Ctrl+Z
	m_editor->SetText(renumbered);
	m_editor->GotoPos(pos);
	m_editor->SetFocus();
}

void wxMainWindow::loadSettings() {
	FileManager F;
	F.loadSettings(settings);   // sin archivo quedan los valores por defecto
	m_editor->ApplySettings(settings);
	if (settings.maximize_on_startup) {
		this->Maximize(true);
	}
}

void wxMainWindow::copy_program_clipboard( wxCommandEvent& event )  {
	writeClipboardText(m_editor->GetText());
}

void wxMainWindow::paste_program_clipboard( wxCommandEvent& event )  {
	if (!confirmDiscard(wxT("reemplazar el programa"))) return;
	if (readClipboardText(text_program)) {
		m_editor->SetText(text_program);
		m_editor->SetFocus();
	}
}

/** Pega el portapapeles en el cursor cambiando la coma decimal por punto */
void wxMainWindow::paste_formatting( wxCommandEvent& event )  {
	wxString aux;
	if (!readClipboardText(aux)) return;
	aux.Replace(wxT(','), wxT('.'));
	// Se inserta en la posición del cursor (o reemplaza la selección, si la hay)
	m_editor->ReplaceSelection(aux);
	m_editor->SetFocus();
}

void wxMainWindow::channels( wxCommandEvent& event )  {
	// Execution of external program
	// Not included on this repository due copyright restrictions
	if (syntax_version != FAGOR_8025) {
		wxMessageBox( wxT("Solo puede simular programas de 8025.\nEl código G actual es 8035"), wxT("Versión G no compatible"), wxICON_ERROR);
		return;
	}
	wxExecute("Canalesw.exe");
}

void wxMainWindow::simulate( wxCommandEvent& event )  {
	// Execution of external program
	// Not included on this repository due copyright restrictions
	if (syntax_version != FAGOR_8025) {
		wxMessageBox( wxT("Solo puede simular programas de 8025.\nEl código G actual es 8035"), wxT("Versión G no compatible"), wxICON_ERROR);
		return;
	}
	wxString tmp = temp_file(wxT("tmp.txt"));
	FileManager F(tmp);
	text_program = m_editor->GetText();
	if (F.writeFile(text_program)) {
		wxExecute(wxT("ABsim.exe \"") + tmp + wxT("\""));
	}
}

///**  FTP OPTIONS  ** ///

void wxMainWindow::connectFTP( const std::string &name )  {
	ftp.disconnect();
	connected_machine.Clear();
	m_treeCtrl1->DeleteAllItems();

	const Machine *m = find_machine(machines, name);
	if (!m) {
		wxMessageBox(wxT("El torno \"") + wxString::FromUTF8(name.c_str()) + wxT("\" no está en machines.json"), wxT("Torno desconocido"), wxICON_ERROR);
		return;
	}
	sf::Ftp::Response r = ftp.connect(sf::IpAddress(m->ip), (unsigned short) m->port, sf::seconds(5));
	if (r.isOk()) r = ftp.login();
	if (r.isOk() && !m->directory.empty()) r = ftp.changeDirectory(m->directory);
	if (!r.isOk()) {
		m_statusBar->SetStatusText(wxT("No se pudo conectar"), 0);
		wxMessageBox(wxString::FromUTF8(m->name.c_str()) + wxT(" (") + m->ip + wxT("): ") + wxString::FromUTF8(r.getMessage().c_str()),
		             wxT("Error de conexión"), wxICON_ERROR);
		return;
	}
	connected_machine = wxString::FromUTF8(m->name.c_str());
	m_statusBar->SetStatusText("Conectado a " + connected_machine, 1);
	ftp.keepAlive();
	refreshFtpFileList();
}

/** Double click on FTP file**/
void wxMainWindow::openFtpFile( wxMouseEvent& event)  {
	wxTreeItemId item = m_treeCtrl1->GetSelection();
	if (!item.IsOk() || item == m_treeCtrl1->GetRootItem()) return;
	if (!confirmDiscard(wxT("abrir el programa del torno"))) return;
	filename = m_treeCtrl1->GetItemText(item);

	m_statusBar->SetStatusText("Descargando " + filename, 0);

	wxString dir = temp_dir();
	sf::Ftp::Response r = ftp.download(filename.ToStdString(), dir.ToStdString(), sf::Ftp::Binary);
	if (!r.isOk()) {
		wxMessageBox( wxString::Format(wxT("No se pudo descargar el archivo (código %d)"), (int) r.getStatus()), "Error de descarga", wxICON_ERROR);
		m_statusBar->SetStatusText("Error al descargar " + filename, 0);
		return;
	}

	updateTitle();
	FM = FileManager(dir, filename);
	bool flag = FM.readFile(this->text_program);
	if (flag) {
		m_statusBar->SetStatusText("Leyendo archivo...", 0);
		showProgram(this->text_program, WAS_8035);
		m_statusBar->SetStatusText("Archivo abierto del torno: " + filename, 0);
	}
}

void wxMainWindow::FtpDisconnect( wxCommandEvent& event )  {
	ftp.disconnect();
	connected_machine.Clear();
	m_treeCtrl1->DeleteAllItems();
	m_statusBar->SetStatusText("Local. 8025 -> 8035", 1);
	m_statusBar->SetStatusText("Desconectado", 0);
}

/** Botón "Conectar a CNC": menú con los tornos FTP de machines.json. Los FANUC
    (UDP) no tienen explorador de archivos. Antes el menú venía fijo del diseño. */
void wxMainWindow::connectFtpMenu( wxCommandEvent& event )  {
	wxMenu menu;
	wxBitmap icon = tree_icon(server_xpm, FromDIP(16));
	for (size_t i = 0; i < machines.size(); ++i) {
		if (machines[i].protocol != "ftp") continue;
		wxMenuItem *item = new wxMenuItem(&menu, wxID_ANY, wxString::FromUTF8(machines[i].name.c_str()));
		item->SetBitmap(icon);
		menu.Append(item);
		std::string name = machines[i].name;
		menu.Bind(wxEVT_MENU, [this, name](wxCommandEvent &) { connectFTP(name); }, item->GetId());
	}
	if (menu.GetMenuItemCount() == 0) {
		wxMessageBox(wxT("No hay tornos con FTP en machines.json"), wxT("Conectar a CNC"), wxICON_WARNING);
		return;
	}
	PopupMenu(&menu);
}

void wxMainWindow::deleteFtpFile( wxCommandEvent& event )  {
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

void wxMainWindow::RenameFtpFile( wxCommandEvent& event )  {
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

void wxMainWindow::ftpFileOptions( wxTreeEvent& event )  {
	event.Skip();
}

void wxMainWindow::FtpRefresh( wxCommandEvent& event )  {
	refreshFtpFileList();
}

void wxMainWindow::refreshFtpFileList() {
	sf::Ftp::DirectoryResponse directory = ftp.getWorkingDirectory();
	if (!directory.isOk()) {
		m_statusBar->SetLabel("FTP no conectado");
		return;
	}

	m_treeCtrl1->DeleteAllItems();
	sf::Ftp::ListingResponse response = ftp.getDirectoryListing();
	wxTreeItemId raiz;
	if (response.isOk()) {
		const std::vector<std::string>& listing = response.getListing();
		raiz = m_treeCtrl1->AddRoot(connected_machine, 1);
		for (std::vector<std::string>::const_iterator it = listing.begin(); it != listing.end(); ++it) {
			m_treeCtrl1->AppendItem(raiz, *it, 2);
		}

		m_treeCtrl1->SortChildren(raiz);
		m_treeCtrl1->Expand(raiz);

		int sz = FromDIP(16);
		wxImageList* imageList = new wxImageList(sz, sz);
		imageList->Add(tree_icon(folder_xpm, sz));						// 0
		imageList->Add(tree_icon(server_xpm, sz));						// 1
		imageList->Add(tree_icon(pit_extension_xpm, sz));				// 2
		m_treeCtrl1->AssignImageList(imageList);

		ftp.keepAlive();
		//m_statusBar->SetStatusText("Conectado a " + aux, 1);
		m_statusBar->SetStatusText("Directorio listado correctamente", 0);
	}
}

void wxMainWindow::checkUpdates( wxCommandEvent& event )  {
	wxActualizaciones* w = new wxActualizaciones(this);
	w->CenterOnParent();
	w->Show(true);
}

/** Enviar programa a torno (F2): diálogo interno, antes EnvioCNC.exe */
void wxMainWindow::openFormSendProgram( wxCommandEvent& event ) {
	openSendDialog();
}

void wxMainWindow::openSendDialog() {
	wxString suggested = filename.BeforeLast('.');
	if (suggested.IsEmpty()) suggested = filename;
	wxSendWindow dlg(this, machines, settings, m_editor->GetText(), suggested, connected_machine);
	dlg.ShowModal();
	if (dlg.sent) {
		m_editor->SetSavePoint();   // ya está en el torno: no hace falta avisar al cerrar
		m_statusBar->SetStatusText(wxT("Programa enviado a ") + settings.last_machine, 0);
		if (!connected_machine.IsEmpty()) refreshFtpFileList();
	}
}

/** Botón "Enviar el programa al CNC conectado": mismo diálogo, con el torno conectado preseleccionado */
void wxMainWindow::sendProgramOnFly( wxCommandEvent& event ) {
	openSendDialog();
}
