#include "BrandonFrameGUI.h"
#include <wx/stc/stc.h>

class BrandonFrame : public BrandonFrameGUI
{
    public:
        BrandonFrame();
        ~BrandonFrame();
    private:
        void OnChangePage(wxNotebookEvent& event);
		void OnColorKey(wxCommandEvent& event);
		void OnDither(wxCommandEvent& event);

		void OnSourceFiles(wxCommandEvent& event);
		void OnSelectSource(wxCommandEvent& event);
		void OnModeChange(wxCommandEvent& event);
		void OnExport(wxCommandEvent& event);
		void DoExport(const wxString& exportFilename, bool hide = true);
		void OnExportCommand( wxCommandEvent& event );
		void OnPreviousImage(wxCommandEvent& event);
		void OnNextImage(wxCommandEvent& event);


		void DoUpdateCode(void);
		void DoUpdateImages(void);
		void DoUpdateSettings(void);
		void DoCompile(const wxString& file);
		void DoUpdateExportedImages(const wxString& sofile, int id, const wxString& palette, int width, int height);

		wxArrayString sourceFiles;
		int mode;
		int sourceFileIndex;
		wxStaticBitmap* currentImageBefore;
		wxStaticBitmap* currentImageAfter;
};

enum
{
    MARGIN_LINE_NUMBERS,
    MARGIN_FOLD
};

