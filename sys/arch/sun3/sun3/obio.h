/*
 * This file defines addresses in Type 1 space for various devices
 * which can be on the motherboard directly.
 *
 * Supposedly these values are constant across the entire sun3 architecture.
 *
 */


#define OBIO_KEYBD_MS     0x00000000
#define OBIO_ZS           0x00020000
#define OBIO_EEPROM       0x00040000
#define OBIO_CLOCK        0x00060000
#define OBIO_MEMERR       0x00080000
#define OBIO_INTERREG     0x000A0000
#define OBIO_INTEL_ETHER  0x000C0000
#define OBIO_COLOR_MAP    0x000E0000
#define OBIO_EPROM        0x00100000
#define OBIO_AMD_ETHER    0x00120000
#define OBIO_NCR_SCSI     0x00140000
#define OBIO_RESERVED1    0x00160000
#define OBIO_RESERVED2    0x00180000
#define OBIO_IOX_BUS      0x001A0000
#define OBIO_DES          0x001C0000
#define OBIO_ECCREG       0x001E0000

#define OBIO_KEYBD_MS_SIZE	0x00008
#define OBIO_ZS_SIZE		0x00008
#define OBIO_EEPROM_SIZE	0x00800
#define OBIO_CLOCK_SIZE		0x00020
#define OBIO_MEMERR_SIZE	0x00008			
#define OBIO_INTERREG_SIZE	0x00001			
#define OBIO_INTEL_ETHER_SIZE	0x00001			
#define OBIO_COLOR_MAP_SIZE	0x00400			
#define OBIO_EPROM_SIZE		0x10000		
#define OBIO_AMD_ETHER_SIZE	0x00004			
#define OBIO_NCR_SCSI_SIZE	0x00020			
#define OBIO_IO_BUS_SIZE      0x1000000			
#define OBIO_DES_SIZE		0x00004		
#define OBIO_ECCREG_SIZE	0x00100			

#ifdef KERNEL

extern char *interrupt_reg;

#endif
