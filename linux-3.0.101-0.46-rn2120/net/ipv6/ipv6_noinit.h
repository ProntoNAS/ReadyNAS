/*
 * See somment in ipv6.c
 * Prevents placing anything in the .init.text section because we will need
 * it after the module is loaded.
*/

#undef __init
#define __init
#undef __initdata
#define __initdata
