/* loader.h -- Common definitions for Atari boot loader
 *
 * Copyright (C) 1997 Roman Hodek <Roman.Hodek@informatik.uni-erlangen.de>
 *
 * This program is free software.  You can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation: either version 2 or
 * (at your option) any later version.
 * 
 * $Id: loader.h,v 1.3 1998-02-26 10:19:57 rnhodek Exp $
 * 
 * $Log: loader.h,v $
 * Revision 1.3  1998-02-26 10:19:57  rnhodek
 * New option 'workdir' to exec_tos_program(), to implement new config
 * var 'WorkDir'.
 * New function bios_printf() for debugging {Read,Write}Sectors and the
 * hdv_* implementations in tmpmount.c. These are called by GEMDOS and
 * thus can't use GEMDOS functions.
 *
 * Revision 1.2  1997/09/19 09:06:58  geert
 * Big bunch of changes by Geert: make things work on Amiga; cosmetic things
 *
 * Revision 1.1  1997/08/12 15:27:10  rnhodek
 * Import of Amiga and newly written Atari lilo sources, with many mods
 * to separate out common parts.
 *
 * 
 */
#ifndef _loader_h
#define _loader_h

#include "loader_common.h"

#undef DEBUG_RW_SECTORS

extern unsigned int SerialPort;
extern unsigned int AutoBoot;
extern const char *Prompt;
extern unsigned int NoGUI;
extern const struct BootRecord *dflt_os;
extern int CurrentFloppy;
extern struct TagTmpMnt *MountPointList;
extern struct tmpmnt *MountPoints;

/***************************** Prototypes *****************************/

int is_available( const struct BootRecord *rec );
void boot_tos( const struct BootRecord *rec );
void boot_linux( const struct BootRecord *rec, const char *cmdline );
void boot_bootsector( const struct BootRecord *rec );
int exec_tos_program( const char *prog, const char *workdir );
const char *tos_perror( long err );
void MachInitDebug( void );
void Alert( enum AlertCodes code );
long WriteSectors( char *buf, int device, unsigned int sector, unsigned int cnt );
#ifdef DEBUG_RW_SECTORS
void bios_printf( const char *format, ... )
	__attribute__((format(printf,1,2)));
#endif

/************************* End of Prototypes **************************/


#endif  /* _loader_h */

