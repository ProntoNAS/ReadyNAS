#ifndef TEST_UI_DOC_H
#define TEST_UI_DOC_H

#include <eikdoc.h>

class CHXTestUIDoc : public CEikDocument
{
public:
	CHXTestUIDoc(CEikApplication& aApp);
private: 
	CEikAppUi* CreateAppUiL();
};

#endif /* TEST_UI_DOC_H */
