#include "config.h"
#include "pot.h"
#include <locale.h>

/*************************************************************************
 * if you want to turn off gettext without changing sources edit pot.h 
 *************************************************************************/

void gettexton(void)
{
#ifdef USE_GETTEXT
	setlocale(LC_ALL, "");
	bindtextdomain("quota", "/usr/share/locale");
	textdomain("quota");
#endif
}
