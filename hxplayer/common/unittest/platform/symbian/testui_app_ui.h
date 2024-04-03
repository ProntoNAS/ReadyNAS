#ifndef TEST_UI_APP_UI_H
#define TEST_UI_APP_UI_H

#include <aknappui.h>
#include "testui_app_view.h"

class CHXTestUIAppUI : public CAknAppUi
{
    virtual ~CHXTestUIAppUI();

private:
    void ConstructL();
    void HandleCommandL(TInt aCommand);
    
    void DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane);

    CHXTestUIAppView* m_pAppView;
};

#endif /* TEST_UI_APP_UI_H */
