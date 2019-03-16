#include "MainWindow.h"
#include <wx/filedlg.h>
#include "FileManager.h"
#include "SyntaxColor.h"
#include "Translator.h"
#include "wxOptions.h"
#include "wxSearch.h"
using namespace std;

MainWindow::MainWindow(wxWindow *parent) : wxMainWindow(parent) {
	m_textCtrl->SetBackgroundColour(wxColour( 0, 30, 60));
	m_textCtrl->SetFont( wxFont( 12, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxT("Courier New") ) );
	m_textCtrl->SetDefaultStyle(wxTextAttr(*wxYELLOW));
	m_statusBar->SetLabel("Programa iniciado");
	m_statusBar->SetStatusText("8025 a 8035", 1);
	this->title_bar = "Traductor 8025 a 8035";
}

MainWindow::~MainWindow() {
	
}

/** LOAD PROGRAM FOR 8025 FROM FILE **/
void MainWindow::loadProgramFromFile( wxCommandEvent& event )  {
	wxFileDialog* OpenDialog = new wxFileDialog(this, wxT("Abrir programa para Fagor 8025"), wxEmptyString, wxEmptyString, wxT("Programa de mecanizado (*.NC, *.PIT)|*.nc;*.pit|Archivo de texto (*.txt)|*.txt|Todos los archivos|*.*"), wxFD_OPEN, wxDefaultPosition);
	if (OpenDialog->ShowModal() == wxID_OK) // if the user click "Open" instead of "Cancel"
	{
		this->SetTitle(this->title_bar + " - " + OpenDialog->GetFilename());
		FileManager FM(OpenDialog->GetPath(), OpenDialog->GetFilename());
		bool flag = FM.readFile(this->text_program);
		if (flag) {
			m_textCtrl->SetValue("");
			m_textCtrl->SetValue(this->text_program);
			syntax_highlight(m_textCtrl, 8025);
			m_textCtrl->SetInsertionPoint(0);
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
	switch (syntax_slection->GetSelection()) {
	case 0:
		syntax_version = 8025;
		break;
	case 1:
		syntax_version = 8035;
		break;
	}
	int ip = m_textCtrl->GetInsertionPoint();
	syntax_highlight(m_textCtrl, syntax_version);
	m_textCtrl->SetInsertionPoint(ip);
}

void MainWindow::translate( wxCommandEvent& event )  {
	translate_8025_to_8035(m_textCtrl);
	syntax_highlight(m_textCtrl, 8035);
	m_textCtrl->SetInsertionPoint(0);
	event.Skip();
}

void MainWindow::open_options( wxCommandEvent& event )  {
	wxOptions *opt = new wxOptions(NULL);
	opt->Show();
}

void MainWindow::search_window( wxCommandEvent& event )  {
	wxSearch *srch = new wxSearch(this);
	srch->assignSearchField(m_textCtrl);
	srch->Show();
}

void MainWindow::search_next( wxCommandEvent& event )  {
	event.Skip();
}

