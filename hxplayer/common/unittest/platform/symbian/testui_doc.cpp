#include "testui_doc.h"
#include "testui_app_ui.h"

CHXTestUIDoc::CHXTestUIDoc(CEikApplication& aApp) :
    CEikDocument(aApp)
{}

CEikAppUi* CHXTestUIDoc::CreateAppUiL()
{
    return new(ELeave) CHXTestUIAppUI;
}
