#ifndef TEST_UI_APP_VIEW_H
#define TEST_UI_APP_VIEW_H

#include <coecntrl.h>

class CHXTestUIAppView : public CCoeControl
{
public:    
    static CHXTestUIAppView* NewL(const TRect& aRect);
    CHXTestUIAppView();
    ~CHXTestUIAppView();
    void ConstructL(const TRect& aRect);

    void RunTest();

private:
    // Inherited from CCoeControl
    void Draw(const TRect& /*aRect*/) const;
    
    HBufC*  m_testText;
};

#endif /* TEST_UI_APP_VIEW_H */
