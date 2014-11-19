#include "BrandonFrame.hpp"
#include <dlfcn.h>
#include <wx/msgdlg.h>
#include <wx/filedlg.h>


static void SetUpStyledText(wxStyledTextCtrl* text);
static void UpdateStyledText(wxStyledTextCtrl* text, const wxString& file);
static void Export(wxArrayString& files, wxString filename, int mode, int width, int height, int startIndex,
                   int paletteSize, int bpp, bool export_2d, bool isTransparent, wxColor transparent, bool dither,
                   int ditherLevel, int weights[4], bool hide = false);
wxString GetExportCommand(wxArrayString& files, wxString filename, int mode, int width, int height, int startIndex,
                          int paletteSize, int bpp, bool export_2d, bool isTransparent, wxColor transparent, bool dither,
                          int ditherLevel, int weights[4], bool hide = false);


wxString tempdir;
wxFileName tempfile;
wxString lastExport = wxEmptyString;

/** @brief BrandonFrame
  *
  * @todo: document this function
  */
BrandonFrame::BrandonFrame() : BrandonFrameGUI(0L)
{
    currentImageBefore = new wxStaticBitmap(beforeWindow, wxID_ANY, wxNullBitmap);
    currentImageAfter = new wxStaticBitmap(afterWindow, wxID_ANY, wxNullBitmap);
    tempdir = wxFileName::GetTempDir();
    tempfile = wxFileName(tempdir, "BrandonTools");
    mode = 3;
    SetUpStyledText(cCode);
    SetUpStyledText(hCode);
    volume->Enable(false);
    population->Enable(false);
    popvolume->Enable(false);
    error->Enable(false);
    dither->Enable(false);
    ditherLevel->Enable(false);
}

/** @brief ~BrandonFrame
  *
  * @todo: document this function
  */
BrandonFrame::~BrandonFrame()
{

}

/** @brief OnChangePage
  *
  * @todo: document this function
  */
void BrandonFrame::OnChangePage(wxNotebookEvent& event)
{
    if (notebook->GetPage(event.GetSelection()) == codePanel)
        DoUpdateCode();
    else if (notebook->GetPage(event.GetSelection()) == imagesPanel)
        DoUpdateImages();
    else if (notebook->GetPage(event.GetSelection()) == settingsPanel)
        DoUpdateSettings();
    event.Skip();
}

/** @brief OnSourceFiles
  *
  * @todo: document this function
  */
void BrandonFrame::OnSourceFiles(wxCommandEvent& event)
{
    wxFileDialog* dialog = new wxFileDialog(this, "Select files to export", wxEmptyString, wxEmptyString,
                                            wxEmptyString, wxFD_PREVIEW | wxFD_MULTIPLE | wxFD_OPEN |
                                            wxFD_FILE_MUST_EXIST);
    if (dialog->ShowModal() == wxID_OK)
    {
        sourceFiles.Clear();
        dialog->GetPaths(sourceFiles);
        if (sourceFileIndex >= (int)sourceFiles.size()) sourceFileIndex = 0;
        sourceImages->Set(sourceFiles);
        sourceImages->SetSelection(sourceFileIndex);
    }

    delete dialog;
}

/** @brief OnSelectSource
  *
  * @todo: document this function
  */
void BrandonFrame::OnSelectSource(wxCommandEvent& event)
{
    sourceFileIndex = event.GetSelection();
}

/** @brief OnModeChange
  *
  * @todo: document this function
  */
void BrandonFrame::OnModeChange(wxCommandEvent& event)
{
    int rawmode = event.GetSelection();
    switch(rawmode)
    {
        case 0:
            mode = 3;
            break;
        case 1:
            mode = 4;
            break;
        case 2:
            mode = 0;
            break;
        case 3:
            mode = 7;
            break;
    }
    startIndex->Enable(mode != 3);
    paletteSize->Enable(mode != 3);
    spriteMode->Enable(mode == 7);
    bpp->Enable(mode == 0 || mode == 7);
    volume->Enable(mode != 3);
    population->Enable(mode != 3);
    popvolume->Enable(mode != 3);
    error->Enable(mode != 3);
    dither->Enable(mode != 3);
    ditherLevel->Enable(mode != 3);
}

/** @brief OnDither
  *
  * @todo: document this function
  */
void BrandonFrame::OnDither(wxCommandEvent& event)
{
    ditherLevel->Enable(event.IsChecked());
}

/** @brief OnColorKey
  *
  * @todo: document this function
  */
void BrandonFrame::OnColorKey(wxCommandEvent& event)
{
    transparent->Enable(event.IsChecked());
}

/** @brief OnNextImage
  *
  * @todo: document this function
  */
void BrandonFrame::OnNextImage(wxCommandEvent& event)
{
    sourceFileIndex = (sourceFileIndex + 1) % sourceFiles.size();
    sourceImages->SetSelection(sourceFileIndex);
    DoUpdateImages();
}

/** @brief OnPreviousImage
  *
  * @todo: document this function
  */
void BrandonFrame::OnPreviousImage(wxCommandEvent& event)
{
    sourceFileIndex = (sourceFileIndex - 1) % sourceFiles.size();
    sourceImages->SetSelection(sourceFileIndex);
    DoUpdateImages();
}

/** @brief OnExport
  *
  * @todo: document this function
  */
void BrandonFrame::OnExport(wxCommandEvent& event)
{
    if (filename->IsEmpty())
    {
        wxMessageBox(_("Need to specify filename before exporting!"), _("ERROR"));
        return;
    }
    DoExport(filename->GetValue(), false);
}



/** @brief OnExportCommand
  *
  * @todo: document this function
  */
void BrandonFrame::OnExportCommand(wxCommandEvent& event)
{
    if (filename->IsEmpty())
    {
        wxMessageBox(_("Need to specify filename before exporting!"), _("ERROR"));
        return;
    }
    int weights[4];
    weights[0] = volume->GetValue();
    weights[1] = population->GetValue();
    weights[2] = popvolume->GetValue();
    weights[3] = error->GetValue();
    wxString command = GetExportCommand(sourceFiles, filename->GetValue(), mode, width->GetValue(), height->GetValue(), startIndex->GetValue(),
                                        paletteSize->GetValue(), bpp->GetSelection() == 0 ? 4 : 8, spriteMode->GetSelection(),
                                        colorkey->IsChecked(), transparent->GetColour(), dither->IsChecked(), ditherLevel->GetValue(), weights, false);
    wxMessageBox(command, _("Export Command"));
}

/** @brief DoUpdateCode
  *
  * @todo: document this function
  */
void BrandonFrame::DoUpdateCode(void)
{
    if (sourceFiles.size() == 0) return;

    DoExport(tempfile.GetFullPath());
    UpdateStyledText(cCode, tempfile.GetFullPath() + _(".c"));
    UpdateStyledText(hCode, tempfile.GetFullPath() + _(".h"));
}


/** @brief DoUpdateSettings
  *
  * @todo: document this function
  */
void BrandonFrame::DoUpdateSettings(void)
{
    sourceImages->SetSelection(sourceFileIndex);
}

/** @brief DoUpdateImages
  *
  * @todo: document this function
  */
void BrandonFrame::DoUpdateImages(void)
{
    if (sourceFiles.size() == 0) return;

    wxFileName srcFile = sourceFiles[sourceFileIndex];


    wxImage image(srcFile.GetFullPath());
    if (!image.IsOk()) return;
    currentImageBefore->SetBitmap(wxBitmap(image));
    beforeWindow->SetVirtualSize(image.GetWidth(), image.GetHeight());

    int width = this->width->GetValue();
    int height = this->height->GetValue();
    if (width == -1) width = image.GetWidth();
    if (height == -1) height = image.GetHeight();


    DoExport(tempfile.GetFullPath());

    wxFileName tempsofile = tempfile;
    tempsofile.SetExt("so");

    // Compile the file.
    DoCompile(tempfile.GetFullPath());
    afterWindow->SetVirtualSize(width, height);
    DoUpdateExportedImages(tempsofile.GetFullPath(), sourceFileIndex, tempfile.GetName() + "_palette", width, height);
}

void BrandonFrame::DoExport(const wxString& exportFilename, bool hide)
{
    int weights[4];
    weights[0] = volume->GetValue();
    weights[1] = population->GetValue();
    weights[2] = popvolume->GetValue();
    weights[3] = error->GetValue();
    Export(sourceFiles, exportFilename, mode, width->GetValue(), height->GetValue(), startIndex->GetValue(),
           paletteSize->GetValue(), bpp->GetSelection() == 0 ? 4 : 8, spriteMode->GetSelection(),
           colorkey->IsChecked(), transparent->GetColour(), dither->IsChecked(), ditherLevel->GetValue(), weights, hide);
}

/** @brief DoCompile
  *
  * @todo: document this function
  */
void BrandonFrame::DoCompile(const wxString& file)
{
    wxString compileCommand = wxString::Format("gcc -shared %s.c -o %s.so", file, file);
    wxExecute(compileCommand, wxEXEC_SYNC);
}

/** @brief DoUpdateExportedImages
  *
  * @todo: document this function
  */
void BrandonFrame::DoUpdateExportedImages(const wxString& sofile, int id, const wxString& palette, int width, int height)
{
    void* odata = dlopen(sofile.c_str(), RTLD_LAZY | RTLD_GLOBAL);
    if (odata == NULL)
    {
         wxMessageBox("Wasn't able to open the generated files to display what it would look like on the gba", _("Failure"));
         return;
    }
    wxString image = wxString::Format("image%d", id);

    if (mode == 3)
    {
        unsigned short* data = (unsigned short*) dlsym(odata, image.c_str());
        unsigned char* rgbdata = (unsigned char*) malloc(sizeof(char) * 3 * width * height);
        for (int i = 0; i < width * height; i++)
        {
            rgbdata[3 * i + 0] = (data[i] & 0x1F) * 255 / 31;
            rgbdata[3 * i + 1] = ((data[i] >> 5) & 0x1F) * 255 / 31;
            rgbdata[3 * i + 2] = ((data[i] >> 10) & 0x1F) * 255 / 31;
        }
        wxImage processed(width, height, rgbdata, false);
        currentImageAfter->SetBitmap(wxBitmap(processed));
        currentPalette->SetBitmap(wxNullBitmap);
        dlclose(odata);
        return;
    }

    unsigned short* palette_data = (unsigned short*) dlsym(odata, palette.c_str());
    wxImage paletteImage(16, 16);
    for (int i = 0; i < 256; i++)
    {
        unsigned short c1 = palette_data[i];
        int x = i % 16;
        int y = i / 16;
        paletteImage.SetRGB(x, y, (c1 & 0x1F) * 255 / 31, ((c1 >> 5) & 0x1F) * 255 / 31, ((c1 >> 10) & 0x1F) * 255 / 31);
    }
    paletteImage.Rescale(128, 128);
    currentPalette->SetBitmap(wxBitmap(paletteImage));

    if (mode == 4)
    {
        unsigned short* data = (unsigned short*) dlsym(odata, image.c_str());
        unsigned char* rgbdata = (unsigned char*) malloc(sizeof(char) * 3 * width * height);
        for (int i = 0; i < width * height; i += 2)
        {
            unsigned short c1, c2;
            c1 = palette_data[data[i / 2] & 0xFF];
            c2 = palette_data[(data[i / 2] >> 8) & 0xFF];

            rgbdata[3 * i + 0] = (c1 & 0x1F) * 255 / 31;
            rgbdata[3 * i + 1] = ((c1 >> 5) & 0x1F) * 255 / 31;
            rgbdata[3 * i + 2] = ((c1 >> 10) & 0x1F) * 255 / 31;

            rgbdata[3 * i + 3] = (c2 & 0x1F) * 255 / 31;
            rgbdata[3 * i + 4] = ((c2 >> 5) & 0x1F) * 255 / 31;
            rgbdata[3 * i + 5] = ((c2 >> 10) & 0x1F) * 255 / 31;
        }

        wxImage processed(width, height, rgbdata, false);
        currentImageAfter->SetBitmap(wxBitmap(processed));
        afterWindow->SetVirtualSize(width, height);
    }
    else
    {
        wxMessageBox(_("Viewing of tile/sprite data not supported at this time, come back after Thanksgiving!"), _("Error"));
    }


    dlclose(odata);
}

static void SetUpStyledText(wxStyledTextCtrl* text)
{
    text->SetMarginWidth (MARGIN_LINE_NUMBERS, 56);
    text->StyleSetForeground (wxSTC_STYLE_LINENUMBER, wxColour (75, 75, 75) );
    text->StyleSetBackground (wxSTC_STYLE_LINENUMBER, wxColour (220, 220, 220));
    text->SetMarginType (MARGIN_LINE_NUMBERS, wxSTC_MARGIN_NUMBER);


    /*// ---- Enable code folding
    text->SetMarginType (MARGIN_FOLD, wxSTC_MARGIN_SYMBOL);
    text->SetMarginWidth(MARGIN_FOLD, 15);
    text->SetMarginMask (MARGIN_FOLD, wxSTC_MASK_FOLDERS);
    text->StyleSetBackground(MARGIN_FOLD, wxColor(200, 200, 200) );
    text->SetMarginSensitive(MARGIN_FOLD, true);

    // Properties found from http://www.scintilla.org/SciTEDoc.html
    text->SetProperty (wxT("fold"),         wxT("1") );
    text->SetProperty (wxT("fold.comment"), wxT("1") );
    text->SetProperty (wxT("fold.compact"), wxT("1") );

    wxColor grey( 100, 100, 100 );
    text->MarkerDefine (wxSTC_MARKNUM_FOLDER, wxSTC_MARK_ARROW );
    text->MarkerSetForeground (wxSTC_MARKNUM_FOLDER, grey);
    text->MarkerSetBackground (wxSTC_MARKNUM_FOLDER, grey);

    text->MarkerDefine (wxSTC_MARKNUM_FOLDEROPEN,    wxSTC_MARK_ARROWDOWN);
    text->MarkerSetForeground (wxSTC_MARKNUM_FOLDEROPEN, grey);
    text->MarkerSetBackground (wxSTC_MARKNUM_FOLDEROPEN, grey);

    text->MarkerDefine (wxSTC_MARKNUM_FOLDERSUB,     wxSTC_MARK_EMPTY);
    text->MarkerSetForeground (wxSTC_MARKNUM_FOLDERSUB, grey);
    text->MarkerSetBackground (wxSTC_MARKNUM_FOLDERSUB, grey);

    text->MarkerDefine (wxSTC_MARKNUM_FOLDEREND,     wxSTC_MARK_ARROW);
    text->MarkerSetForeground (wxSTC_MARKNUM_FOLDEREND, grey);
    text->MarkerSetBackground (wxSTC_MARKNUM_FOLDEREND, _T("WHITE"));

    text->MarkerDefine (wxSTC_MARKNUM_FOLDEROPENMID, wxSTC_MARK_ARROWDOWN);
    text->MarkerSetForeground (wxSTC_MARKNUM_FOLDEROPENMID, grey);
    text->MarkerSetBackground (wxSTC_MARKNUM_FOLDEROPENMID, _T("WHITE"));

    text->MarkerDefine (wxSTC_MARKNUM_FOLDERMIDTAIL, wxSTC_MARK_EMPTY);
    text->MarkerSetForeground (wxSTC_MARKNUM_FOLDERMIDTAIL, grey);
    text->MarkerSetBackground (wxSTC_MARKNUM_FOLDERMIDTAIL, grey);

    text->MarkerDefine (wxSTC_MARKNUM_FOLDERTAIL,    wxSTC_MARK_EMPTY);
    text->MarkerSetForeground (wxSTC_MARKNUM_FOLDERTAIL, grey);
    text->MarkerSetBackground (wxSTC_MARKNUM_FOLDERTAIL, grey);
    // ---- End of code folding part*/

    text->SetWrapMode (wxSTC_WRAP_NONE); // other choice is wxSCI_WRAP_NONE

    text->StyleClearAll();
    text->ClearAll();
    text->SetLexer(wxSTC_LEX_CPP);
    text->StyleSetForeground (wxSTC_C_STRING,            wxColour(160,0,160));
    text->StyleSetFaceName(wxSTC_C_STRING, _("Courier New"));
    text->StyleSetForeground (wxSTC_C_PREPROCESSOR,      wxColour(160,160,0));
    text->StyleSetFaceName(wxSTC_C_PREPROCESSOR, _("Courier New"));
    text->StyleSetForeground (wxSTC_C_IDENTIFIER,        wxColour(0,0,0));
    text->StyleSetFaceName(wxSTC_C_IDENTIFIER, _("Courier New"));
    text->StyleSetForeground (wxSTC_C_NUMBER,            wxColour(255,0,0));
    text->StyleSetFaceName(wxSTC_C_NUMBER, _("Courier New"));
    text->StyleSetForeground (wxSTC_C_CHARACTER,         wxColour(160,0,0));
    text->StyleSetFaceName(wxSTC_C_CHARACTER, _("Courier New"));
    text->StyleSetForeground (wxSTC_C_WORD,              wxColour(0,0,255));
    text->StyleSetFaceName(wxSTC_C_WORD, _("Courier New"));
    text->StyleSetForeground (wxSTC_C_WORD2,             wxColour(0,0,255));
    text->StyleSetFaceName(wxSTC_C_WORD2, _("Courier New"));
    text->StyleSetForeground (wxSTC_C_COMMENT,           wxColour(0,160,0));
    text->StyleSetFaceName(wxSTC_C_COMMENT, _("Courier New"));
    text->StyleSetForeground (wxSTC_C_COMMENTLINE,       wxColour(0,160,0));
    text->StyleSetFaceName(wxSTC_C_COMMENTLINE, _("Courier New"));
    text->StyleSetForeground (wxSTC_C_COMMENTDOC,        wxColour(0,160,0));
    text->StyleSetFaceName(wxSTC_C_COMMENTDOC, _("Courier New"));
    text->StyleSetForeground (wxSTC_C_COMMENTDOCKEYWORD, wxColour(160,0,0));
    text->StyleSetFaceName(wxSTC_C_COMMENTDOCKEYWORD, _("Courier New"));
    text->StyleSetForeground(wxSTC_C_COMMENTDOCKEYWORDERROR, wxColour(80,0,0));
    text->StyleSetFaceName(wxSTC_C_COMMENTDOCKEYWORDERROR, _("Courier New"));

    text->SetKeyWords(0, wxT("return for while break continue"));
    text->SetKeyWords(1, wxT("const int short extern signed unsigned float void char double"));

    text->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 90, false, wxT("Courier New") ) );
}

void UpdateStyledText(wxStyledTextCtrl* text, const wxString& file)
{
    text->SetReadOnly(false);
    text->ClearAll();
    text->LoadFile(file);
    text->SetReadOnly(true);
    text->SetSelection(0, 0);
}

void Export(wxArrayString& files, wxString filename, int mode, int width, int height, int startIndex,
                          int paletteSize, int bpp, bool export_2d, bool isTransparent, wxColor transparent, bool dither,
                          int ditherLevel, int weights[4], bool hide)
{
    wxString execute = GetExportCommand(files, filename, mode, width, height, startIndex, paletteSize, bpp, export_2d, isTransparent,
                                        transparent, dither, ditherLevel, weights, hide);

    if (execute == lastExport) return;

    lastExport = execute;
    wxArrayString stdout;
    wxPuts(execute);
    wxExecute(execute, stdout, wxEXEC_SYNC);
}

wxString GetExportCommand(wxArrayString& files, wxString filename, int mode, int width, int height, int startIndex,
                          int paletteSize, int bpp, bool export_2d, bool isTransparent, wxColor transparent, bool dither,
                          int ditherLevel, int weights[4], bool hide)
{
    wxString command;

#ifdef __WXMSW__
    command += _("brandontools.exe");
#else
    command += _("brandontools");
#endif

    // Mode
    command += (mode == 3 || mode == 4 || mode == 0) ? wxString::Format(" -mode%d", mode) : _(" -sprites");

    // Special
    command += hide ? _(" -hide -fullpalette") : _("");
    command += (width == -1 || height == -1) ? _("") : wxString::Format(" -resize=%d,%d", width, height);
    command += (mode == 3 || startIndex == 0) ? _("") : wxString::Format(" -start=%d", startIndex);
    command += (mode == 3 || paletteSize == 255) ? _("") : wxString::Format(" -palette=%d", paletteSize);
    command += isTransparent ? wxString::Format(" -transparent=%d,%d,%d", transparent.Red(), transparent.Green(), transparent.Blue()) : _("");
    command += (mode == 3 || (weights[0] == 25 && weights[1] == 25 && weights[2] == 25 && weights[3] == 25)) ? _("") :
                wxString::Format(" -weights=%d,%d,%d,%d", weights[0], weights[1], weights[2], weights[3]);
    command += (mode == 3 || dither) ? _("") : wxString::Format(" -dither=%d", dither);
    command += (mode == 3 || ditherLevel == 80) ? _("") : wxString::Format(" -dither_level=%d", ditherLevel);
    command += (mode == 0 || mode == 7) ? wxString::Format(" -bpp=%d", bpp) : _("");
    command += (mode == 7 && export_2d) ? wxString::Format(" -export_2d") : _("");

    // Hide is used when doing a temp export.
    if (hide)
    {
        command += " -names=";
        for (unsigned int i = 0; i < files.size() - 1; i++)
            command += wxString::Format("image%d,", (int)i);
        command += wxString::Format("image%d", (int)files.size() - 1);
    }

    command += wxString::Format(" %s ", filename);

    for (unsigned int i = 0; i < files.size(); i++)
        command += "\"" + files[i] + "\" ";

    return command;

}
