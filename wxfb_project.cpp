///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct 26 2018)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "wxfb_project.h"

///////////////////////////////////////////////////////////////////////////

wxMainWindow::wxMainWindow( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	this->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );

	wxGridSizer* gSizer1;
	gSizer1 = new wxGridSizer( 0, 2, 0, 0 );


	gSizer1->Add( 0, 0, 1, wxEXPAND, 5 );

	m_bitmap1 = new wxStaticBitmap( this, wxID_ANY, wxBitmap( wxT("logo_small.png"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxSize( -1,-1 ), 0 );
	gSizer1->Add( m_bitmap1, 0, wxALL|wxALIGN_RIGHT, 5 );


	bSizer1->Add( gSizer1, 0, wxEXPAND, 5 );

	m_textCtrl = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_AUTO_URL|wxTE_MULTILINE|wxTE_RICH2 );
	bSizer1->Add( m_textCtrl, 1, wxALL|wxEXPAND, 5 );


	this->SetSizer( bSizer1 );
	this->Layout();
	m_statusBar = this->CreateStatusBar( 3, 0, wxID_ANY );
	m_toolBar1 = this->CreateToolBar( wxTB_HORIZONTAL, wxID_ANY );
	m_toolBar1->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_ACTIVECAPTION ) );

	m_tool1 = m_toolBar1->AddTool( wxID_ANY, wxT("tool"), wxBitmap( wxT("resources/folder.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxT("Cargar archivo"), wxT("Cargar un archivo generado para 8025"), NULL );

	m_tool2 = m_toolBar1->AddTool( wxID_ANY, wxT("tool"), wxBitmap( wxT("resources/script_save.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxT("Guardar archivo"), wxT("Guardar archivo en el estado actual"), NULL );

	m_toolBar1->AddSeparator();

	m_tool3 = m_toolBar1->AddTool( wxID_ANY, wxT("tool"), wxBitmap( wxT("resources/refresh.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxT("Actualizar coloreo de sintaxis"), wxT("Actualizar coloreo de sintaxis"), NULL );

	m_tool4 = m_toolBar1->AddTool( wxID_ANY, wxT("tool"), wxBitmap( wxT("resources/cog_go.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL );

	m_toolBar1->Realize();

	m_menubar1 = new wxMenuBar( 0 );
	m_menu1 = new wxMenu();
	wxMenuItem* m_menuItem1;
	m_menuItem1 = new wxMenuItem( m_menu1, wxID_ANY, wxString( wxT("Cargar archivo") ) , wxEmptyString, wxITEM_NORMAL );
	#ifdef __WXMSW__
	m_menuItem1->SetBitmaps( wxBitmap( wxT("resources/folder.png"), wxBITMAP_TYPE_ANY ) );
	#elif (defined( __WXGTK__ ) || defined( __WXOSX__ ))
	m_menuItem1->SetBitmap( wxBitmap( wxT("resources/folder.png"), wxBITMAP_TYPE_ANY ) );
	#endif
	m_menu1->Append( m_menuItem1 );

	wxMenuItem* m_menuItem2;
	m_menuItem2 = new wxMenuItem( m_menu1, wxID_ANY, wxString( wxT("Guardar programa") ) , wxEmptyString, wxITEM_NORMAL );
	#ifdef __WXMSW__
	m_menuItem2->SetBitmaps( wxBitmap( wxT("resources/script_save.png"), wxBITMAP_TYPE_ANY ) );
	#elif (defined( __WXGTK__ ) || defined( __WXOSX__ ))
	m_menuItem2->SetBitmap( wxBitmap( wxT("resources/script_save.png"), wxBITMAP_TYPE_ANY ) );
	#endif
	m_menu1->Append( m_menuItem2 );

	m_menubar1->Append( m_menu1, wxT("&Archivo") );

	m_menu2 = new wxMenu();
	wxMenuItem* m_menuItem3;
	m_menuItem3 = new wxMenuItem( m_menu2, wxID_ANY, wxString( wxT("Actualizar coloración de sintaxis") ) + wxT('\t') + wxT("F5"), wxEmptyString, wxITEM_NORMAL );
	#ifdef __WXMSW__
	m_menuItem3->SetBitmaps( wxBitmap( wxT("resources/refresh.png"), wxBITMAP_TYPE_ANY ) );
	#elif (defined( __WXGTK__ ) || defined( __WXOSX__ ))
	m_menuItem3->SetBitmap( wxBitmap( wxT("resources/refresh.png"), wxBITMAP_TYPE_ANY ) );
	#endif
	m_menu2->Append( m_menuItem3 );

	wxMenuItem* m_menuItem4;
	m_menuItem4 = new wxMenuItem( m_menu2, wxID_ANY, wxString( wxT("Traducir a 8035") ) + wxT('\t') + wxT("F9"), wxEmptyString, wxITEM_NORMAL );
	#ifdef __WXMSW__
	m_menuItem4->SetBitmaps( wxBitmap( wxT("resources/cog_go.png"), wxBITMAP_TYPE_ANY ) );
	#elif (defined( __WXGTK__ ) || defined( __WXOSX__ ))
	m_menuItem4->SetBitmap( wxBitmap( wxT("resources/cog_go.png"), wxBITMAP_TYPE_ANY ) );
	#endif
	m_menu2->Append( m_menuItem4 );

	m_menubar1->Append( m_menu2, wxT("A&cciones") );

	this->SetMenuBar( m_menubar1 );


	this->Centre( wxBOTH );

	// Connect Events
	m_textCtrl->Connect( wxEVT_KEY_UP, wxKeyEventHandler( wxMainWindow::edit_text ), NULL, this );
	this->Connect( m_tool1->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( wxMainWindow::loadProgramFromFile ) );
	this->Connect( m_tool3->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( wxMainWindow::update_syntax_highlight ) );
	this->Connect( m_tool4->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( wxMainWindow::translate ) );
	m_menu1->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxMainWindow::loadProgramFromFile ), this, m_menuItem1->GetId());
	m_menu2->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxMainWindow::update_syntax_highlight ), this, m_menuItem3->GetId());
	m_menu2->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxMainWindow::translate ), this, m_menuItem4->GetId());
}

wxMainWindow::~wxMainWindow()
{
	// Disconnect Events
	m_textCtrl->Disconnect( wxEVT_KEY_UP, wxKeyEventHandler( wxMainWindow::edit_text ), NULL, this );
	this->Disconnect( m_tool1->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( wxMainWindow::loadProgramFromFile ) );
	this->Disconnect( m_tool3->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( wxMainWindow::update_syntax_highlight ) );
	this->Disconnect( m_tool4->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( wxMainWindow::translate ) );

}
