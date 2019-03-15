#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "wxfb_project.h"

class MainWindow : public wxMainWindow {
	
private:
	wxString text_program;
	wxString title_bar;
protected:
	void translate( wxCommandEvent& event )  override;
	void update_syntax_highlight( wxCommandEvent& event )  override;
	void edit_text( wxKeyEvent& event )  override;
	void loadProgramFromFile( wxCommandEvent& event )  override;

public:
	MainWindow(wxWindow *parent=NULL);
	~MainWindow();
};

#endif

