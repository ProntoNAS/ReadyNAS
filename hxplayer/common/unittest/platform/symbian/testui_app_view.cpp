#include "testui_app_view.h"

#include <eikenv.h>

#include "testui.rsg"
#include "hx_simple_test_driver.h"

CHXTestUIAppView::CHXTestUIAppView()
{}

CHXTestUIAppView* CHXTestUIAppView::NewL(const TRect& aRect)
{
    CHXTestUIAppView* self = new(ELeave) CHXTestUIAppView();
    CleanupStack::PushL(self);
    self->ConstructL(aRect);
    CleanupStack::Pop();
    return self;
}


CHXTestUIAppView::~CHXTestUIAppView()
{
    delete m_testText;
}


void CHXTestUIAppView::ConstructL(const TRect& aRect)
{
    CEikonEnv* eikonEnv=CEikonEnv::Static();
    m_testText = eikonEnv->AllocReadResourceL(R_BLANK_TEXT);

    CreateWindowL();
    SetRect(aRect);
    ActivateL();
}

void CHXTestUIAppView::RunTest()
{
    CEikonEnv* eikonEnv=CEikonEnv::Static();

    delete m_testText;

    m_testText = eikonEnv->AllocReadResourceL(R_RUNNING_TEXT);
    DrawDeferred();

    CHXSimpleTestDriver testDriver;
    bool testResult = testDriver.Run();

    delete m_testText;

    if (testResult)
    {
	m_testText = eikonEnv->AllocReadResourceL(R_PASSED_TEXT);
    }
    else
    {
	m_testText = eikonEnv->AllocReadResourceL(R_FAILED_TEXT);
    }

    DrawDeferred();
}

void CHXTestUIAppView::Draw(const TRect& /*aRect*/) const
{
    CWindowGc&   gc = SystemGc();
    TRect        drawRect = Rect();
    const CFont* fontUsed;
    CEikonEnv*   eikonEnv = CEikonEnv::Static();
    
    gc.Clear();

    fontUsed = eikonEnv->TitleFont();
    gc.UseFont(fontUsed);

    TInt baselineOffset = (drawRect.Height() - fontUsed->HeightInPixels())/2; 
    gc.DrawText(*m_testText, drawRect, baselineOffset, 
		CGraphicsContext::ECenter, 0);
    gc.DiscardFont();
}
