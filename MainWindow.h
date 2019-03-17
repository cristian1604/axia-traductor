#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "wxfb_project.h"

class MainWindow : public wxMainWindow {
	
private:
	wxString text_program;
	wxString title_bar;
	int syntax_version;
	bool is_loading;
protected:
	void search_replace_window( wxCommandEvent& event ) ;
	void search_next( wxCommandEvent& event )  override;
	void search_window( wxCommandEvent& event )  override;
	void open_options( wxCommandEvent& event )  override;
	void translate( wxCommandEvent& event )  override;
	void update_syntax_highlight( wxCommandEvent& event )  override;
	void edit_text( wxKeyEvent& event )  override;
	void loadProgramFromFile( wxCommandEvent& event )  override;

public:
	MainWindow(wxWindow *parent=NULL);
	~MainWindow();
};

#endif

