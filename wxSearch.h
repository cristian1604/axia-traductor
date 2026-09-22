#ifndef WXSEARCH_H
#define WXSEARCH_H
#include "wxSearchBase.h"
#include <wx/stc/stc.h>

class wxSearch : public wxSearchBase {

private:
	wxStyledTextCtrl *editor;
	int pos;  // posición de la última coincidencia (-1 si no hubo)
	bool find_from(int from);
protected:
	void evt_key_up( wxKeyEvent& event ) ;
	void search( wxCommandEvent& event )  override;

public:
	wxSearch(wxWindow *parent=NULL);
	~wxSearch();
	void assignSearchField(wxStyledTextCtrl *x);
	int search_next();
};

#endif
