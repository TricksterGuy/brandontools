///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __BrandonFrameGUI__
#define __BrandonFrameGUI__

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/choice.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/filepicker.h>
#include <wx/spinctrl.h>
#include <wx/checkbox.h>
#include <wx/clrpicker.h>
#include <wx/panel.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/scrolwin.h>
#include <wx/statbox.h>
#include <wx/statbmp.h>
#include <wx/stc/stc.h>
#include <wx/notebook.h>
#include <wx/frame.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class BrandonFrameGUI
///////////////////////////////////////////////////////////////////////////////
class BrandonFrameGUI : public wxFrame 
{
	private:
	
	protected:
		wxNotebook* notebook;
		wxPanel* settingsPanel;
		wxStaticText* m_staticText8;
		wxChoice* sourceImages;
		wxButton* m_button4;
		wxStaticText* m_staticText1;
		wxFilePickerCtrl* filename;
		wxStaticText* m_staticText7;
		
		wxChoice* mode;
		wxStaticText* m_staticText2;
		
		wxStaticText* m_staticText3;
		wxSpinCtrl* width;
		wxStaticText* m_staticText4;
		wxSpinCtrl* height;
		wxStaticText* m_staticText5;
		
		wxSpinCtrl* startIndex;
		wxStaticText* m_staticText151;
		wxCheckBox* colorkey;
		wxStaticText* m_staticText141;
		
		wxColourPickerCtrl* transparent;
		
		
		
		
		wxButton* command;
		wxButton* exportImage;
		wxPanel* advancedPanel;
		wxStaticText* m_staticText11;
		
		wxSpinCtrl* volume;
		wxStaticText* m_staticText12;
		
		wxSpinCtrl* population;
		wxStaticText* m_staticText13;
		
		wxSpinCtrl* popvolume;
		wxStaticText* m_staticText14;
		
		wxSpinCtrl* error;
		wxStaticText* m_staticText15;
		
		wxCheckBox* dither;
		wxStaticText* m_staticText16;
		
		wxSpinCtrl* ditherLevel;
		wxStaticText* m_staticText17;
		
		wxCheckBox* useGimp;
		wxPanel* imagesPanel;
		wxScrolledWindow* beforeWindow;
		
		wxScrolledWindow* afterWindow;
		wxButton* previousImage;
		wxButton* nextImage;
		wxStaticBitmap* currentPalette;
		wxPanel* codePanel;
		wxBoxSizer* codeSizer;
		wxNotebook* codeBook;
		wxPanel* cCodePanel;
		wxStyledTextCtrl* cCode;
		wxPanel* hCodePanel;
		wxStyledTextCtrl* hCode;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnChangePage( wxNotebookEvent& event ) { event.Skip(); }
		virtual void OnSelectSource( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnSourceFiles( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnFilename( wxFileDirPickerEvent& event ) { event.Skip(); }
		virtual void OnModeChange( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnColorKey( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnExportCommand( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnExport( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnDither( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnPreviousImage( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnNextImage( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		BrandonFrameGUI( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("BrandonTools"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 624,528 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
		~BrandonFrameGUI();
	
};

#endif //__BrandonFrameGUI__
