#ifndef WXOPTIONS_H
#define WXOPTIONS_H
#include "wxfb_project.h"

struct s_Settings {
	wxColour colour_textCtrl;
	wxColour colour_text;
	wxColour colour_comments;
	wxColour colour_command_m;
	wxColour colour_command_tool;
	wxColour colour_line_number;
	
	bool maximize_on_startup;
	bool remove_m08;
	char replace_from[10];
	char replace_to[10];
};

class wxOptions : public wxParameters {
	s_Settings settings;
private:
	
protected:
	void remove_m08( wxCommandEvent& event ) ;
	void reset_defaults( wxCommandEvent& event ) ;
	void reset( wxCommandEvent& event ) ;
	void maximize( wxCommandEvent& event ) ;
	void colour_text( wxColourPickerEvent& event ) ;
	void colour_m( wxColourPickerEvent& event ) ;
	void colour_tool( wxColourPickerEvent& event ) ;
	void colour_comments( wxColourPickerEvent& event ) ;
	void colour_line( wxColourPickerEvent& event ) ;
	void colour_textCtrl( wxColourPickerEvent& event ) ;
	void save( wxCommandEvent& event ) ;
	void evt_key_up( wxKeyEvent& event ) ;
	
public:
	wxOptions(wxWindow *parent=NULL);
	~wxOptions();
};

#endif

