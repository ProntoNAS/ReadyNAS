#line 430 "ifupdown.nw"
#include <stdlib.h>
#include "header.h"

#line 3509 "ifupdown.nw"
extern address_family addr_inet;
#line 3651 "ifupdown.nw"
extern address_family addr_inet6;
#line 3713 "ifupdown.nw"
extern address_family addr_ipx;

#line 435 "ifupdown.nw"
address_family *addr_fams[] = {
	
#line 3513 "ifupdown.nw"
&addr_inet, 
#line 3655 "ifupdown.nw"
&addr_inet6,
#line 3717 "ifupdown.nw"
&addr_ipx,
#line 437 "ifupdown.nw"
	NULL
};
