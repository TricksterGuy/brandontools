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
    frame->Show();

    return true;
}
