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

	wxString syntax_slectionChoices[] = { wxT("Sintaxis para 8025"), wxT("Sintaxis para 8035") };
	int syntax_slectionNChoices = sizeof( syntax_slectionChoices ) / sizeof( wxString );
	syntax_slection = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, syntax_slectionNChoices, syntax_slectionChoices, 0 );
	syntax_slection->SetSelection( 0 );
	gSizer1->Add( syntax_slection, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

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

	m_tool3 = m_toolBar1->AddTool( wxID_ANY, wxT("tool"), wxBitmap( wxT("resources/refresh.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxT("Actualizar coloración de sintaxis"), wxT("Actualizar coloración de sintaxis"), NULL );

	m_tool4 = m_toolBar1->AddTool( wxID_ANY, wxT("tool"), wxBitmap( wxT("resources/cog_go.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL );

	m_toolBar1->AddSeparator();

	m_tool5 = m_toolBar1->AddTool( wxID_ANY, wxT("tool"), wxBitmap( wxT("resources/wrench.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxT("Parámetros de conversión"), wxT("Parámetros de conversión"), NULL );

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

	m_menu3 = new wxMenu();
	wxMenuItem* m_menuItem5;
	m_menuItem5 = new wxMenuItem( m_menu3, wxID_ANY, wxString( wxT("Buscar") ) + wxT('\t') + wxT("F3"), wxEmptyString, wxITEM_NORMAL );
	#ifdef __WXMSW__
	m_menuItem5->SetBitmaps( wxBitmap( wxT("resources/magnifier.png"), wxBITMAP_TYPE_ANY ) );
	#elif (defined( __WXGTK__ ) || defined( __WXOSX__ ))
	m_menuItem5->SetBitmap( wxBitmap( wxT("resources/magnifier.png"), wxBITMAP_TYPE_ANY ) );
	#endif
	m_menu3->Append( m_menuItem5 );

	wxMenuItem* m_menuItem6;
	m_menuItem6 = new wxMenuItem( m_menu3, wxID_ANY, wxString( wxT("Buscar siguiente") ) + wxT('\t') + wxT("F4"), wxT("Buscar próximo término"), wxITEM_NORMAL );
	#ifdef __WXMSW__
	m_menuItem6->SetBitmaps( wxBitmap( wxT("resources/magnifier_zoom_in.png"), wxBITMAP_TYPE_ANY ) );
	#elif (defined( __WXGTK__ ) || defined( __WXOSX__ ))
	m_menuItem6->SetBitmap( wxBitmap( wxT("resources/magnifier_zoom_in.png"), wxBITMAP_TYPE_ANY ) );
	#endif
	m_menu3->Append( m_menuItem6 );
	m_menuItem6->Enable( false );

	m_menubar1->Append( m_menu3, wxT("Editar") );

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
	syntax_slection->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( wxMainWindow::update_syntax_highlight ), NULL, this );
	m_textCtrl->Connect( wxEVT_KEY_UP, wxKeyEventHandler( wxMainWindow::edit_text ), NULL, this );
	m_textCtrl->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( wxMainWindow::update_syntax_highlight ), NULL, this );
	this->Connect( m_tool1->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( wxMainWindow::loadProgramFromFile ) );
	this->Connect( m_tool3->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( wxMainWindow::update_syntax_highlight ) );
	this->Connect( m_tool4->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( wxMainWindow::translate ) );
	this->Connect( m_tool5->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( wxMainWindow::open_options ) );
	m_menu1->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxMainWindow::loadProgramFromFile ), this, m_menuItem1->GetId());
	m_menu3->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxMainWindow::search_window ), this, m_menuItem5->GetId());
	m_menu3->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxMainWindow::search_next ), this, m_menuItem6->GetId());
	m_menu2->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxMainWindow::update_syntax_highlight ), this, m_menuItem3->GetId());
	m_menu2->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxMainWindow::translate ), this, m_menuItem4->GetId());
}

wxMainWindow::~wxMainWindow()
{
	// Disconnect Events
	syntax_slection->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( wxMainWindow::update_syntax_highlight ), NULL, this );
	m_textCtrl->Disconnect( wxEVT_KEY_UP, wxKeyEventHandler( wxMainWindow::edit_text ), NULL, this );
	m_textCtrl->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( wxMainWindow::update_syntax_highlight ), NULL, this );
	this->Disconnect( m_tool1->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( wxMainWindow::loadProgramFromFile ) );
	this->Disconnect( m_tool3->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( wxMainWindow::update_syntax_highlight ) );
	this->Disconnect( m_tool4->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( wxMainWindow::translate ) );
	this->Disconnect( m_tool5->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( wxMainWindow::open_options ) );

}

wxParameters::wxParameters( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	this->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );

	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );

	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Opciones") ), wxVERTICAL );

	wxGridSizer* gSizer2;
	gSizer2 = new wxGridSizer( 0, 2, 0, 0 );

	m_staticText1 = new wxStaticText( sbSizer1->GetStaticBox(), wxID_ANY, wxT("Reemplazar sentido de giro"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	gSizer2->Add( m_staticText1, 0, wxALL, 5 );

	wxGridSizer* gSizer3;
	gSizer3 = new wxGridSizer( 0, 3, 0, 0 );

	m_textCtrl2 = new wxTextCtrl( sbSizer1->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	gSizer3->Add( m_textCtrl2, 1, wxALL|wxEXPAND, 5 );

	m_staticText3 = new wxStaticText( sbSizer1->GetStaticBox(), wxID_ANY, wxT("por"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	gSizer3->Add( m_staticText3, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5 );

	m_textCtrl3 = new wxTextCtrl( sbSizer1->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	gSizer3->Add( m_textCtrl3, 1, wxALL|wxEXPAND, 5 );


	gSizer2->Add( gSizer3, 1, wxEXPAND, 5 );

	m_staticText2 = new wxStaticText( sbSizer1->GetStaticBox(), wxID_ANY, wxT("Opciones "), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	gSizer2->Add( m_staticText2, 0, wxALL, 5 );

	remove_m08 = new wxCheckBox( sbSizer1->GetStaticBox(), wxID_ANY, wxT("Quitar parámetro M08"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer2->Add( remove_m08, 1, wxALL|wxEXPAND, 5 );


	sbSizer1->Add( gSizer2, 1, wxEXPAND|wxALL, 5 );


	bSizer2->Add( sbSizer1, 0, wxALL|wxEXPAND, 5 );


	this->SetSizer( bSizer2 );
	this->Layout();

	this->Centre( wxBOTH );
}

wxParameters::~wxParameters()
{
}

searchDialog::searchDialog( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );

	m_staticText5 = new wxStaticText( this, wxID_ANY, wxT("Buscar"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText5->Wrap( -1 );
	bSizer3->Add( m_staticText5, 0, wxALL, 5 );

	search_term = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER );
	bSizer3->Add( search_term, 0, wxALL|wxEXPAND, 5 );

	hint = new wxStaticText( this, wxID_ANY, wxT("Para buscar una próxima coincidencia, presione F4"), wxDefaultPosition, wxDefaultSize, 0 );
	hint->Wrap( -1 );
	bSizer3->Add( hint, 0, wxALL, 5 );

	m_button2 = new wxButton( this, wxID_ANY, wxT("Buscar"), wxDefaultPosition, wxDefaultSize, 0 );

	m_button2->SetBitmap( wxBitmap( wxT("resources/magnifier.png"), wxBITMAP_TYPE_ANY ) );
	bSizer3->Add( m_button2, 0, wxALIGN_RIGHT|wxALL, 5 );


	this->SetSizer( bSizer3 );
	this->Layout();

	this->Centre( wxBOTH );

	// Connect Events
	search_term->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( searchDialog::search ), NULL, this );
	m_button2->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( searchDialog::search ), NULL, this );
}

searchDialog::~searchDialog()
{
	// Disconnect Events
	search_term->Disconnect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( searchDialog::search ), NULL, this );
	m_button2->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( searchDialog::search ), NULL, this );

}
