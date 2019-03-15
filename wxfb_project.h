///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Dec 21 2016)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __WXFB_PROJECT_H__
#define __WXFB_PROJECT_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/statbmp.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <wx/statusbr.h>
#include <wx/toolbar.h>
#include <wx/menu.h>
#include <wx/frame.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class wxMainWindow
///////////////////////////////////////////////////////////////////////////////
class wxMainWindow : public wxFrame 
{
	private:
	
	protected:
		wxStaticBitmap* m_bitmap1;
		wxTextCtrl* m_textCtrl;
		wxStatusBar* m_statusBar;
		wxToolBar* m_toolBar1;
		wxToolBarToolBase* m_tool1; 
		wxToolBarToolBase* m_tool2; 
		wxToolBarToolBase* m_tool3; 
		wxMenuBar* m_menubar1;
		wxMenu* m_menu1;
		
		// Virtual event handlers, overide them in your derived class
		virtual void edit_text( wxKeyEvent& event ) { event.Skip(); }
		virtual void loadProgramFromFile( wxCommandEvent& event ) { event.Skip(); }
		virtual void update_syntax_highlight( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		wxMainWindow( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Traductor 8025 -> 8035"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 964,729 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
		
		~wxMainWindow();
	
};

#endif //__WXFB_PROJECT_H__
