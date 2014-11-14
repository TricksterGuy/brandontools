///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Nov 12 2014)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "BrandonToolsEditorGUI.h"

///////////////////////////////////////////////////////////////////////////

BitmapGUI::BitmapGUI( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	m_splitter1 = new wxSplitterWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D );
	m_splitter1->Connect( wxEVT_IDLE, wxIdleEventHandler( BitmapGUI::m_splitter1OnIdle ), NULL, this );
	
	m_panel1 = new wxPanel( m_splitter1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );
	
	m_splitter2 = new wxSplitterWindow( m_panel1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D );
	m_splitter2->Connect( wxEVT_IDLE, wxIdleEventHandler( BitmapGUI::m_splitter2OnIdle ), NULL, this );
	
	toolPanel = new wxPanel( m_splitter2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 4, 2, 0, 0 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_toggleBtn1 = new wxToggleButton( toolPanel, wxID_ANY, _("Toggle me!"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_toggleBtn1, 0, wxALL, 5 );
	
	m_toggleBtn2 = new wxToggleButton( toolPanel, wxID_ANY, _("Toggle me!"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_toggleBtn2, 0, wxALL, 5 );
	
	m_toggleBtn3 = new wxToggleButton( toolPanel, wxID_ANY, _("Toggle me!"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_toggleBtn3, 0, wxALL, 5 );
	
	m_toggleBtn4 = new wxToggleButton( toolPanel, wxID_ANY, _("Toggle me!"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_toggleBtn4, 0, wxALL, 5 );
	
	m_toggleBtn5 = new wxToggleButton( toolPanel, wxID_ANY, _("Toggle me!"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_toggleBtn5, 0, wxALL, 5 );
	
	m_toggleBtn6 = new wxToggleButton( toolPanel, wxID_ANY, _("Toggle me!"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_toggleBtn6, 0, wxALL, 5 );
	
	m_toggleBtn7 = new wxToggleButton( toolPanel, wxID_ANY, _("Toggle me!"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_toggleBtn7, 0, wxALL, 5 );
	
	m_toggleBtn8 = new wxToggleButton( toolPanel, wxID_ANY, _("Toggle me!"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_toggleBtn8, 0, wxALL, 5 );
	
	
	bSizer4->Add( fgSizer1, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxVERTICAL );
	
	m_colourPicker1 = new wxColourPickerCtrl( toolPanel, wxID_ANY, *wxBLACK, wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE );
	bSizer8->Add( m_colourPicker1, 0, wxALL, 5 );
	
	m_staticText1 = new wxStaticText( toolPanel, wxID_ANY, _("Left"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	bSizer8->Add( m_staticText1, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	
	bSizer6->Add( bSizer8, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer( wxVERTICAL );
	
	m_colourPicker3 = new wxColourPickerCtrl( toolPanel, wxID_ANY, *wxBLACK, wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE );
	bSizer9->Add( m_colourPicker3, 0, wxALL, 5 );
	
	m_staticText2 = new wxStaticText( toolPanel, wxID_ANY, _("Middle"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	bSizer9->Add( m_staticText2, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	
	bSizer6->Add( bSizer9, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer10;
	bSizer10 = new wxBoxSizer( wxVERTICAL );
	
	m_colourPicker2 = new wxColourPickerCtrl( toolPanel, wxID_ANY, *wxBLACK, wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE );
	bSizer10->Add( m_colourPicker2, 0, wxALL, 5 );
	
	m_staticText3 = new wxStaticText( toolPanel, wxID_ANY, _("Right"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	bSizer10->Add( m_staticText3, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	
	bSizer6->Add( bSizer10, 1, wxEXPAND, 5 );
	
	
	bSizer4->Add( bSizer6, 1, wxEXPAND, 5 );
	
	
	toolPanel->SetSizer( bSizer4 );
	toolPanel->Layout();
	bSizer4->Fit( toolPanel );
	palettePanel = new wxPanel( m_splitter2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_splitter2->SplitHorizontally( toolPanel, palettePanel, 271 );
	bSizer2->Add( m_splitter2, 1, wxEXPAND, 5 );
	
	
	m_panel1->SetSizer( bSizer2 );
	m_panel1->Layout();
	bSizer2->Fit( m_panel1 );
	drawingPanel = new wxPanel( m_splitter1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );
	
	
	drawingPanel->SetSizer( bSizer3 );
	drawingPanel->Layout();
	bSizer3->Fit( drawingPanel );
	m_splitter1->SplitVertically( m_panel1, drawingPanel, 0 );
	bSizer1->Add( m_splitter1, 1, wxEXPAND, 5 );
	
	
	this->SetSizer( bSizer1 );
	this->Layout();
	
	this->Centre( wxBOTH );
}

BitmapGUI::~BitmapGUI()
{
}

TileMapGUI::TileMapGUI( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer28;
	bSizer28 = new wxBoxSizer( wxVERTICAL );
	
	m_splitter7 = new wxSplitterWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D );
	m_splitter7->Connect( wxEVT_IDLE, wxIdleEventHandler( TileMapGUI::m_splitter7OnIdle ), NULL, this );
	
	m_panel16 = new wxPanel( m_splitter7, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer29;
	bSizer29 = new wxBoxSizer( wxVERTICAL );
	
	m_splitter8 = new wxSplitterWindow( m_panel16, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D );
	m_splitter8->Connect( wxEVT_IDLE, wxIdleEventHandler( TileMapGUI::m_splitter8OnIdle ), NULL, this );
	
	tilePanel = new wxPanel( m_splitter8, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	palettePanel = new wxPanel( m_splitter8, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_splitter8->SplitHorizontally( tilePanel, palettePanel, 0 );
	bSizer29->Add( m_splitter8, 1, wxEXPAND, 5 );
	
	
	m_panel16->SetSizer( bSizer29 );
	m_panel16->Layout();
	bSizer29->Fit( m_panel16 );
	mapPanel = new wxPanel( m_splitter7, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_splitter7->SplitVertically( m_panel16, mapPanel, 0 );
	bSizer28->Add( m_splitter7, 1, wxEXPAND, 5 );
	
	
	this->SetSizer( bSizer28 );
	this->Layout();
	
	this->Centre( wxBOTH );
}

TileMapGUI::~TileMapGUI()
{
}

SpriteEditorGUI::SpriteEditorGUI( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer28;
	bSizer28 = new wxBoxSizer( wxVERTICAL );
	
	m_splitter7 = new wxSplitterWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D );
	m_splitter7->Connect( wxEVT_IDLE, wxIdleEventHandler( SpriteEditorGUI::m_splitter7OnIdle ), NULL, this );
	
	palettePanel = new wxPanel( m_splitter7, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	mapPanel = new wxPanel( m_splitter7, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_splitter7->SplitVertically( palettePanel, mapPanel, 0 );
	bSizer28->Add( m_splitter7, 1, wxEXPAND, 5 );
	
	
	this->SetSizer( bSizer28 );
	this->Layout();
	
	this->Centre( wxBOTH );
}

SpriteEditorGUI::~SpriteEditorGUI()
{
}
