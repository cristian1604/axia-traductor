#ifndef WXSEARCHREPLACE_H
#define WXSEARCHREPLACE_H
#include "wxfb_project.h"
#include <wx/textctrl.h>

class wxSearchReplace : public search_and_replace {
	
private:
	wxTextCtrl *textCtrl;
protected:
	void start_replace( wxCommandEvent& event ) ;
	void close( wxCommandEvent& event ) ;
	
public:
	wxSearchReplace(wxWindow *parent=NULL);
	~wxSearchReplace();
	void assignTextField(wxTextCtrl *elem);
	unsigned int replace(wxString source, wxString replacement);
};

#endif

