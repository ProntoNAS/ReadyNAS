#line 450 "ifupdown.nw"
#include <stdlib.h>
#include "header.h"

#line 3870 "ifupdown.nw"
extern address_family addr_inet;
#line 4050 "ifupdown.nw"
extern address_family addr_inet6;
#line 4134 "ifupdown.nw"
extern address_family addr_ipx;

#line 455 "ifupdown.nw"
address_family *addr_fams[] = {
	
#line 3874 "ifupdown.nw"
&addr_inet, 
#line 4054 "ifupdown.nw"
&addr_inet6,
#line 4138 "ifupdown.nw"
&addr_ipx,
#line 457 "ifupdown.nw"
	NULL
};
