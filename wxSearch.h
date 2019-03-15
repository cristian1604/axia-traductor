#ifndef WXSEARCH_H
#define WXSEARCH_H
#include "wxfb_project.h"
#include <wx/textctrl.h>

class wxSearch : public searchDialog {
	
private:
	wxWindow *win;
	wxTextCtrl *textCtrl;
protected:
	void search2( wxCommandEvent& event )  override;
	void search( wxCommandEvent& event )  override;
	
public:
	wxSearch(wxWindow *parent=NULL);
	~wxSearch();
	void assignSearchField(wxTextCtrl *x);
};

#endif

