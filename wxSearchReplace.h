#ifndef WXSEARCHREPLACE_H
#define WXSEARCHREPLACE_H
#include "wxSearchReplaceBase.h"
#include <wx/stc/stc.h>

class wxSearchReplace : public wxSearchReplaceBase {

private:
	wxStyledTextCtrl *editor;
protected:
	void evt_key_up( wxKeyEvent& event ) ;
	void start_replace( wxCommandEvent& event ) ;
	void close( wxCommandEvent& event ) ;

public:
	wxSearchReplace(wxWindow *parent=NULL);
	~wxSearchReplace();
	void assignTextField(wxStyledTextCtrl *elem);
	unsigned int replace(wxString source, wxString replacement);
};

#endif
