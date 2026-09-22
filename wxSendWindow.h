#ifndef WXSENDWINDOW_H
#define WXSENDWINDOW_H
#include <wx/dialog.h>
#include <wx/choice.h>
#include <wx/textctrl.h>
#include <wx/checkbox.h>
#include <wx/stattext.h>
#include <wx/button.h>
#include <vector>
#include "Machines.h"
#include "Settings.h"

/**
	Diálogo "Enviar programa a torno": elige el torno de la lista compartida,
	el nombre de destino y envía por FTP (Fagor) o UDP (FANUC) sin necesidad
	de una conexión previa. Recuerda el último torno y nombre por usuario.
**/
class wxSendWindow : public wxDialog {
private:
	const std::vector<Machine> &machines;
	s_Settings &settings;
	wxString program;

	wxChoice *m_machine;
	wxTextCtrl *m_name;
	wxCheckBox *m_close;
	wxStaticText *m_status;
	wxButton *m_send;

	void OnSend(wxCommandEvent &event);
	void OnKeyDown(wxKeyEvent &event);
	void SetStatus(const wxString &text, bool error);
public:
	wxSendWindow(wxWindow *parent, const std::vector<Machine> &machines, s_Settings &settings,
	             const wxString &program, const wxString &suggestedName, const wxString &preselect);
	bool sent;   // true si se transfirió al menos una vez
};

#endif
