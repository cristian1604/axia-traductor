///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 4.2.1-0-g80c4cb6)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/choice.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/statbmp.h>
#include <wx/sizer.h>
#include <wx/toolbar.h>
#include <wx/treectrl.h>
#include <wx/menu.h>
#include <wx/panel.h>
#include <wx/textctrl.h>
#include <wx/splitter.h>
#include <wx/statusbr.h>
#include <wx/frame.h>
#include <wx/stattext.h>
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/statline.h>
#include <wx/gauge.h>
#include <wx/checkbox.h>
#include <wx/statbox.h>
#include <wx/clrpicker.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class wxMainWindow
///////////////////////////////////////////////////////////////////////////////
class wxMainWindow : public wxFrame
{
	private:

	protected:
		wxChoice* m_syntax_slection;
		wxStaticBitmap* m_bitmap1;
		wxSplitterWindow* m_splitter1;
		wxPanel* m_panel1;
		wxToolBar* m_toolBar2;
		wxToolBarToolBase* m_tool6;
		wxToolBarToolBase* ftp_desconectar;
		wxToolBarToolBase* m_tool10;
		wxToolBarToolBase* ftp_eliminar;
		wxToolBarToolBase* ftp_renombrar;
		wxToolBarToolBase* ftp_enviar;
		wxTreeCtrl* m_treeCtrl1;
		wxMenu* ftpOptions;
		wxPanel* m_panel2;
		wxTextCtrl* m_textCtrl;
		wxStatusBar* m_statusBar;
		wxToolBar* m_toolBar1;
		wxToolBarToolBase* m_tool1;
		wxToolBarToolBase* m_tool2;
		wxToolBarToolBase* m_tool3;
		wxToolBarToolBase* m_tool4;
		wxToolBarToolBase* m_tool5;
		wxToolBarToolBase* ftp_enviar1;
		wxMenuBar* m_menubar1;
		wxMenu* m_menu1;
		wxMenu* m_menu3;
		wxMenu* m_menu2;
		wxMenu* m_menu5;

		// Virtual event handlers, override them in your derived class
		virtual void update_syntax_highlight( wxCommandEvent& event ) { event.Skip(); }
		virtual void connectFtpMenu( wxCommandEvent& event ) { event.Skip(); }
		virtual void FtpDisconnect( wxCommandEvent& event ) { event.Skip(); }
		virtual void FtpRefresh( wxCommandEvent& event ) { event.Skip(); }
		virtual void deleteFtpFile( wxCommandEvent& event ) { event.Skip(); }
		virtual void RenameFtpFile( wxCommandEvent& event ) { event.Skip(); }
		virtual void sendProgramOnFly( wxCommandEvent& event ) { event.Skip(); }
		virtual void openFtpFile( wxMouseEvent& event ) { event.Skip(); }
		virtual void ftpFileOptions( wxTreeEvent& event ) { event.Skip(); }
		virtual void FtpConnectWas8037( wxCommandEvent& event ) { event.Skip(); }
		virtual void FtpConnectWas8035( wxCommandEvent& event ) { event.Skip(); }
		virtual void FtpConnectTaki8037( wxCommandEvent& event ) { event.Skip(); }
		virtual void edit_text( wxKeyEvent& event ) { event.Skip(); }
		virtual void loadProgramFromFile( wxCommandEvent& event ) { event.Skip(); }
		virtual void save_program( wxCommandEvent& event ) { event.Skip(); }
		virtual void translate( wxCommandEvent& event ) { event.Skip(); }
		virtual void translateFanuc( wxCommandEvent& event ) { event.Skip(); }
		virtual void paste_formatting( wxCommandEvent& event ) { event.Skip(); }
		virtual void copy_program_clipboard( wxCommandEvent& event ) { event.Skip(); }
		virtual void paste_program_clipboard( wxCommandEvent& event ) { event.Skip(); }
		virtual void search_window( wxCommandEvent& event ) { event.Skip(); }
		virtual void search_next( wxCommandEvent& event ) { event.Skip(); }
		virtual void search_replace_window( wxCommandEvent& event ) { event.Skip(); }
		virtual void enum_lines( wxCommandEvent& event ) { event.Skip(); }
		virtual void open_options( wxCommandEvent& event ) { event.Skip(); }
		virtual void simulate( wxCommandEvent& event ) { event.Skip(); }
		virtual void channels( wxCommandEvent& event ) { event.Skip(); }
		virtual void openFormSendProgram( wxCommandEvent& event ) { event.Skip(); }
		virtual void about( wxCommandEvent& event ) { event.Skip(); }
		virtual void checkUpdates( wxCommandEvent& event ) { event.Skip(); }


	public:

		wxMainWindow( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Traductor código CNC 8025 a 8035 / 8037 / FANUC"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 1270,730 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );

		~wxMainWindow();

		void m_splitter1OnIdle( wxIdleEvent& )
		{
			m_splitter1->SetSashPosition( 198 );
			m_splitter1->Disconnect( wxEVT_IDLE, wxIdleEventHandler( wxMainWindow::m_splitter1OnIdle ), NULL, this );
		}

		void m_panel1OnContextMenu( wxMouseEvent &event )
		{
			m_panel1->PopupMenu( ftpOptions, event.GetPosition() );
		}

};

///////////////////////////////////////////////////////////////////////////////
/// Class saveFtpWindow
///////////////////////////////////////////////////////////////////////////////
class saveFtpWindow : public wxDialog
{
	private:

	protected:
		wxStaticText* m_staticText5;
		wxTextCtrl* save_filename;
		wxButton* m_button2;

		// Virtual event handlers, override them in your derived class
		virtual void evt_key_up( wxKeyEvent& event ) { event.Skip(); }
		virtual void saveFtpCommand( wxCommandEvent& event ) { event.Skip(); }


	public:

		saveFtpWindow( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Enviar archivo a CNC"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 286,129 ), long style = wxDEFAULT_DIALOG_STYLE|wxSTAY_ON_TOP );

		~saveFtpWindow();

};

///////////////////////////////////////////////////////////////////////////////
/// Class searchDialog
///////////////////////////////////////////////////////////////////////////////
class searchDialog : public wxDialog
{
	private:

	protected:
		wxStaticText* m_staticText5;
		wxTextCtrl* search_term;
		wxStaticText* hint;
		wxButton* m_button2;

		// Virtual event handlers, override them in your derived class
		virtual void evt_key_up( wxKeyEvent& event ) { event.Skip(); }
		virtual void search( wxCommandEvent& event ) { event.Skip(); }


	public:

		searchDialog( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Buscar..."), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 286,156 ), long style = wxDEFAULT_DIALOG_STYLE|wxSTAY_ON_TOP );

		~searchDialog();

};

///////////////////////////////////////////////////////////////////////////////
/// Class search_and_replace
///////////////////////////////////////////////////////////////////////////////
class search_and_replace : public wxDialog
{
	private:

	protected:
		wxStaticText* m_staticText6;
		wxTextCtrl* m_search;
		wxStaticText* m_staticText7;
		wxTextCtrl* m_replace;
		wxButton* m_button2;
		wxButton* m_button3;

		// Virtual event handlers, override them in your derived class
		virtual void evt_key_up( wxKeyEvent& event ) { event.Skip(); }
		virtual void start_replace( wxCommandEvent& event ) { event.Skip(); }
		virtual void close( wxCommandEvent& event ) { event.Skip(); }


	public:

		search_and_replace( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Buscar y reemplazar"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxCAPTION|wxDEFAULT_DIALOG_STYLE|wxSTAY_ON_TOP|wxSYSTEM_MENU );

		~search_and_replace();

};

///////////////////////////////////////////////////////////////////////////////
/// Class about
///////////////////////////////////////////////////////////////////////////////
class about : public wxDialog
{
	private:

	protected:
		wxStaticBitmap* m_bitmap5;
		wxStaticText* m_staticText18;
		wxStaticLine* m_staticline2;
		wxStaticText* m_staticText19;
		wxStaticText* m_staticText201;
		wxStaticText* m_staticText181;
		wxStaticText* m_staticText_fechaUltimaActualizacion;
		wxStaticText* m_staticText_version;
		wxStaticText* m_staticText15;

		// Virtual event handlers, override them in your derived class
		virtual void evt_key_up( wxKeyEvent& event ) { event.Skip(); }


	public:

		about( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Acerca de"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 454,233 ), long style = wxDEFAULT_DIALOG_STYLE|wxSTAY_ON_TOP );

		~about();

};

///////////////////////////////////////////////////////////////////////////////
/// Class ComprobarActualizaciones
///////////////////////////////////////////////////////////////////////////////
class ComprobarActualizaciones : public wxDialog
{
	private:

	protected:
		wxStaticBitmap* m_bitmap3;
		wxGauge* m_gauge;
		wxStaticText* m_staticText21;
		wxButton* m_button6;

		// Virtual event handlers, override them in your derived class
		virtual void close( wxCommandEvent& event ) { event.Skip(); }


	public:

		ComprobarActualizaciones( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Comprobar actualizaciones"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 370,225 ), long style = wxDEFAULT_DIALOG_STYLE );

		~ComprobarActualizaciones();

};

///////////////////////////////////////////////////////////////////////////////
/// Class wxParameters
///////////////////////////////////////////////////////////////////////////////
class wxParameters : public wxDialog
{
	private:

	protected:
		wxStaticText* m_staticText1;
		wxTextCtrl* m_replace_from;
		wxStaticText* m_staticText3;
		wxTextCtrl* m_replace_to;
		wxStaticText* m_staticText2;
		wxCheckBox* m_remove_m08;
		wxStaticText* m_staticText9;
		wxColourPickerCtrl* m_colour_textCtrl;
		wxStaticText* m_staticText18;
		wxColourPickerCtrl* m_colour_text;
		wxStaticText* m_staticText19;
		wxColourPickerCtrl* m_colour_m;
		wxStaticText* m_staticText20;
		wxColourPickerCtrl* m_colour_tool;
		wxStaticText* m_staticText21;
		wxColourPickerCtrl* m_colour_comments;
		wxStaticText* m_staticText23;
		wxColourPickerCtrl* m_colour_line;
		wxCheckBox* m_maximize;
		wxButton* reset;
		wxButton* m_button4;

		// Virtual event handlers, override them in your derived class
		virtual void remove_m08( wxCommandEvent& event ) { event.Skip(); }
		virtual void colour_textCtrl( wxColourPickerEvent& event ) { event.Skip(); }
		virtual void colour_text( wxColourPickerEvent& event ) { event.Skip(); }
		virtual void colour_m( wxColourPickerEvent& event ) { event.Skip(); }
		virtual void colour_tool( wxColourPickerEvent& event ) { event.Skip(); }
		virtual void colour_comments( wxColourPickerEvent& event ) { event.Skip(); }
		virtual void colour_line( wxColourPickerEvent& event ) { event.Skip(); }
		virtual void maximize( wxCommandEvent& event ) { event.Skip(); }
		virtual void reset_defaults( wxCommandEvent& event ) { event.Skip(); }
		virtual void save( wxCommandEvent& event ) { event.Skip(); }


	public:

		wxParameters( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Opciones"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 485,433 ), long style = wxDEFAULT_DIALOG_STYLE );

		~wxParameters();

};

