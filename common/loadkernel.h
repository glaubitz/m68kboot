#ifndef _loadkernel_h
#define _loadkernel_h

/***************************** Prototypes *****************************/

unsigned long open_kernel( const char *kernel_name );
int load_kernel( void *memptr );
void kernel_debug_infos( unsigned long base );
unsigned long load_ramdisk( const char *ramdisk_name );
void move_ramdisk( void *dst, unsigned long rd_size );

/************************* End of Prototypes **************************/

#endif  /* _loadkernel_h */

