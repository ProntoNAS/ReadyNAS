#include "testui_app_ui.h"

#include <eikon.hrh>
#include <eikmenup.h>

#include "testui_app_view.h"
#include "testui.hrh"
#include "testui.rsg"

void CHXTestUIAppUI::ConstructL()
{
    BaseConstructL();

    m_pAppView = CHXTestUIAppView::NewL(ClientRect());
}

CHXTestUIAppUI::~CHXTestUIAppUI()
{
    delete m_pAppView;
}

void CHXTestUIAppUI::HandleCommandL(TInt aCommand)
{
    switch (aCommand) {
    case ETestUICmdRunTest:
	m_pAppView->RunTest();
	break;
    case EAknSoftkeyExit: 
	Exit();
	break;
    }
}

void CHXTestUIAppUI::DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane)
{
}
