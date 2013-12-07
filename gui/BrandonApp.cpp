#include "BrandonApp.hpp"
#include "BrandonFrame.hpp"
#include <wx/sysopt.h>
#include <wx/image.h>
#include <wx/gdicmn.h>

IMPLEMENT_APP(BrandonApp);

/** @brief OnInit
  *
  * @todo: document this function
  */
bool BrandonApp::OnInit()
{
    wxInitAllImageHandlers();

    BrandonFrame* frame = new BrandonFrame();
#ifdef __WXMSW__
    frame->SetIcon(wxICON(aaaa)); // To Set App Icon
#endif
    frame->Show();

    return true;
}
