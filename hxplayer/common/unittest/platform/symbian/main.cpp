
#include "./testui_app.h"
#include <stdlib.h>

EXPORT_C CApaApplication* NewApplication()
{
    return new CHXTestUIApp;
}

GLDEF_C TInt E32Dll(TDllReason)
{
    CloseSTDLIB();
    return KErrNone;
}
