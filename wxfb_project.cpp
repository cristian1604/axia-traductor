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

	wxString m_syntax_slectionChoices[] = { wxT("Sintaxis para 8025"), wxT("Sintaxis para 8035") };
	int m_syntax_slectionNChoices = sizeof( m_syntax_slectionChoices ) / sizeof( wxString );
	m_syntax_slection = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_syntax_slectionNChoices, m_syntax_slectionChoices, 0 );
	m_syntax_slection->SetSelection( 0 );
	gSizer1->Add( m_syntax_slection, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxRIGHT|wxLEFT, 5 );

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

	m_tool4 = m_toolBar1->AddTool( wxID_ANY, wxT("tool"), wxBitmap( wxT("resources/cog_go.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxT("Convertir código a Fagor 8035"), wxT("Convertir código a Fagor 8035"), NULL );

	m_toolBar1->AddSeparator();

	m_tool5 = m_toolBar1->AddTool( wxID_ANY, wxT("tool"), wxBitmap( wxT("resources/wrench.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxT("Parámetros de conversión"), wxT("Parámetros de conversión"), NULL );

	m_toolBar1->Realize();

	m_menubar1 = new wxMenuBar( 0 );
	m_menu1 = new wxMenu();
	wxMenuItem* m_menuItem1;
	m_menuItem1 = new wxMenuItem( m_menu1, wxID_ANY, wxString( wxT("Cargar archivo") ) + wxT('\t') + wxT("CTRL+O"), wxEmptyString, wxITEM_NORMAL );
	#ifdef __WXMSW__
	m_menuItem1->SetBitmaps( wxBitmap( wxT("resources/folder.png"), wxBITMAP_TYPE_ANY ) );
	#elif (defined( __WXGTK__ ) || defined( __WXOSX__ ))
	m_menuItem1->SetBitmap( wxBitmap( wxT("resources/folder.png"), wxBITMAP_TYPE_ANY ) );
	#endif
	m_menu1->Append( m_menuItem1 );

	wxMenuItem* m_menuItem2;
	m_menuItem2 = new wxMenuItem( m_menu1, wxID_ANY, wxString( wxT("Guardar programa") ) + wxT('\t') + wxT("CTRL+S"), wxEmptyString, wxITEM_NORMAL );
	#ifdef __WXMSW__
	m_menuItem2->SetBitmaps( wxBitmap( wxT("resources/script_save.png"), wxBITMAP_TYPE_ANY ) );
	#elif (defined( __WXGTK__ ) || defined( __WXOSX__ ))
	m_menuItem2->SetBitmap( wxBitmap( wxT("resources/script_save.png"), wxBITMAP_TYPE_ANY ) );
	#endif
	m_menu1->Append( m_menuItem2 );

	m_menubar1->Append( m_menu1, wxT("&Archivo") );

	m_menu3 = new wxMenu();
	wxMenuItem* m_menuItem10;
	m_menuItem10 = new wxMenuItem( m_menu3, wxID_ANY, wxString( wxT("Copiar programa completo") ) + wxT('\t') + wxT("CTRL+SHIFT+C"), wxEmptyString, wxITEM_NORMAL );
	#ifdef __WXMSW__
	m_menuItem10->SetBitmaps( wxBitmap( wxT("resources/copy.gif"), wxBITMAP_TYPE_ANY ) );
	#elif (defined( __WXGTK__ ) || defined( __WXOSX__ ))
	m_menuItem10->SetBitmap( wxBitmap( wxT("resources/copy.gif"), wxBITMAP_TYPE_ANY ) );
	#endif
	m_menu3->Append( m_menuItem10 );

	wxMenuItem* m_menuItem11;
	m_menuItem11 = new wxMenuItem( m_menu3, wxID_ANY, wxString( wxT("Pegar reemplazando todo") ) + wxT('\t') + wxT("CTRL+SHIFT+V"), wxEmptyString, wxITEM_NORMAL );
	#ifdef __WXMSW__
	m_menuItem11->SetBitmaps( wxBitmap( wxT("resources/paste_plain.png"), wxBITMAP_TYPE_ANY ) );
	#elif (defined( __WXGTK__ ) || defined( __WXOSX__ ))
	m_menuItem11->SetBitmap( wxBitmap( wxT("resources/paste_plain.png"), wxBITMAP_TYPE_ANY ) );
	#endif
	m_menu3->Append( m_menuItem11 );

	m_menu3->AppendSeparator();

	wxMenuItem* m_menuItem5;
	m_menuItem5 = new wxMenuItem( m_menu3, wxID_ANY, wxString( wxT("Buscar") ) + wxT('\t') + wxT("CTRL+F"), wxEmptyString, wxITEM_NORMAL );
	#ifdef __WXMSW__
	m_menuItem5->SetBitmaps( wxBitmap( wxT("resources/magnifier.png"), wxBITMAP_TYPE_ANY ) );
	#elif (defined( __WXGTK__ ) || defined( __WXOSX__ ))
	m_menuItem5->SetBitmap( wxBitmap( wxT("resources/magnifier.png"), wxBITMAP_TYPE_ANY ) );
	#endif
	m_menu3->Append( m_menuItem5 );

	wxMenuItem* m_menuItem6;
	m_menuItem6 = new wxMenuItem( m_menu3, wxID_ANY, wxString( wxT("Buscar siguiente") ) + wxT('\t') + wxT("F3"), wxT("Buscar próximo término"), wxITEM_NORMAL );
	#ifdef __WXMSW__
	m_menuItem6->SetBitmaps( wxBitmap( wxT("resources/magnifier_zoom_in.png"), wxBITMAP_TYPE_ANY ) );
	#elif (defined( __WXGTK__ ) || defined( __WXOSX__ ))
	m_menuItem6->SetBitmap( wxBitmap( wxT("resources/magnifier_zoom_in.png"), wxBITMAP_TYPE_ANY ) );
	#endif
	m_menu3->Append( m_menuItem6 );

	wxMenuItem* m_menuItem7;
	m_menuItem7 = new wxMenuItem( m_menu3, wxID_ANY, wxString( wxT("Buscar y reemplazar") ) + wxT('\t') + wxT("CTRL+R"), wxEmptyString, wxITEM_NORMAL );
	#ifdef __WXMSW__
	m_menuItem7->SetBitmaps( wxBitmap( wxT("resources/text_replace.png"), wxBITMAP_TYPE_ANY ) );
	#elif (defined( __WXGTK__ ) || defined( __WXOSX__ ))
	m_menuItem7->SetBitmap( wxBitmap( wxT("resources/text_replace.png"), wxBITMAP_TYPE_ANY ) );
	#endif
	m_menu3->Append( m_menuItem7 );

	m_menu3->AppendSeparator();

	wxMenuItem* m_menuItem9;
	m_menuItem9 = new wxMenuItem( m_menu3, wxID_ANY, wxString( wxT("Enumerar / Re-enumerar líneas") ) + wxT('\t') + wxT("F4"), wxEmptyString, wxITEM_NORMAL );
	#ifdef __WXMSW__
	m_menuItem9->SetBitmaps( wxBitmap( wxT("resources/text_list_numbers.png"), wxBITMAP_TYPE_ANY ) );
	#elif (defined( __WXGTK__ ) || defined( __WXOSX__ ))
	m_menuItem9->SetBitmap( wxBitmap( wxT("resources/text_list_numbers.png"), wxBITMAP_TYPE_ANY ) );
	#endif
	m_menu3->Append( m_menuItem9 );

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

	m_menu5 = new wxMenu();
	wxMenuItem* m_menuItem8;
	m_menuItem8 = new wxMenuItem( m_menu5, wxID_ANY, wxString( wxT("Acerca de...") ) , wxEmptyString, wxITEM_NORMAL );
	#ifdef __WXMSW__
	m_menuItem8->SetBitmaps( wxBitmap( wxT("resources/information.png"), wxBITMAP_TYPE_ANY ) );
	#elif (defined( __WXGTK__ ) || defined( __WXOSX__ ))
	m_menuItem8->SetBitmap( wxBitmap( wxT("resources/information.png"), wxBITMAP_TYPE_ANY ) );
	#endif
	m_menu5->Append( m_menuItem8 );

	m_menubar1->Append( m_menu5, wxT("Ayuda") );

	this->SetMenuBar( m_menubar1 );


	this->Centre( wxBOTH );

	// Connect Events
	m_syntax_slection->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( wxMainWindow::update_syntax_highlight ), NULL, this );
	m_textCtrl->Connect( wxEVT_KEY_UP, wxKeyEventHandler( wxMainWindow::edit_text ), NULL, this );
	m_textCtrl->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( wxMainWindow::update_syntax_highlight ), NULL, this );
	this->Connect( m_tool1->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( wxMainWindow::loadProgramFromFile ) );
	this->Connect( m_tool2->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( wxMainWindow::save_program ) );
	this->Connect( m_tool3->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( wxMainWindow::update_syntax_highlight ) );
	this->Connect( m_tool4->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( wxMainWindow::translate ) );
	this->Connect( m_tool5->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( wxMainWindow::open_options ) );
	m_menu1->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxMainWindow::loadProgramFromFile ), this, m_menuItem1->GetId());
	m_menu1->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxMainWindow::save_program ), this, m_menuItem2->GetId());
	m_menu3->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxMainWindow::copy_program_clipboard ), this, m_menuItem10->GetId());
	m_menu3->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxMainWindow::paste_program_clipboard ), this, m_menuItem11->GetId());
	m_menu3->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxMainWindow::search_window ), this, m_menuItem5->GetId());
	m_menu3->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxMainWindow::search_next ), this, m_menuItem6->GetId());
	m_menu3->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxMainWindow::search_replace_window ), this, m_menuItem7->GetId());
	m_menu3->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxMainWindow::enum_lines ), this, m_menuItem9->GetId());
	m_menu2->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxMainWindow::update_syntax_highlight ), this, m_menuItem3->GetId());
	m_menu2->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxMainWindow::translate ), this, m_menuItem4->GetId());
	m_menu5->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxMainWindow::about ), this, m_menuItem8->GetId());
}

wxMainWindow::~wxMainWindow()
{
	// Disconnect Events
	m_syntax_slection->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( wxMainWindow::update_syntax_highlight ), NULL, this );
	m_textCtrl->Disconnect( wxEVT_KEY_UP, wxKeyEventHandler( wxMainWindow::edit_text ), NULL, this );
	m_textCtrl->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( wxMainWindow::update_syntax_highlight ), NULL, this );
	this->Disconnect( m_tool1->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( wxMainWindow::loadProgramFromFile ) );
	this->Disconnect( m_tool2->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( wxMainWindow::save_program ) );
	this->Disconnect( m_tool3->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( wxMainWindow::update_syntax_highlight ) );
	this->Disconnect( m_tool4->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( wxMainWindow::translate ) );
	this->Disconnect( m_tool5->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( wxMainWindow::open_options ) );

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

	hint = new wxStaticText( this, wxID_ANY, wxT("Para buscar una próxima coincidencia, presione F3"), wxDefaultPosition, wxDefaultSize, 0 );
	hint->Wrap( -1 );
	bSizer3->Add( hint, 0, wxALL, 5 );

	m_button2 = new wxButton( this, wxID_ANY, wxT("Buscar"), wxDefaultPosition, wxDefaultSize, 0 );

	m_button2->SetBitmap( wxBitmap( wxT("resources/magnifier.png"), wxBITMAP_TYPE_ANY ) );
	bSizer3->Add( m_button2, 0, wxALIGN_RIGHT|wxALL, 5 );


	this->SetSizer( bSizer3 );
	this->Layout();

	this->Centre( wxBOTH );

	// Connect Events
	this->Connect( wxEVT_KEY_UP, wxKeyEventHandler( searchDialog::evt_key_up ) );
	search_term->Connect( wxEVT_KEY_UP, wxKeyEventHandler( searchDialog::evt_key_up ), NULL, this );
	search_term->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( searchDialog::search ), NULL, this );
	m_button2->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( searchDialog::search ), NULL, this );
}

searchDialog::~searchDialog()
{
	// Disconnect Events
	this->Disconnect( wxEVT_KEY_UP, wxKeyEventHandler( searchDialog::evt_key_up ) );
	search_term->Disconnect( wxEVT_KEY_UP, wxKeyEventHandler( searchDialog::evt_key_up ), NULL, this );
	search_term->Disconnect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( searchDialog::search ), NULL, this );
	m_button2->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( searchDialog::search ), NULL, this );

}

search_and_replace::search_and_replace( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticText6 = new wxStaticText( this, wxID_ANY, wxT("Buscar todas las ocurrencias de"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText6->Wrap( -1 );
	fgSizer1->Add( m_staticText6, 0, wxALL, 5 );

	m_search = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_search, 0, wxALL|wxEXPAND, 5 );

	m_staticText7 = new wxStaticText( this, wxID_ANY, wxT("Y reemplazarlas por"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText7->Wrap( -1 );
	fgSizer1->Add( m_staticText7, 0, wxALL, 5 );

	m_replace = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_replace, 0, wxALL|wxEXPAND, 5 );


	fgSizer1->Add( 0, 0, 1, wxEXPAND, 5 );

	wxGridSizer* gSizer4;
	gSizer4 = new wxGridSizer( 0, 2, 0, 0 );

	m_button2 = new wxButton( this, wxID_ANY, wxT("Reemplazar"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer4->Add( m_button2, 0, wxALL, 5 );

	m_button3 = new wxButton( this, wxID_ANY, wxT("Cancelar"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer4->Add( m_button3, 0, wxALL, 5 );


	fgSizer1->Add( gSizer4, 1, wxEXPAND, 5 );


	this->SetSizer( fgSizer1 );
	this->Layout();
	fgSizer1->Fit( this );

	this->Centre( wxBOTH );

	// Connect Events
	this->Connect( wxEVT_KEY_UP, wxKeyEventHandler( search_and_replace::evt_key_up ) );
	m_search->Connect( wxEVT_KEY_UP, wxKeyEventHandler( search_and_replace::evt_key_up ), NULL, this );
	m_replace->Connect( wxEVT_KEY_UP, wxKeyEventHandler( search_and_replace::evt_key_up ), NULL, this );
	m_button2->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( search_and_replace::start_replace ), NULL, this );
	m_button3->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( search_and_replace::close ), NULL, this );
}

search_and_replace::~search_and_replace()
{
	// Disconnect Events
	this->Disconnect( wxEVT_KEY_UP, wxKeyEventHandler( search_and_replace::evt_key_up ) );
	m_search->Disconnect( wxEVT_KEY_UP, wxKeyEventHandler( search_and_replace::evt_key_up ), NULL, this );
	m_replace->Disconnect( wxEVT_KEY_UP, wxKeyEventHandler( search_and_replace::evt_key_up ), NULL, this );
	m_button2->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( search_and_replace::start_replace ), NULL, this );
	m_button3->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( search_and_replace::close ), NULL, this );

}

about::about( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );

	wxGridBagSizer* gbSizer1;
	gbSizer1 = new wxGridBagSizer( 0, 0 );
	gbSizer1->SetFlexibleDirection( wxBOTH );
	gbSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_bitmap2 = new wxStaticBitmap( this, wxID_ANY, wxBitmap( wxT("resources/logo_md.png"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxSize( -1,-1 ), 0 );
	gbSizer1->Add( m_bitmap2, wxGBPosition( 0, 0 ), wxGBSpan( 1, 1 ), wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );

	m_staticText8 = new wxStaticText( this, wxID_ANY, wxT("Editor / traductor CNC 8025 a 8035\n\nPuesta en producción: 22/03/2019\n\nVersión: 26/03/2019"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText8->Wrap( -1 );
	gbSizer1->Add( m_staticText8, wxGBPosition( 0, 1 ), wxGBSpan( 1, 1 ), wxALL|wxEXPAND, 5 );


	bSizer4->Add( gbSizer1, 1, wxEXPAND, 1 );

	m_staticText15 = new wxStaticText( this, wxID_ANY, wxT("© 2019 AXIA"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText15->Wrap( -1 );
	m_staticText15->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNSHADOW ) );

	bSizer4->Add( m_staticText15, 0, wxALL|wxALIGN_RIGHT, 5 );


	this->SetSizer( bSizer4 );
	this->Layout();

	this->Centre( wxBOTH );

	// Connect Events
	this->Connect( wxEVT_KEY_UP, wxKeyEventHandler( about::evt_key_up ) );
}

about::~about()
{
	// Disconnect Events
	this->Disconnect( wxEVT_KEY_UP, wxKeyEventHandler( about::evt_key_up ) );

}

wxParameters::wxParameters( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );

	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Opciones de código") ), wxVERTICAL );

	wxGridSizer* gSizer2;
	gSizer2 = new wxGridSizer( 0, 2, 0, 0 );

	m_staticText1 = new wxStaticText( sbSizer1->GetStaticBox(), wxID_ANY, wxT("Reemplazar sentido de giro"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	gSizer2->Add( m_staticText1, 0, wxALL, 5 );

	wxGridSizer* gSizer3;
	gSizer3 = new wxGridSizer( 0, 3, 0, 0 );

	m_replace_from = new wxTextCtrl( sbSizer1->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	gSizer3->Add( m_replace_from, 1, wxALL|wxEXPAND, 5 );

	m_staticText3 = new wxStaticText( sbSizer1->GetStaticBox(), wxID_ANY, wxT("por"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	gSizer3->Add( m_staticText3, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5 );

	m_replace_to = new wxTextCtrl( sbSizer1->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	gSizer3->Add( m_replace_to, 1, wxALL|wxEXPAND, 5 );


	gSizer2->Add( gSizer3, 1, wxEXPAND, 5 );

	m_staticText2 = new wxStaticText( sbSizer1->GetStaticBox(), wxID_ANY, wxT("Opciones "), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	gSizer2->Add( m_staticText2, 0, wxALL, 5 );

	m_remove_m08 = new wxCheckBox( sbSizer1->GetStaticBox(), wxID_ANY, wxT("Quitar parámetro M08"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer2->Add( m_remove_m08, 1, wxALL|wxEXPAND, 5 );


	sbSizer1->Add( gSizer2, 1, wxEXPAND|wxALL, 5 );


	bSizer2->Add( sbSizer1, 0, wxALL|wxEXPAND, 5 );

	wxStaticBoxSizer* sbSizer2;
	sbSizer2 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Colores y opciones de editor") ), wxVERTICAL );

	wxGridSizer* gSizer6;
	gSizer6 = new wxGridSizer( 0, 2, 0, 0 );

	m_staticText9 = new wxStaticText( sbSizer2->GetStaticBox(), wxID_ANY, wxT("Color de fondo de área de texto"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText9->Wrap( -1 );
	gSizer6->Add( m_staticText9, 0, wxALL, 5 );

	m_colour_textCtrl = new wxColourPickerCtrl( sbSizer2->GetStaticBox(), wxID_ANY, wxColour( 0, 30, 60 ), wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE );
	gSizer6->Add( m_colour_textCtrl, 0, wxALL|wxALIGN_RIGHT, 5 );

	m_staticText18 = new wxStaticText( sbSizer2->GetStaticBox(), wxID_ANY, wxT("Color de texto"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText18->Wrap( -1 );
	gSizer6->Add( m_staticText18, 0, wxALL, 5 );

	m_colour_text = new wxColourPickerCtrl( sbSizer2->GetStaticBox(), wxID_ANY, *wxBLACK, wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE );
	gSizer6->Add( m_colour_text, 0, wxALL|wxALIGN_RIGHT, 5 );

	m_staticText19 = new wxStaticText( sbSizer2->GetStaticBox(), wxID_ANY, wxT("Color de sentencia M"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText19->Wrap( -1 );
	gSizer6->Add( m_staticText19, 0, wxALL, 5 );

	m_colour_m = new wxColourPickerCtrl( sbSizer2->GetStaticBox(), wxID_ANY, *wxBLACK, wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE );
	gSizer6->Add( m_colour_m, 0, wxALL|wxALIGN_RIGHT, 5 );

	m_staticText20 = new wxStaticText( sbSizer2->GetStaticBox(), wxID_ANY, wxT("Color sentencia de herramienta"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText20->Wrap( -1 );
	gSizer6->Add( m_staticText20, 0, wxALL, 5 );

	m_colour_tool = new wxColourPickerCtrl( sbSizer2->GetStaticBox(), wxID_ANY, *wxBLACK, wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE );
	gSizer6->Add( m_colour_tool, 0, wxALL|wxALIGN_RIGHT, 5 );

	m_staticText21 = new wxStaticText( sbSizer2->GetStaticBox(), wxID_ANY, wxT("Comentario"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText21->Wrap( -1 );
	gSizer6->Add( m_staticText21, 0, wxALL, 5 );

	m_colour_comments = new wxColourPickerCtrl( sbSizer2->GetStaticBox(), wxID_ANY, *wxBLACK, wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE );
	gSizer6->Add( m_colour_comments, 0, wxALL|wxALIGN_RIGHT, 5 );

	m_staticText23 = new wxStaticText( sbSizer2->GetStaticBox(), wxID_ANY, wxT("Color marcador de Número de línea"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText23->Wrap( -1 );
	gSizer6->Add( m_staticText23, 0, wxALL, 5 );

	m_colour_line = new wxColourPickerCtrl( sbSizer2->GetStaticBox(), wxID_ANY, *wxBLACK, wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE );
	gSizer6->Add( m_colour_line, 0, wxALL|wxALIGN_RIGHT, 5 );

	m_maximize = new wxCheckBox( sbSizer2->GetStaticBox(), wxID_ANY, wxT("Iniciar ventana maximizada"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer6->Add( m_maximize, 0, wxALL, 5 );

	reset = new wxButton( sbSizer2->GetStaticBox(), wxID_ANY, wxT("Reestablecer"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer6->Add( reset, 0, wxALL|wxALIGN_RIGHT, 5 );


	sbSizer2->Add( gSizer6, 1, wxEXPAND, 5 );


	bSizer2->Add( sbSizer2, 1, wxEXPAND|wxRIGHT|wxLEFT, 5 );

	m_button4 = new wxButton( this, wxID_ANY, wxT("Guardar"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_button4, 0, wxALL|wxALIGN_RIGHT, 5 );


	this->SetSizer( bSizer2 );
	this->Layout();

	this->Centre( wxBOTH );

	// Connect Events
	m_remove_m08->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( wxParameters::remove_m08 ), NULL, this );
	m_colour_textCtrl->Connect( wxEVT_COMMAND_COLOURPICKER_CHANGED, wxColourPickerEventHandler( wxParameters::colour_textCtrl ), NULL, this );
	m_colour_text->Connect( wxEVT_COMMAND_COLOURPICKER_CHANGED, wxColourPickerEventHandler( wxParameters::colour_text ), NULL, this );
	m_colour_m->Connect( wxEVT_COMMAND_COLOURPICKER_CHANGED, wxColourPickerEventHandler( wxParameters::colour_m ), NULL, this );
	m_colour_tool->Connect( wxEVT_COMMAND_COLOURPICKER_CHANGED, wxColourPickerEventHandler( wxParameters::colour_tool ), NULL, this );
	m_colour_comments->Connect( wxEVT_COMMAND_COLOURPICKER_CHANGED, wxColourPickerEventHandler( wxParameters::colour_comments ), NULL, this );
	m_colour_line->Connect( wxEVT_COMMAND_COLOURPICKER_CHANGED, wxColourPickerEventHandler( wxParameters::colour_line ), NULL, this );
	m_maximize->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( wxParameters::maximize ), NULL, this );
	reset->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( wxParameters::reset_defaults ), NULL, this );
	m_button4->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( wxParameters::save ), NULL, this );
}

wxParameters::~wxParameters()
{
	// Disconnect Events
	m_remove_m08->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( wxParameters::remove_m08 ), NULL, this );
	m_colour_textCtrl->Disconnect( wxEVT_COMMAND_COLOURPICKER_CHANGED, wxColourPickerEventHandler( wxParameters::colour_textCtrl ), NULL, this );
	m_colour_text->Disconnect( wxEVT_COMMAND_COLOURPICKER_CHANGED, wxColourPickerEventHandler( wxParameters::colour_text ), NULL, this );
	m_colour_m->Disconnect( wxEVT_COMMAND_COLOURPICKER_CHANGED, wxColourPickerEventHandler( wxParameters::colour_m ), NULL, this );
	m_colour_tool->Disconnect( wxEVT_COMMAND_COLOURPICKER_CHANGED, wxColourPickerEventHandler( wxParameters::colour_tool ), NULL, this );
	m_colour_comments->Disconnect( wxEVT_COMMAND_COLOURPICKER_CHANGED, wxColourPickerEventHandler( wxParameters::colour_comments ), NULL, this );
	m_colour_line->Disconnect( wxEVT_COMMAND_COLOURPICKER_CHANGED, wxColourPickerEventHandler( wxParameters::colour_line ), NULL, this );
	m_maximize->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( wxParameters::maximize ), NULL, this );
	reset->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( wxParameters::reset_defaults ), NULL, this );
	m_button4->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( wxParameters::save ), NULL, this );

}
