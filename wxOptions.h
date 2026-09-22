#ifndef WXOPTIONS_H
#define WXOPTIONS_H
#include "wxfb_project.h"
#include "Settings.h"

// s_Settings y default_settings() viven en Settings.h

class wxOptions : public wxParameters {
	s_Settings settings;
private:
	
protected:
	void remove_m08( wxCommandEvent& event ) ;
	void reset_defaults( wxCommandEvent& event) ;
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
	void showSettings();
	
public:
	wxOptions(wxWindow *parent=NULL);
	~wxOptions();
};

#endif

