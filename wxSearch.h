#ifndef WXSEARCH_H
#define WXSEARCH_H
#include "wxfb_project.h"
#include <wx/textctrl.h>

class wxSearch : public searchDialog {
	
private:
	wxWindow *win;
	wxTextCtrl *textCtrl;
	int pos;  // position of the last coincidence
protected:
	void search( wxCommandEvent& event )  override;
	
public:
	wxSearch(wxWindow *parent=NULL);
	~wxSearch();
	void assignSearchField(wxTextCtrl *x);
	unsigned int search_next();
};

#endif

