#ifndef WXSEARCHREPLACE_H
#define WXSEARCHREPLACE_H
#include "wxfb_project.h"
#include <wx/stc/stc.h>

class wxSearchReplace : public search_and_replace {

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
