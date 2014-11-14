///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Nov 12 2014)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __BRANDONTOOLSEDITORGUI_H__
#define __BRANDONTOOLSEDITORGUI_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include <wx/string.h>
#include <wx/tglbtn.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/clrpicker.h>
#include <wx/stattext.h>
#include <wx/panel.h>
#include <wx/splitter.h>
#include <wx/frame.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class BitmapGUI
///////////////////////////////////////////////////////////////////////////////
class BitmapGUI : public wxFrame 
{
	private:
	
	protected:
		wxSplitterWindow* m_splitter1;
		wxPanel* m_panel1;
		wxSplitterWindow* m_splitter2;
		wxPanel* toolPanel;
		wxToggleButton* m_toggleBtn1;
		wxToggleButton* m_toggleBtn2;
		wxToggleButton* m_toggleBtn3;
		wxToggleButton* m_toggleBtn4;
		wxToggleButton* m_toggleBtn5;
		wxToggleButton* m_toggleBtn6;
		wxToggleButton* m_toggleBtn7;
		wxToggleButton* m_toggleBtn8;
		wxColourPickerCtrl* m_colourPicker1;
		wxStaticText* m_staticText1;
		wxColourPickerCtrl* m_colourPicker3;
		wxStaticText* m_staticText2;
		wxColourPickerCtrl* m_colourPicker2;
		wxStaticText* m_staticText3;
		wxPanel* palettePanel;
		wxPanel* drawingPanel;
	
	public:
		
		BitmapGUI( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("BitmapEditor"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
		
		~BitmapGUI();
		
		void m_splitter1OnIdle( wxIdleEvent& )
		{
			m_splitter1->SetSashPosition( 0 );
			m_splitter1->Disconnect( wxEVT_IDLE, wxIdleEventHandler( BitmapGUI::m_splitter1OnIdle ), NULL, this );
		}
		
		void m_splitter2OnIdle( wxIdleEvent& )
		{
			m_splitter2->SetSashPosition( 271 );
			m_splitter2->Disconnect( wxEVT_IDLE, wxIdleEventHandler( BitmapGUI::m_splitter2OnIdle ), NULL, this );
		}
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class TileMapGUI
///////////////////////////////////////////////////////////////////////////////
class TileMapGUI : public wxFrame 
{
	private:
	
	protected:
		wxSplitterWindow* m_splitter7;
		wxPanel* m_panel16;
		wxSplitterWindow* m_splitter8;
		wxPanel* tilePanel;
		wxPanel* palettePanel;
		wxPanel* mapPanel;
	
	public:
		
		TileMapGUI( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Tile Map Editor"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
		
		~TileMapGUI();
		
		void m_splitter7OnIdle( wxIdleEvent& )
		{
			m_splitter7->SetSashPosition( 0 );
			m_splitter7->Disconnect( wxEVT_IDLE, wxIdleEventHandler( TileMapGUI::m_splitter7OnIdle ), NULL, this );
		}
		
		void m_splitter8OnIdle( wxIdleEvent& )
		{
			m_splitter8->SetSashPosition( 0 );
			m_splitter8->Disconnect( wxEVT_IDLE, wxIdleEventHandler( TileMapGUI::m_splitter8OnIdle ), NULL, this );
		}
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class SpriteEditorGUI
///////////////////////////////////////////////////////////////////////////////
class SpriteEditorGUI : public wxFrame 
{
	private:
	
	protected:
		wxSplitterWindow* m_splitter7;
		wxPanel* palettePanel;
		wxPanel* mapPanel;
	
	public:
		
		SpriteEditorGUI( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Sprite Editor"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
		
		~SpriteEditorGUI();
		
		void m_splitter7OnIdle( wxIdleEvent& )
		{
			m_splitter7->SetSashPosition( 0 );
			m_splitter7->Disconnect( wxEVT_IDLE, wxIdleEventHandler( SpriteEditorGUI::m_splitter7OnIdle ), NULL, this );
		}
	
};

#endif //__BRANDONTOOLSEDITORGUI_H__
