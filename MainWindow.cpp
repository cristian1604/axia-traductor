#include "MainWindow.h"
#include <wx/filedlg.h>
#include "FileManager.h"
#include <iostream>
using namespace std;

MainWindow::MainWindow(wxWindow *parent) : wxMainWindow(parent) {
	m_textCtrl->SetBackgroundColour(wxColour( 0, 30, 60));
	m_textCtrl->SetFont( wxFont( 12, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxT("Courier New") ) );
	m_textCtrl->SetDefaultStyle(wxTextAttr(*wxYELLOW));
	/*m_textCtrl->AppendText("Yellow text\n");
	m_textCtrl->SetDefaultStyle(wxTextAttr(wxNullColour, *wxLIGHT_GREY));
	m_textCtrl->AppendText("Red on grey text\n");
	m_textCtrl->SetDefaultStyle(wxTextAttr(*wxBLUE));
	m_textCtrl->AppendText("Blue on grey text\n");
	*/
	m_statusBar->SetLabel("Programa iniciado");
	m_statusBar->SetStatusText("8025 a 8035", 1);
	this->title_bar = "Traductor 8025 a 8035";
}

void syntax_color_8025(wxTextCtrl* elem) {
	//elem->Show(false);
	string aux = std::string(elem->GetValue() + '\n');
	elem->SetValue("");
	
	while (!aux.empty()) {
		int pos = aux.find_first_of('\n');
		string line = aux.substr(0, pos);
		aux = aux.substr(pos+1, aux.length());
		
		while (!line.empty()) {
			if (line[0] == ' ') {
				elem->AppendText(' ');
				line = line.substr(1, line.length());
				continue;
			}
			pos = line.find_first_of(' ');
			if (pos < 0)
				pos = line.length();
			string sentence = line.substr(0, pos);
			line = line.substr(pos, line.length());
			switch (sentence[0]) {
			case '%':
				elem->SetDefaultStyle(wxTextAttr(wxColour( 28, 255, 251)));
				//elem->SetStyle(elem->GetInsertionPoint(), elem->GetInsertionPoint() + sentence.length(), wxTextAttr(wxColour( 28, 255, 251)));
				break;
			case 'N':
				elem->SetDefaultStyle(wxTextAttr(*wxGREEN));
				break;
			case '(':
				sentence = sentence + line;
				line = "";
				elem->SetDefaultStyle(wxTextAttr(wxColour(68, 117, 71)));
				break;
			case '#':
				sentence = sentence + line;
				line = "";
				elem->SetDefaultStyle(wxTextAttr(wxColour(224, 13, 13)));
				break;
			default:
				elem->SetDefaultStyle(wxTextAttr(*wxYELLOW));
			}
			elem->AppendText(sentence);
		}
		elem->AppendText('\n');
	}
	//elem->Show(true);
	elem->SetFocus();
}

MainWindow::~MainWindow() {
	
}

/** CARGAR PROGRAMA PARA 8025 DESDE ARCHIVO **/
void MainWindow::loadProgramFromFile( wxCommandEvent& event )  {
	wxFileDialog* OpenDialog = new wxFileDialog(this, wxT("Abrir programa para Fagor 8025"), wxEmptyString, wxEmptyString, wxT("Programa de mecanizado (*.NC, *.PIT)|*.nc;*.pit|Archivo de texto (*.txt)|*.txt|Todos los archivos|*.*"), wxFD_OPEN, wxDefaultPosition);
	// Creates a "open file" dialog with 4 file types
	if (OpenDialog->ShowModal() == wxID_OK) // if the user click "Open" instead of "Cancel"
	{
		this->SetTitle(this->title_bar + " - " + OpenDialog->GetFilename());
		FileManager FM(OpenDialog->GetPath(), OpenDialog->GetFilename());
		bool flag = FM.readFile(this->text_program);
		if (flag) {
			/*m_textCtrl->SetValue("");
			m_textCtrl->SetDefaultStyle(wxTextAttr(*wxYELLOW));
			
			m_textCtrl->SetInsertionPoint(0);*/
			m_textCtrl->SetValue(this->text_program);
			syntax_color_8025(m_textCtrl);
			m_textCtrl->SetInsertionPoint(0);
		}
		// Sets our current document to the file the user selected
		//MainEditBox->LoadFile(CurrentDocPath); //Opens that file
		//SetTitle(wxString("Edit - ") << OpenDialog->GetFilename()); // Set the Title to reflect the file open
	}
	
	// Clean up after ourselves
	OpenDialog->Destroy();
	
}

void MainWindow::edit_text( wxKeyEvent& event )  {
	event.Skip();
}

void MainWindow::update_syntax_highlight( wxCommandEvent& event )  {
	int ip = m_textCtrl->GetInsertionPoint();
	syntax_color_8025(m_textCtrl);
	m_textCtrl->SetInsertionPoint(ip);
}

