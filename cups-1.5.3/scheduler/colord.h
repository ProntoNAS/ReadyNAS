/*
 * "$Id$"
 *
 *   colord integration for the CUPS scheduler.
 *
 *   Copyright 2011, Red Hat.
 *
 *   These coded instructions, statements, and computer programs are the
 *   property of Apple Inc. and are protected by Federal copyright
 *   law.  Distribution and use rights are outlined in the file "LICENSE.txt"
 *   which should have been included with this file.  If this file is
 *   file is missing or damaged, see the license at "http://www.cups.org/".
 */

void	colordRegisterPrinter(cupsd_printer_t *p);
void	colordUnregisterPrinter(cupsd_printer_t *p);
void	colordStart(void);
void	colordStop(void);

/*
 * End of "$Id$".
 */
