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

	m_bitmap1 = new wxStaticBitmap( this, wxID_ANY, wxBitmap( wxT("resources/logo_md.png"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxSize( -1,40 ), 0 );
	gSizer1->Add( m_bitmap1, 0, wxALL|wxALIGN_RIGHT, 5 );


	bSizer1->Add( gSizer1, 0, wxEXPAND, 5 );

	m_splitter1 = new wxSplitterWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D|wxSP_3DBORDER|wxSP_3DSASH|wxSP_BORDER );
	m_splitter1->Connect( wxEVT_IDLE, wxIdleEventHandler( wxMainWindow::m_splitter1OnIdle ), NULL, this );

	m_panel1 = new wxPanel( m_splitter1, wxID_ANY, wxDefaultPosition, wxSize( -1,-1 ), wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxVERTICAL );

	m_toolBar2 = new wxToolBar( m_panel1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_HORIZONTAL );
	m_tool6 = m_toolBar2->AddTool( wxID_ANY, wxT("tool"), wxBitmap( wxT("resources/plug-connect.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxT("Conectar a CNC"), wxT("Conectar a CNC"), NULL );

	ftp_desconectar = m_toolBar2->AddTool( wxID_ANY, wxT("tool"), wxBitmap( wxT("resources/plug-disconnect.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxT("Desconectar de control CNC"), wxT("Desconectar de control CNC"), NULL );

	m_toolBar2->AddSeparator();

	m_tool10 = m_toolBar2->AddTool( wxID_ANY, wxT("tool"), wxBitmap( wxT("resources/refresh.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxT("Actualizar"), wxT("Actualizar"), NULL );

	ftp_eliminar = m_toolBar2->AddTool( wxID_ANY, wxT("tool"), wxBitmap( wxT("resources/cross.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxT("Eliminar archivo seleccionado"), wxT("Eliminar archivo seleccionado"), NULL );

	ftp_renombrar = m_toolBar2->AddTool( wxID_ANY, wxT("tool"), wxBitmap( wxT("resources/document-rename.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxT("Renombrar archivo seleccionado"), wxT("Renombrar archivo seleccionado"), NULL );

	m_toolBar2->Realize();

	bSizer6->Add( m_toolBar2, 0, wxEXPAND, 5 );

	m_treeCtrl1 = new wxTreeCtrl( m_panel1, wxID_ANY, wxDefaultPosition, wxSize( -1,-1 ), wxTR_DEFAULT_STYLE );
	bSizer6->Add( m_treeCtrl1, 1, wxALL|wxEXPAND, 5 );


	m_panel1->SetSizer( bSizer6 );
	m_panel1->Layout();
	bSizer6->Fit( m_panel1 );
	ftpOptions = new wxMenu();
	wxMenuItem* m_menuItem16;
	m_menuItem16 = new wxMenuItem( ftpOptions, wxID_ANY, wxString( wxT("WASINO 8035") ) , wxEmptyString, wxITEM_NORMAL );
	#ifdef __WXMSW__
	m_menuItem16->SetBitmaps( wxBitmap( wxT("resources/server.png"), wxBITMAP_TYPE_ANY ) );
	#elif (defined( __WXGTK__ ) || defined( __WXOSX__ ))
	m_menuItem16->SetBitmap( wxBitmap( wxT("resources/server.png"), wxBITMAP_TYPE_ANY ) );
	#endif
	ftpOptions->Append( m_menuItem16 );

	wxMenuItem* m_menuItem161;
	m_menuItem161 = new wxMenuItem( ftpOptions, wxID_ANY, wxString( wxT("TAKISAWA 8037") ) , wxEmptyString, wxITEM_NORMAL );
	#ifdef __WXMSW__
	m_menuItem161->SetBitmaps( wxBitmap( wxT("resources/server.png"), wxBITMAP_TYPE_ANY ) );
	#elif (defined( __WXGTK__ ) || defined( __WXOSX__ ))
	m_menuItem161->SetBitmap( wxBitmap( wxT("resources/server.png"), wxBITMAP_TYPE_ANY ) );
	#endif
	ftpOptions->Append( m_menuItem161 );

	m_panel1->Connect( wxEVT_RIGHT_DOWN, wxMouseEventHandler( wxMainWindow::m_panel1OnContextMenu ), NULL, this );

	m_panel2 = new wxPanel( m_splitter1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxVERTICAL );

	m_textCtrl = new wxTextCtrl( m_panel2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_AUTO_URL|wxTE_MULTILINE|wxTE_RICH2 );
	bSizer7->Add( m_textCtrl, 1, wxEXPAND|wxALL, 5 );


	m_panel2->SetSizer( bSizer7 );
	m_panel2->Layout();
	bSizer7->Fit( m_panel2 );
	m_splitter1->SplitVertically( m_panel1, m_panel2, 198 );
	bSizer1->Add( m_splitter1, 1, wxEXPAND, 5 );


	this->SetSizer( bSizer1 );
	this->Layout();
	m_statusBar = this->CreateStatusBar( 3, 0, wxID_ANY );
	m_toolBar1 = this->CreateToolBar( wxTB_HORIZONTAL, wxID_ANY );
	m_toolBar1->SetBackgroundColour( wxColour( 213, 213, 255 ) );

	m_tool1 = m_toolBar1->AddTool( wxID_ANY, wxT("tool"), wxBitmap( wxT("resources/folder.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxT("Cargar archivo"), wxT("Cargar un archivo generado para 8025"), NULL );

	m_tool2 = m_toolBar1->AddTool( wxID_ANY, wxT("tool"), wxBitmap( wxT("resources/disk.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxT("Guardar archivo"), wxT("Guardar archivo en el estado actual"), NULL );

	m_toolBar1->AddSeparator();

	m_tool3 = m_toolBar1->AddTool( wxID_ANY, wxT("tool"), wxBitmap( wxT("resources/wand.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxT("Actualizar coloración de sintaxis"), wxT("Actualizar coloración de sintaxis"), NULL );

	m_tool4 = m_toolBar1->AddTool( wxID_ANY, wxT("tool"), wxBitmap( wxT("resources/convert.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxT("Convertir código a Fagor 8035 (F9)"), wxT("Convertir código a Fagor 8035  (F9)"), NULL );

	m_toolBar1->AddSeparator();

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
	m_menuItem2->SetBitmaps( wxBitmap( wxT("resources/disk.png"), wxBITMAP_TYPE_ANY ) );
	#elif (defined( __WXGTK__ ) || defined( __WXOSX__ ))
	m_menuItem2->SetBitmap( wxBitmap( wxT("resources/disk.png"), wxBITMAP_TYPE_ANY ) );
	#endif
	m_menu1->Append( m_menuItem2 );

	m_menubar1->Append( m_menu1, wxT("&Archivo") );

	m_menu3 = new wxMenu();
	wxMenuItem* m_menuItem13;
	m_menuItem13 = new wxMenuItem( m_menu3, wxID_ANY, wxString( wxT("Pegar formateando a código NC") ) + wxT('\t') + wxT("CTRL+ALT+V"), wxEmptyString, wxITEM_NORMAL );
	#ifdef __WXMSW__
	m_menuItem13->SetBitmaps( wxBitmap( wxT("resources/page_paste.png"), wxBITMAP_TYPE_ANY ) );
	#elif (defined( __WXGTK__ ) || defined( __WXOSX__ ))
	m_menuItem13->SetBitmap( wxBitmap( wxT("resources/page_paste.png"), wxBITMAP_TYPE_ANY ) );
	#endif
	m_menu3->Append( m_menuItem13 );

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

	wxMenuItem* m_search_next;
	m_search_next = new wxMenuItem( m_menu3, wxID_ANY, wxString( wxT("Buscar siguiente") ) + wxT('\t') + wxT("F3"), wxT("Buscar próximo término"), wxITEM_NORMAL );
	#ifdef __WXMSW__
	m_search_next->SetBitmaps( wxBitmap( wxT("resources/magnifier_zoom_in.png"), wxBITMAP_TYPE_ANY ) );
	#elif (defined( __WXGTK__ ) || defined( __WXOSX__ ))
	m_search_next->SetBitmap( wxBitmap( wxT("resources/magnifier_zoom_in.png"), wxBITMAP_TYPE_ANY ) );
	#endif
	m_menu3->Append( m_search_next );

	wxMenuItem* m_menuItem7;
	m_menuItem7 = new wxMenuItem( m_menu3, wxID_ANY, wxString( wxT("Buscar y reemplazar") ) + wxT('\t') + wxT("CTRL+R"), wxEmptyString, wxITEM_NORMAL );
	#ifdef __WXMSW__
	m_menuItem7->SetBitmaps( wxBitmap( wxT("resources/edit-replace.png"), wxBITMAP_TYPE_ANY ) );
	#elif (defined( __WXGTK__ ) || defined( __WXOSX__ ))
	m_menuItem7->SetBitmap( wxBitmap( wxT("resources/edit-replace.png"), wxBITMAP_TYPE_ANY ) );
	#endif
	m_menu3->Append( m_menuItem7 );

	m_menu3->AppendSeparator();

	wxMenuItem* m_menuItem9;
	m_menuItem9 = new wxMenuItem( m_menu3, wxID_ANY, wxString( wxT("Enumerar / Re-enumerar líneas") ) + wxT('\t') + wxT("F8"), wxEmptyString, wxITEM_NORMAL );
	#ifdef __WXMSW__
	m_menuItem9->SetBitmaps( wxBitmap( wxT("resources/text_list_numbers.png"), wxBITMAP_TYPE_ANY ) );
	#elif (defined( __WXGTK__ ) || defined( __WXOSX__ ))
	m_menuItem9->SetBitmap( wxBitmap( wxT("resources/text_list_numbers.png"), wxBITMAP_TYPE_ANY ) );
	#endif
	m_menu3->Append( m_menuItem9 );

	m_menu3->AppendSeparator();

	wxMenuItem* m_menuItem18;
	m_menuItem18 = new wxMenuItem( m_menu3, wxID_ANY, wxString( wxT("Parámetros") ) + wxT('\t') + wxT("F10"), wxEmptyString, wxITEM_NORMAL );
	#ifdef __WXMSW__
	m_menuItem18->SetBitmaps( wxBitmap( wxT("resources/settings-icon16.png"), wxBITMAP_TYPE_ANY ) );
	#elif (defined( __WXGTK__ ) || defined( __WXOSX__ ))
	m_menuItem18->SetBitmap( wxBitmap( wxT("resources/settings-icon16.png"), wxBITMAP_TYPE_ANY ) );
	#endif
	m_menu3->Append( m_menuItem18 );

	m_menubar1->Append( m_menu3, wxT("Editar") );

	m_menu2 = new wxMenu();
	wxMenuItem* m_menuItem3;
	m_menuItem3 = new wxMenuItem( m_menu2, wxID_ANY, wxString( wxT("Actualizar coloración de sintaxis") ) + wxT('\t') + wxT("F11"), wxEmptyString, wxITEM_NORMAL );
	#ifdef __WXMSW__
	m_menuItem3->SetBitmaps( wxBitmap( wxT("resources/refresh.png"), wxBITMAP_TYPE_ANY ) );
	#elif (defined( __WXGTK__ ) || defined( __WXOSX__ ))
	m_menuItem3->SetBitmap( wxBitmap( wxT("resources/refresh.png"), wxBITMAP_TYPE_ANY ) );
	#endif
	m_menu2->Append( m_menuItem3 );

	wxMenuItem* m_menuItem4;
	m_menuItem4 = new wxMenuItem( m_menu2, wxID_ANY, wxString( wxT("Convertir a 8035") ) + wxT('\t') + wxT("F9"), wxEmptyString, wxITEM_NORMAL );
	#ifdef __WXMSW__
	m_menuItem4->SetBitmaps( wxBitmap( wxT("resources/cog_go.png"), wxBITMAP_TYPE_ANY ) );
	#elif (defined( __WXGTK__ ) || defined( __WXOSX__ ))
	m_menuItem4->SetBitmap( wxBitmap( wxT("resources/cog_go.png"), wxBITMAP_TYPE_ANY ) );
	#endif
	m_menu2->Append( m_menuItem4 );

	m_menu2->AppendSeparator();

	wxMenuItem* m_menuItem14;
	m_menuItem14 = new wxMenuItem( m_menu2, wxID_ANY, wxString( wxT("Simular") ) + wxT('\t') + wxT("F4"), wxEmptyString, wxITEM_NORMAL );
	#ifdef __WXMSW__
	m_menuItem14->SetBitmaps( wxBitmap( wxT("resources/chart_line.png"), wxBITMAP_TYPE_ANY ) );
	#elif (defined( __WXGTK__ ) || defined( __WXOSX__ ))
	m_menuItem14->SetBitmap( wxBitmap( wxT("resources/chart_line.png"), wxBITMAP_TYPE_ANY ) );
	#endif
	m_menu2->Append( m_menuItem14 );

	wxMenuItem* m_menuItem12;
	m_menuItem12 = new wxMenuItem( m_menu2, wxID_ANY, wxString( wxT("Canales") ) + wxT('\t') + wxT("F5"), wxEmptyString, wxITEM_NORMAL );
	#ifdef __WXMSW__
	m_menuItem12->SetBitmaps( wxBitmap( wxT("resources/text_letter_omega.png"), wxBITMAP_TYPE_ANY ) );
	#elif (defined( __WXGTK__ ) || defined( __WXOSX__ ))
	m_menuItem12->SetBitmap( wxBitmap( wxT("resources/text_letter_omega.png"), wxBITMAP_TYPE_ANY ) );
	#endif
	m_menu2->Append( m_menuItem12 );

	m_menu2->AppendSeparator();

	wxMenuItem* m_menuItem15;
	m_menuItem15 = new wxMenuItem( m_menu2, wxID_ANY, wxString( wxT("Enviar programa a torno") ) + wxT('\t') + wxT("F2"), wxEmptyString, wxITEM_NORMAL );
	#ifdef __WXMSW__
	m_menuItem15->SetBitmaps( wxBitmap( wxT("resources/navigation-000-frame.png"), wxBITMAP_TYPE_ANY ) );
	#elif (defined( __WXGTK__ ) || defined( __WXOSX__ ))
	m_menuItem15->SetBitmap( wxBitmap( wxT("resources/navigation-000-frame.png"), wxBITMAP_TYPE_ANY ) );
	#endif
	m_menu2->Append( m_menuItem15 );

	m_menubar1->Append( m_menu2, wxT("&Herramientas") );

	m_menu5 = new wxMenu();
	wxMenuItem* m_menuItem8;
	m_menuItem8 = new wxMenuItem( m_menu5, wxID_ANY, wxString( wxT("Acerca de...") ) + wxT('\t') + wxT("F12"), wxT("Información del programa"), wxITEM_NORMAL );
	#ifdef __WXMSW__
	m_menuItem8->SetBitmaps( wxBitmap( wxT("resources/information.png"), wxBITMAP_TYPE_ANY ) );
	#elif (defined( __WXGTK__ ) || defined( __WXOSX__ ))
	m_menuItem8->SetBitmap( wxBitmap( wxT("resources/information.png"), wxBITMAP_TYPE_ANY ) );
	#endif
	m_menu5->Append( m_menuItem8 );

	wxMenuItem* m_menuItem19;
	m_menuItem19 = new wxMenuItem( m_menu5, wxID_ANY, wxString( wxT("Comprobar actualizaciones") ) , wxT("Comprobar si hay actualizaciones del software"), wxITEM_NORMAL );
	#ifdef __WXMSW__
	m_menuItem19->SetBitmaps( wxBitmap( wxT("resources/application-browser-icon.png"), wxBITMAP_TYPE_ANY ) );
	#elif (defined( __WXGTK__ ) || defined( __WXOSX__ ))
	m_menuItem19->SetBitmap( wxBitmap( wxT("resources/application-browser-icon.png"), wxBITMAP_TYPE_ANY ) );
	#endif
	m_menu5->Append( m_menuItem19 );

	m_menubar1->Append( m_menu5, wxT("Ayuda") );

	this->SetMenuBar( m_menubar1 );


	this->Centre( wxBOTH );

	// Connect Events
	m_syntax_slection->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( wxMainWindow::update_syntax_highlight ), NULL, this );
	this->Connect( m_tool6->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( wxMainWindow::connectFtpMenu ) );
	this->Connect( ftp_desconectar->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( wxMainWindow::FtpDisconnect ) );
	this->Connect( m_tool10->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( wxMainWindow::FtpRefresh ) );
	this->Connect( ftp_eliminar->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( wxMainWindow::deleteFtpFile ) );
	this->Connect( ftp_renombrar->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( wxMainWindow::RenameFtpFile ) );
	m_treeCtrl1->Connect( wxEVT_LEFT_DCLICK, wxMouseEventHandler( wxMainWindow::openFtpFile ), NULL, this );
	m_treeCtrl1->Connect( wxEVT_COMMAND_TREE_ITEM_MENU, wxTreeEventHandler( wxMainWindow::ftpFileOptions ), NULL, this );
	ftpOptions->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxMainWindow::FtpConnect8035 ), this, m_menuItem16->GetId());
	ftpOptions->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxMainWindow::FtpConnect8037 ), this, m_menuItem161->GetId());
	m_textCtrl->Connect( wxEVT_KEY_UP, wxKeyEventHandler( wxMainWindow::edit_text ), NULL, this );
	m_textCtrl->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( wxMainWindow::update_syntax_highlight ), NULL, this );
	this->Connect( m_tool1->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( wxMainWindow::loadProgramFromFile ) );
	this->Connect( m_tool2->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( wxMainWindow::save_program ) );
	this->Connect( m_tool3->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( wxMainWindow::update_syntax_highlight ) );
	this->Connect( m_tool4->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( wxMainWindow::translate ) );
	m_menu1->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxMainWindow::loadProgramFromFile ), this, m_menuItem1->GetId());
	m_menu1->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxMainWindow::save_program ), this, m_menuItem2->GetId());
	m_menu3->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxMainWindow::paste_formatting ), this, m_menuItem13->GetId());
	m_menu3->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxMainWindow::copy_program_clipboard ), this, m_menuItem10->GetId());
	m_menu3->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxMainWindow::paste_program_clipboard ), this, m_menuItem11->GetId());
	m_menu3->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxMainWindow::search_window ), this, m_menuItem5->GetId());
	m_menu3->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxMainWindow::search_next ), this, m_search_next->GetId());
	m_menu3->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxMainWindow::search_replace_window ), this, m_menuItem7->GetId());
	m_menu3->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxMainWindow::enum_lines ), this, m_menuItem9->GetId());
	m_menu3->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxMainWindow::open_options ), this, m_menuItem18->GetId());
	m_menu2->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxMainWindow::update_syntax_highlight ), this, m_menuItem3->GetId());
	m_menu2->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxMainWindow::translate ), this, m_menuItem4->GetId());
	m_menu2->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxMainWindow::simulate ), this, m_menuItem14->GetId());
	m_menu2->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxMainWindow::channels ), this, m_menuItem12->GetId());
	m_menu2->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxMainWindow::openFormSendProgram ), this, m_menuItem15->GetId());
	m_menu5->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxMainWindow::about ), this, m_menuItem8->GetId());
	m_menu5->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxMainWindow::checkUpdates ), this, m_menuItem19->GetId());
}

wxMainWindow::~wxMainWindow()
{
	// Disconnect Events
	m_syntax_slection->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( wxMainWindow::update_syntax_highlight ), NULL, this );
	this->Disconnect( m_tool6->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( wxMainWindow::connectFtpMenu ) );
	this->Disconnect( ftp_desconectar->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( wxMainWindow::FtpDisconnect ) );
	this->Disconnect( m_tool10->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( wxMainWindow::FtpRefresh ) );
	this->Disconnect( ftp_eliminar->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( wxMainWindow::deleteFtpFile ) );
	this->Disconnect( ftp_renombrar->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( wxMainWindow::RenameFtpFile ) );
	m_treeCtrl1->Disconnect( wxEVT_LEFT_DCLICK, wxMouseEventHandler( wxMainWindow::openFtpFile ), NULL, this );
	m_treeCtrl1->Disconnect( wxEVT_COMMAND_TREE_ITEM_MENU, wxTreeEventHandler( wxMainWindow::ftpFileOptions ), NULL, this );
	m_textCtrl->Disconnect( wxEVT_KEY_UP, wxKeyEventHandler( wxMainWindow::edit_text ), NULL, this );
	m_textCtrl->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( wxMainWindow::update_syntax_highlight ), NULL, this );
	this->Disconnect( m_tool1->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( wxMainWindow::loadProgramFromFile ) );
	this->Disconnect( m_tool2->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( wxMainWindow::save_program ) );
	this->Disconnect( m_tool3->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( wxMainWindow::update_syntax_highlight ) );
	this->Disconnect( m_tool4->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( wxMainWindow::translate ) );

	delete ftpOptions;
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

	m_search = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER );
	fgSizer1->Add( m_search, 0, wxALL|wxEXPAND, 5 );

	m_staticText7 = new wxStaticText( this, wxID_ANY, wxT("Y reemplazarlas por"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText7->Wrap( -1 );
	fgSizer1->Add( m_staticText7, 0, wxALL, 5 );

	m_replace = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER );
	fgSizer1->Add( m_replace, 0, wxALL|wxEXPAND, 5 );


	fgSizer1->Add( 0, 0, 1, wxEXPAND, 5 );

	wxGridSizer* gSizer4;
	gSizer4 = new wxGridSizer( 0, 2, 0, 0 );

	m_button2 = new wxButton( this, wxID_ANY, wxT("Reemplazar"), wxDefaultPosition, wxDefaultSize, 0 );

	m_button2->SetBitmap( wxBitmap( wxT("resources/edit-replace.png"), wxBITMAP_TYPE_ANY ) );
	gSizer4->Add( m_button2, 0, wxALL, 5 );

	m_button3 = new wxButton( this, wxID_ANY, wxT("Cancelar"), wxDefaultPosition, wxDefaultSize, 0 );

	m_button3->SetBitmap( wxBitmap( wxT("resources/cross.png"), wxBITMAP_TYPE_ANY ) );
	gSizer4->Add( m_button3, 0, wxALL, 5 );


	fgSizer1->Add( gSizer4, 1, wxEXPAND, 5 );


	this->SetSizer( fgSizer1 );
	this->Layout();
	fgSizer1->Fit( this );

	this->Centre( wxBOTH );

	// Connect Events
	this->Connect( wxEVT_KEY_UP, wxKeyEventHandler( search_and_replace::evt_key_up ) );
	m_search->Connect( wxEVT_KEY_UP, wxKeyEventHandler( search_and_replace::evt_key_up ), NULL, this );
	m_search->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( search_and_replace::start_replace ), NULL, this );
	m_replace->Connect( wxEVT_KEY_UP, wxKeyEventHandler( search_and_replace::evt_key_up ), NULL, this );
	m_replace->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( search_and_replace::start_replace ), NULL, this );
	m_button2->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( search_and_replace::start_replace ), NULL, this );
	m_button3->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( search_and_replace::close ), NULL, this );
}

search_and_replace::~search_and_replace()
{
	// Disconnect Events
	this->Disconnect( wxEVT_KEY_UP, wxKeyEventHandler( search_and_replace::evt_key_up ) );
	m_search->Disconnect( wxEVT_KEY_UP, wxKeyEventHandler( search_and_replace::evt_key_up ), NULL, this );
	m_search->Disconnect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( search_and_replace::start_replace ), NULL, this );
	m_replace->Disconnect( wxEVT_KEY_UP, wxKeyEventHandler( search_and_replace::evt_key_up ), NULL, this );
	m_replace->Disconnect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( search_and_replace::start_replace ), NULL, this );
	m_button2->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( search_and_replace::start_replace ), NULL, this );
	m_button3->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( search_and_replace::close ), NULL, this );

}

about::about( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	this->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOWTEXT ) );
	this->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_MENU ) );

	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );

	m_bitmap5 = new wxStaticBitmap( this, wxID_ANY, wxBitmap( wxT("resources/logo_md.png"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxDefaultSize, 0 );
	m_bitmap5->SetForegroundColour( wxColour( 255, 255, 255 ) );
	m_bitmap5->SetBackgroundColour( wxColour( 250, 250, 250 ) );

	bSizer4->Add( m_bitmap5, 1, wxEXPAND|wxBOTTOM, 5 );

	m_staticText18 = new wxStaticText( this, wxID_ANY, wxT("Editor y convertidor de sintaxis Fagor 8025 ► 8035"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText18->SetLabelMarkup( wxT("Editor y convertidor de sintaxis Fagor 8025 ► 8035") );
	m_staticText18->Wrap( -1 );
	m_staticText18->SetFont( wxFont( 10, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxT("Arial") ) );
	m_staticText18->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOWTEXT ) );

	bSizer4->Add( m_staticText18, 0, wxALL, 5 );

	m_staticline2 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer4->Add( m_staticline2, 0, wxEXPAND | wxALL, 5 );

	wxGridSizer* gSizer6;
	gSizer6 = new wxGridSizer( 2, 2, 0, 0 );

	m_staticText19 = new wxStaticText( this, wxID_ANY, wxT("Puesta en producción"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText19->Wrap( -1 );
	m_staticText19->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOWTEXT ) );

	gSizer6->Add( m_staticText19, 0, wxALL, 5 );

	m_staticText201 = new wxStaticText( this, wxID_ANY, wxT("22-03-2019"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText201->Wrap( -1 );
	gSizer6->Add( m_staticText201, 0, wxALL, 5 );

	m_staticText181 = new wxStaticText( this, wxID_ANY, wxT("Última actualización:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText181->Wrap( -1 );
	gSizer6->Add( m_staticText181, 0, wxALL, 5 );

	m_staticText_fechaUltimaActualizacion = new wxStaticText( this, wxID_ANY, wxT("Cargando..."), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText_fechaUltimaActualizacion->Wrap( -1 );
	gSizer6->Add( m_staticText_fechaUltimaActualizacion, 0, wxALL, 5 );


	bSizer4->Add( gSizer6, 1, wxALIGN_CENTER_HORIZONTAL, 5 );

	m_staticText_version = new wxStaticText( this, wxID_ANY, wxT("Versión 1.5"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText_version->Wrap( -1 );
	m_staticText_version->SetFont( wxFont( 10, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxT("Arial") ) );

	bSizer4->Add( m_staticText_version, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );

	m_staticText15 = new wxStaticText( this, wxID_ANY, wxT("© 2019 - 2023 AXIA S.A."), wxDefaultPosition, wxDefaultSize, 0 );
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

ComprobarActualizaciones::ComprobarActualizaciones( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxVERTICAL );

	m_bitmap3 = new wxStaticBitmap( this, wxID_ANY, wxBitmap( wxT("resources/network-2-icon.png"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer7->Add( m_bitmap3, 0, wxALL|wxEXPAND, 5 );

	m_gauge = new wxGauge( this, wxID_ANY, 100, wxDefaultPosition, wxDefaultSize, 0 );
	m_gauge->SetValue( 0 );
	bSizer7->Add( m_gauge, 0, wxALIGN_CENTER_HORIZONTAL, 5 );

	m_staticText21 = new wxStaticText( this, wxID_ANY, wxT("Comprobando actualizaciones"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText21->Wrap( -1 );
	bSizer7->Add( m_staticText21, 1, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );

	m_button6 = new wxButton( this, wxID_ANY, wxT("Cerrar"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer7->Add( m_button6, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );


	this->SetSizer( bSizer7 );
	this->Layout();

	// Connect Events
	m_button6->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ComprobarActualizaciones::close ), NULL, this );
}

ComprobarActualizaciones::~ComprobarActualizaciones()
{
	// Disconnect Events
	m_button6->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ComprobarActualizaciones::close ), NULL, this );

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
