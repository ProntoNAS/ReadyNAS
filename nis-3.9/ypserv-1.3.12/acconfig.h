/* Ultrix xdr_functions in libc prevents linking */
#undef ULTRIX_KLUDGE

/* OSF1 does not want to take the address of structs */
#undef OSF_KLUDGE

/* Define if the systems has the resultproc_t type. */
#undef HAVE_RESULTPROC_T

/* Define if you have the res_gethostbyname function.  */
#undef HAVE_RES_GETHOSTBYNAME

/* Define if you wish to use the tcp wrapper (-lwrap).  */
#undef HAVE_LIBWRAP

/* Define if you have the gdbm library (-lgdbm).  */
#undef HAVE_LIBGDBM

/* Define if you have ndbm support.  */
#undef HAVE_NDBM
