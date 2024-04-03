#include "testui_app.h"
#include "testui_doc.h"

extern "C" const TUid KUidTestUI; 

CApaDocument* CHXTestUIApp::CreateDocumentL()
{
    return new (ELeave) CHXTestUIDoc(*this);
}

TUid CHXTestUIApp::AppDllUid() const
{
    return KUidTestUI;
}
