#ifndef TEST_UI_APP_H
#define TEST_UI_APP_H

#include <eikapp.h>

class CHXTestUIApp : public CEikApplication
{
private: 
    // Inherited from class CApaApplication
    CApaDocument* CreateDocumentL();
    TUid AppDllUid() const;
};

#endif /* TEST_UI_APP_H */
