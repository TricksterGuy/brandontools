///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Nov 12 2014)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "BrandonFrameGUI.h"

///////////////////////////////////////////////////////////////////////////

BrandonFrameGUI::BrandonFrameGUI( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	notebook = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNB_LEFT );
	settingsPanel = new wxPanel( notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* bSizer11b;
	bSizer11b = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 12, 2, 0, 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->AddGrowableRow( 9 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText8 = new wxStaticText( settingsPanel, wxID_ANY, _("Source Images"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText8->Wrap( -1 );
	fgSizer1->Add( m_staticText8, 0, wxALL, 8 );
	
	wxBoxSizer* bSizer15b;
	bSizer15b = new wxBoxSizer( wxHORIZONTAL );
	
	wxArrayString sourceImagesChoices;
	sourceImages = new wxChoice( settingsPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, sourceImagesChoices, 0 );
	sourceImages->SetSelection( 0 );
	bSizer15b->Add( sourceImages, 1, wxALL, 4 );
	
	m_button4 = new wxButton( settingsPanel, wxID_ANY, _("Browse"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer15b->Add( m_button4, 0, wxALL, 4 );
	
	
	fgSizer1->Add( bSizer15b, 1, wxEXPAND, 5 );
	
	m_staticText1 = new wxStaticText( settingsPanel, wxID_ANY, _("Symbol Name (Filename):"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	fgSizer1->Add( m_staticText1, 0, wxALL, 8 );
	
	filename = new wxTextCtrl( settingsPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( filename, 0, wxALL|wxEXPAND, 8 );
	
	m_staticText7 = new wxStaticText( settingsPanel, wxID_ANY, _("GBA Mode:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText7->Wrap( -1 );
	fgSizer1->Add( m_staticText7, 0, wxALL, 8 );
	
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxHORIZONTAL );
	
	
	bSizer8->Add( 0, 0, 1, wxEXPAND, 5 );
	
	wxString modeChoices[] = { _("Mode3"), _("Mode4"), _("Mode0"), _("Sprites") };
	int modeNChoices = sizeof( modeChoices ) / sizeof( wxString );
	mode = new wxChoice( settingsPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, modeNChoices, modeChoices, 0 );
	mode->SetSelection( 0 );
	mode->SetToolTip( _("Which GBA Mode you want.") );
	
	bSizer8->Add( mode, 0, wxALL, 5 );
	
	
	fgSizer1->Add( bSizer8, 1, wxEXPAND, 5 );
	
	m_staticText2 = new wxStaticText( settingsPanel, wxID_ANY, _("Resize Images:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	fgSizer1->Add( m_staticText2, 0, wxALL, 8 );
	
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxHORIZONTAL );
	
	
	bSizer5->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_staticText3 = new wxStaticText( settingsPanel, wxID_ANY, _("Width"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	bSizer5->Add( m_staticText3, 0, wxALL, 8 );
	
	width = new wxSpinCtrl( settingsPanel, wxID_ANY, wxT("-1"), wxDefaultPosition, wxSize( 64,-1 ), wxSP_ARROW_KEYS, -1, 1024, 1 );
	width->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 90, false, wxEmptyString ) );
	width->SetToolTip( _("Resize ALL images to this width") );
	
	bSizer5->Add( width, 0, wxALL, 5 );
	
	m_staticText4 = new wxStaticText( settingsPanel, wxID_ANY, _("Height"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	bSizer5->Add( m_staticText4, 0, wxALL, 8 );
	
	height = new wxSpinCtrl( settingsPanel, wxID_ANY, wxT("-1"), wxDefaultPosition, wxSize( 64,-1 ), wxSP_ARROW_KEYS, -1, 1024, 1 );
	height->SetToolTip( _("Resize ALL images to this height") );
	
	bSizer5->Add( height, 0, wxALL, 5 );
	
	
	fgSizer1->Add( bSizer5, 1, wxALIGN_RIGHT|wxEXPAND, 5 );
	
	m_staticText5 = new wxStaticText( settingsPanel, wxID_ANY, _("Start Index:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText5->Wrap( -1 );
	fgSizer1->Add( m_staticText5, 0, wxALL, 8 );
	
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxHORIZONTAL );
	
	
	bSizer6->Add( 0, 0, 1, wxEXPAND, 5 );
	
	startIndex = new wxSpinCtrl( settingsPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 255, 0 );
	startIndex->Enable( false );
	startIndex->SetToolTip( _("Start index for palette") );
	
	bSizer6->Add( startIndex, 0, wxALL, 5 );
	
	
	fgSizer1->Add( bSizer6, 1, wxEXPAND, 5 );
	
	m_staticText171 = new wxStaticText( settingsPanel, wxID_ANY, _("Palette Size:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText171->Wrap( -1 );
	fgSizer1->Add( m_staticText171, 0, wxALL, 8 );
	
	wxBoxSizer* bSizer18;
	bSizer18 = new wxBoxSizer( wxVERTICAL );
	
	paletteSize = new wxSpinCtrl( settingsPanel, wxID_ANY, wxT("255"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 255, 255 );
	paletteSize->Enable( false );
	
	bSizer18->Add( paletteSize, 0, wxALL|wxALIGN_RIGHT, 5 );
	
	
	fgSizer1->Add( bSizer18, 1, wxEXPAND, 5 );
	
	m_staticText1711 = new wxStaticText( settingsPanel, wxID_ANY, _("Bpp:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1711->Wrap( -1 );
	fgSizer1->Add( m_staticText1711, 0, wxALL, 8 );
	
	wxBoxSizer* bSizer181;
	bSizer181 = new wxBoxSizer( wxVERTICAL );
	
	wxString bppChoices[] = { _("4 Bits"), _("8 Bits") };
	int bppNChoices = sizeof( bppChoices ) / sizeof( wxString );
	bpp = new wxChoice( settingsPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, bppNChoices, bppChoices, 0 );
	bpp->SetSelection( 1 );
	bpp->Enable( false );
	
	bSizer181->Add( bpp, 0, wxALL|wxALIGN_RIGHT, 5 );
	
	
	fgSizer1->Add( bSizer181, 1, wxEXPAND, 5 );
	
	m_staticText21 = new wxStaticText( settingsPanel, wxID_ANY, _("Sprite Mode:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText21->Wrap( -1 );
	fgSizer1->Add( m_staticText21, 0, wxALL, 8 );
	
	wxBoxSizer* bSizer1811;
	bSizer1811 = new wxBoxSizer( wxVERTICAL );
	
	wxString spriteModeChoices[] = { _("1D"), _("2D") };
	int spriteModeNChoices = sizeof( spriteModeChoices ) / sizeof( wxString );
	spriteMode = new wxChoice( settingsPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, spriteModeNChoices, spriteModeChoices, 0 );
	spriteMode->SetSelection( 1 );
	spriteMode->Enable( false );
	
	bSizer1811->Add( spriteMode, 0, wxALL|wxALIGN_RIGHT, 5 );
	
	
	fgSizer1->Add( bSizer1811, 1, wxEXPAND, 5 );
	
	m_staticText151 = new wxStaticText( settingsPanel, wxID_ANY, _("Colorkey Image?"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText151->Wrap( -1 );
	fgSizer1->Add( m_staticText151, 0, wxALL, 8 );
	
	wxBoxSizer* bSizer15;
	bSizer15 = new wxBoxSizer( wxVERTICAL );
	
	colorkey = new wxCheckBox( settingsPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	colorkey->SetToolTip( _("Enable Transparent Color") );
	
	bSizer15->Add( colorkey, 0, wxALIGN_RIGHT|wxTOP|wxBOTTOM|wxRIGHT, 6 );
	
	
	fgSizer1->Add( bSizer15, 1, wxEXPAND, 5 );
	
	m_staticText141 = new wxStaticText( settingsPanel, wxID_ANY, _("Transparent Color:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText141->Wrap( -1 );
	fgSizer1->Add( m_staticText141, 0, wxALL, 8 );
	
	wxBoxSizer* bSizer99;
	bSizer99 = new wxBoxSizer( wxHORIZONTAL );
	
	
	bSizer99->Add( 0, 0, 1, wxEXPAND, 5 );
	
	transparent = new wxColourPickerCtrl( settingsPanel, wxID_ANY, *wxBLACK, wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE|wxCLRP_SHOW_LABEL );
	transparent->Enable( false );
	
	bSizer99->Add( transparent, 0, wxALL, 4 );
	
	
	fgSizer1->Add( bSizer99, 1, wxEXPAND, 5 );
	
	
	fgSizer1->Add( 0, 0, 1, wxEXPAND, 5 );
	
	
	fgSizer1->Add( 0, 0, 1, wxEXPAND, 5 );
	
	
	fgSizer1->Add( 0, 0, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer160;
	bSizer160 = new wxBoxSizer( wxHORIZONTAL );
	
	
	bSizer160->Add( 0, 0, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer171;
	bSizer171 = new wxBoxSizer( wxHORIZONTAL );
	
	command = new wxButton( settingsPanel, wxID_ANY, _("Command"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer171->Add( command, 0, wxALL, 5 );
	
	exportImage = new wxButton( settingsPanel, wxID_ANY, _("Export!"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer171->Add( exportImage, 0, wxALIGN_RIGHT|wxALL, 5 );
	
	
	bSizer160->Add( bSizer171, 0, 0, 5 );
	
	
	fgSizer1->Add( bSizer160, 1, wxEXPAND|wxALIGN_RIGHT, 5 );
	
	
	bSizer11b->Add( fgSizer1, 1, wxEXPAND, 5 );
	
	
	bSizer2->Add( bSizer11b, 1, wxEXPAND, 5 );
	
	
	settingsPanel->SetSizer( bSizer2 );
	settingsPanel->Layout();
	bSizer2->Fit( settingsPanel );
	notebook->AddPage( settingsPanel, _("Settings"), true );
	advancedPanel = new wxPanel( notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	advancedPanel->SetToolTip( _("Do not touch anything here unless you know what you are doing ;D") );
	
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 7, 3, 0, 0 );
	fgSizer2->AddGrowableCol( 1 );
	fgSizer2->SetFlexibleDirection( wxHORIZONTAL );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText11 = new wxStaticText( advancedPanel, wxID_ANY, _("Volume"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText11->Wrap( -1 );
	fgSizer2->Add( m_staticText11, 0, wxALL, 8 );
	
	
	fgSizer2->Add( 0, 0, 1, wxEXPAND, 5 );
	
	volume = new wxSpinCtrl( advancedPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 100, 25 );
	volume->SetToolTip( _("Parameter for Median Cut.  Controls how many boxes are split based on volume.") );
	
	fgSizer2->Add( volume, 0, wxALL, 4 );
	
	m_staticText12 = new wxStaticText( advancedPanel, wxID_ANY, _("Population"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText12->Wrap( -1 );
	fgSizer2->Add( m_staticText12, 0, wxALL, 8 );
	
	
	fgSizer2->Add( 0, 0, 1, wxEXPAND, 5 );
	
	population = new wxSpinCtrl( advancedPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 100, 25 );
	population->SetToolTip( _("Parameter for Median Cut.  Controls how many boxes are split based on color population.") );
	
	fgSizer2->Add( population, 0, wxALL, 4 );
	
	m_staticText13 = new wxStaticText( advancedPanel, wxID_ANY, _("Population*Volume"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText13->Wrap( -1 );
	fgSizer2->Add( m_staticText13, 0, wxALL, 8 );
	
	
	fgSizer2->Add( 0, 0, 1, wxEXPAND, 5 );
	
	popvolume = new wxSpinCtrl( advancedPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 100, 25 );
	popvolume->SetToolTip( _("Parameter for Median Cut.  Controls how many boxes are split based on the product of population and volume.") );
	
	fgSizer2->Add( popvolume, 0, wxALL, 4 );
	
	m_staticText14 = new wxStaticText( advancedPanel, wxID_ANY, _("Error"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText14->Wrap( -1 );
	fgSizer2->Add( m_staticText14, 0, wxALL, 8 );
	
	
	fgSizer2->Add( 0, 0, 1, wxEXPAND, 5 );
	
	error = new wxSpinCtrl( advancedPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 100, 25 );
	error->SetToolTip( _("Parameter for Median Cut.  Controls how many boxes are split based on error.") );
	
	fgSizer2->Add( error, 0, wxALL, 4 );
	
	m_staticText15 = new wxStaticText( advancedPanel, wxID_ANY, _("Dithering?"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText15->Wrap( -1 );
	fgSizer2->Add( m_staticText15, 0, wxALL, 8 );
	
	
	fgSizer2->Add( 0, 0, 1, wxEXPAND, 5 );
	
	dither = new wxCheckBox( advancedPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	dither->SetValue(true); 
	dither->SetToolTip( _("Enables or disables dithering") );
	
	fgSizer2->Add( dither, 0, wxALIGN_RIGHT|wxTOP|wxBOTTOM|wxRIGHT, 8 );
	
	m_staticText16 = new wxStaticText( advancedPanel, wxID_ANY, _("Dither Level"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText16->Wrap( -1 );
	fgSizer2->Add( m_staticText16, 0, wxALL, 8 );
	
	
	fgSizer2->Add( 0, 0, 1, wxEXPAND, 5 );
	
	ditherLevel = new wxSpinCtrl( advancedPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 100, 80 );
	ditherLevel->SetToolTip( _("Controls the error propogation for the dithering algorithm") );
	
	fgSizer2->Add( ditherLevel, 0, wxALL, 5 );
	
	
	advancedPanel->SetSizer( fgSizer2 );
	advancedPanel->Layout();
	fgSizer2->Fit( advancedPanel );
	notebook->AddPage( advancedPanel, _("Advanced"), false );
	imagesPanel = new wxPanel( notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer10;
	bSizer10 = new wxBoxSizer( wxHORIZONTAL );
	
	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( imagesPanel, wxID_ANY, _("Current") ), wxVERTICAL );
	
	beforeWindow = new wxScrolledWindow( imagesPanel, wxID_ANY, wxDefaultPosition, wxSize( 256,256 ), wxHSCROLL|wxVSCROLL );
	beforeWindow->SetScrollRate( 8, 8 );
	beforeWindow->SetMinSize( wxSize( 256,256 ) );
	beforeWindow->SetMaxSize( wxSize( 256,256 ) );
	
	sbSizer1->Add( beforeWindow, 1, wxEXPAND | wxALL, 0 );
	
	
	bSizer10->Add( sbSizer1, 0, 0, 0 );
	
	
	bSizer10->Add( 0, 0, 1, wxEXPAND, 0 );
	
	wxStaticBoxSizer* sbSizer2;
	sbSizer2 = new wxStaticBoxSizer( new wxStaticBox( imagesPanel, wxID_ANY, _("Exported") ), wxVERTICAL );
	
	afterWindow = new wxScrolledWindow( imagesPanel, wxID_ANY, wxDefaultPosition, wxSize( 256,256 ), wxHSCROLL|wxVSCROLL );
	afterWindow->SetScrollRate( 8, 8 );
	afterWindow->SetMinSize( wxSize( 256,256 ) );
	afterWindow->SetMaxSize( wxSize( 256,256 ) );
	
	sbSizer2->Add( afterWindow, 1, wxEXPAND | wxALL, 0 );
	
	
	bSizer10->Add( sbSizer2, 0, 0, 0 );
	
	
	bSizer3->Add( bSizer10, 0, 0, 0 );
	
	wxBoxSizer* bSizer14;
	bSizer14 = new wxBoxSizer( wxHORIZONTAL );
	
	previousImage = new wxButton( imagesPanel, wxID_ANY, _("<-"), wxDefaultPosition, wxDefaultSize, 0 );
	previousImage->SetToolTip( _("Previous image in image list.") );
	
	bSizer14->Add( previousImage, 0, wxALL, 5 );
	
	nextImage = new wxButton( imagesPanel, wxID_ANY, _("->"), wxDefaultPosition, wxDefaultSize, 0 );
	nextImage->SetToolTip( _("Next image in image list.") );
	
	bSizer14->Add( nextImage, 0, wxALL, 5 );
	
	
	bSizer3->Add( bSizer14, 0, wxEXPAND, 0 );
	
	wxStaticBoxSizer* sbSizer3;
	sbSizer3 = new wxStaticBoxSizer( new wxStaticBox( imagesPanel, wxID_ANY, _("Palette") ), wxVERTICAL );
	
	currentPalette = new wxStaticBitmap( imagesPanel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( 128,128 ), 0 );
	currentPalette->SetToolTip( _("Palette (Only applicable for Mode4/Mode0)") );
	currentPalette->SetMinSize( wxSize( 128,128 ) );
	currentPalette->SetMaxSize( wxSize( 128,128 ) );
	
	sbSizer3->Add( currentPalette, 0, wxALL, 0 );
	
	
	bSizer3->Add( sbSizer3, 0, 0, 0 );
	
	
	imagesPanel->SetSizer( bSizer3 );
	imagesPanel->Layout();
	bSizer3->Fit( imagesPanel );
	notebook->AddPage( imagesPanel, _("Images"), false );
	codePanel = new wxPanel( notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	codeSizer = new wxBoxSizer( wxVERTICAL );
	
	codeBook = new wxNotebook( codePanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	cCodePanel = new wxPanel( codeBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer16;
	bSizer16 = new wxBoxSizer( wxVERTICAL );
	
	cCode = new wxStyledTextCtrl(cCodePanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0);
	bSizer16->Add( cCode, 1, wxALL|wxEXPAND, 0 );
	
	
	cCodePanel->SetSizer( bSizer16 );
	cCodePanel->Layout();
	bSizer16->Fit( cCodePanel );
	codeBook->AddPage( cCodePanel, _("C Code"), true );
	hCodePanel = new wxPanel( codeBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer17;
	bSizer17 = new wxBoxSizer( wxVERTICAL );
	
	hCode = new wxStyledTextCtrl(hCodePanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0);
	bSizer17->Add( hCode, 1, wxALL|wxEXPAND, 0 );
	
	
	hCodePanel->SetSizer( bSizer17 );
	hCodePanel->Layout();
	bSizer17->Fit( hCodePanel );
	codeBook->AddPage( hCodePanel, _("H Code"), false );
	
	codeSizer->Add( codeBook, 1, wxEXPAND | wxALL, 0 );
	
	
	codePanel->SetSizer( codeSizer );
	codePanel->Layout();
	codeSizer->Fit( codePanel );
	notebook->AddPage( codePanel, _("Code"), false );
	
	bSizer1->Add( notebook, 1, wxEXPAND | wxALL, 0 );
	
	
	this->SetSizer( bSizer1 );
	this->Layout();
	
	this->Centre( wxBOTH );
	
	// Connect Events
	notebook->Connect( wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED, wxNotebookEventHandler( BrandonFrameGUI::OnChangePage ), NULL, this );
	sourceImages->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( BrandonFrameGUI::OnSelectSource ), NULL, this );
	m_button4->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BrandonFrameGUI::OnSourceFiles ), NULL, this );
	mode->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( BrandonFrameGUI::OnModeChange ), NULL, this );
	colorkey->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( BrandonFrameGUI::OnColorKey ), NULL, this );
	command->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BrandonFrameGUI::OnExportCommand ), NULL, this );
	exportImage->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BrandonFrameGUI::OnExport ), NULL, this );
	dither->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( BrandonFrameGUI::OnDither ), NULL, this );
	previousImage->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BrandonFrameGUI::OnPreviousImage ), NULL, this );
	nextImage->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BrandonFrameGUI::OnNextImage ), NULL, this );
}

BrandonFrameGUI::~BrandonFrameGUI()
{
	// Disconnect Events
	notebook->Disconnect( wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED, wxNotebookEventHandler( BrandonFrameGUI::OnChangePage ), NULL, this );
	sourceImages->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( BrandonFrameGUI::OnSelectSource ), NULL, this );
	m_button4->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BrandonFrameGUI::OnSourceFiles ), NULL, this );
	mode->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( BrandonFrameGUI::OnModeChange ), NULL, this );
	colorkey->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( BrandonFrameGUI::OnColorKey ), NULL, this );
	command->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BrandonFrameGUI::OnExportCommand ), NULL, this );
	exportImage->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BrandonFrameGUI::OnExport ), NULL, this );
	dither->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( BrandonFrameGUI::OnDither ), NULL, this );
	previousImage->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BrandonFrameGUI::OnPreviousImage ), NULL, this );
	nextImage->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BrandonFrameGUI::OnNextImage ), NULL, this );
	
}
