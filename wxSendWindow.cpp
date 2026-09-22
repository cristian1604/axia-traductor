#include "wxSendWindow.h"
#include "Sender.h"
#include "FileManager.h"
#include <wx/sizer.h>
#include <wx/msgdlg.h>
#include <wx/utils.h>
#include <wx/settings.h>   // wxSystemSettings: en wxGTK no llega por otros encabezados
#include <wx/dir.h>
#include <wx/filefn.h>

wxSendWindow::wxSendWindow(wxWindow *parent, const std::vector<Machine> &machines_, s_Settings &settings_,
                           const wxString &program_, const wxString &suggestedName, const wxString &preselect)
	: wxDialog(parent, wxID_ANY, wxT("Enviar programa a torno"), wxDefaultPosition, wxDefaultSize,
	           wxDEFAULT_DIALOG_STYLE),
	  machines(machines_), settings(settings_), program(program_), sent(false) {

	wxBoxSizer *root = new wxBoxSizer(wxVERTICAL);
	wxFlexGridSizer *grid = new wxFlexGridSizer(2, 8, 8);
	grid->AddGrowableCol(1, 1);

	grid->Add(new wxStaticText(this, wxID_ANY, wxT("Torno:")), 0, wxALIGN_CENTER_VERTICAL);
	m_machine = new wxChoice(this, wxID_ANY);
	int selected = 0;
	for (size_t i = 0; i < machines.size(); ++i) {
		m_machine->Append(wxString::FromUTF8(machines[i].name.c_str()));
		wxString name = wxString::FromUTF8(machines[i].name.c_str());
		if (name == preselect) selected = (int) i;
		else if (preselect.IsEmpty() && name == settings.last_machine) selected = (int) i;
	}
	m_machine->SetSelection(selected);
	grid->Add(m_machine, 1, wxEXPAND);

	grid->Add(new wxStaticText(this, wxID_ANY, wxT("Nombre en el torno:")), 0, wxALIGN_CENTER_VERTICAL);
	wxString name = suggestedName.IsEmpty() ? settings.last_filename : suggestedName;
	m_name = new wxTextCtrl(this, wxID_ANY, name, wxDefaultPosition, wxSize(260, -1), wxTE_PROCESS_ENTER);
	grid->Add(m_name, 1, wxEXPAND);

	root->Add(grid, 0, wxEXPAND | wxALL, 12);

	m_close = new wxCheckBox(this, wxID_ANY, wxT("Cerrar luego de transferir"));
	m_close->SetValue(settings.close_after_transfer);
	root->Add(m_close, 0, wxLEFT | wxRIGHT, 12);

	m_status = new wxStaticText(this, wxID_ANY, wxT("Se enviará el contenido actual del editor."));
	root->Add(m_status, 0, wxEXPAND | wxALL, 12);

	wxStdDialogButtonSizer *buttons = new wxStdDialogButtonSizer();
	m_send = new wxButton(this, wxID_OK, wxT("Enviar (F2)"));
	buttons->AddButton(m_send);
	buttons->AddButton(new wxButton(this, wxID_CANCEL, wxT("Cerrar")));
	buttons->Realize();
	root->Add(buttons, 0, wxEXPAND | wxALL, 8);

	SetSizerAndFit(root);
	CentreOnParent();

	m_send->Bind(wxEVT_BUTTON, &wxSendWindow::OnSend, this);
	m_name->Bind(wxEVT_TEXT_ENTER, &wxSendWindow::OnSend, this);
	Bind(wxEVT_CHAR_HOOK, &wxSendWindow::OnKeyDown, this);
	m_name->SetFocus();
	m_name->SelectAll();
}

void wxSendWindow::OnKeyDown(wxKeyEvent &event) {
	if (event.GetKeyCode() == WXK_F2) {
		wxCommandEvent e;
		OnSend(e);
	} else if (event.GetKeyCode() == WXK_ESCAPE) {
		EndModal(wxID_CANCEL);
	} else {
		event.Skip();
	}
}

void wxSendWindow::SetStatus(const wxString &text, bool error) {
	m_status->SetForegroundColour(error ? *wxRED : wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
	m_status->SetLabel(text);
	m_status->Wrap(GetClientSize().GetWidth() - 24);
	Layout();
	Fit();
}

void wxSendWindow::OnSend(wxCommandEvent &event) {
	int sel = m_machine->GetSelection();
	if (sel < 0 || sel >= (int) machines.size()) {
		SetStatus(wxT("Seleccione un torno"), true);
		return;
	}
	const Machine &machine = machines[sel];
	std::string remoteName = normalize_remote_name(m_name->GetValue().ToStdString());
	if (remoteName == ".pit") {
		SetStatus(wxT("Ingrese un nombre de archivo"), true);
		m_name->SetFocus();
		return;
	}
	if (program.Trim().IsEmpty()) {
		SetStatus(wxT("No hay programa para enviar"), true);
		return;
	}

	SendResult r;
	{
		wxBusyCursor busy;
		m_send->Enable(false);
		SetStatus(wxT("Transfiriendo a ") + wxString::FromUTF8(machine.name.c_str()) + wxT("..."), false);
		wxYield();

		if (machine.protocol == "fanuc-udp") {
			r = send_fanuc_udp(machine, program.ToStdString());
		} else {
			// El nombre remoto es el del archivo local, por eso se escribe una copia temporal con ese nombre
			if (!wxDir::Exists("tmp") && !wxMkdir("tmp")) {
				r.message = "No se pudo crear el directorio temporal \"tmp\"";
			} else {
				FileManager tmpFile("tmp", remoteName);
				if (!tmpFile.writeFile(program)) {
					r.message = "No se pudo escribir el archivo temporal " + tmpFile.getFullPath();
				} else {
					r = send_ftp(machine, tmpFile.getFullPath());
				}
			}
		}
		m_send->Enable(true);
	}

	SetStatus(wxString::FromUTF8(r.message.c_str()), !r.ok);
	if (!r.ok) return;

	sent = true;
	settings.last_machine = wxString::FromUTF8(machine.name.c_str());
	settings.last_filename = remoteName;
	settings.close_after_transfer = m_close->GetValue();
	FileManager F;
	F.saveSettings(settings);

	if (settings.close_after_transfer) {
		EndModal(wxID_OK);
	}
}
