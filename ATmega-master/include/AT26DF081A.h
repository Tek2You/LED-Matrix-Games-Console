#ifndef _AT26DF081A_CLARKDAWG
#define _AT26DF081A_CLARKDAWG

#include <inttypes.h>

/* Serial Flash Commands */
// Read commands
#define READ_ARRAY					0x0B
#define READ_ARRAY_LOW_FREQUENCY	0x03
// Program and erase commands
#define BLOCK_ERASE_4K				0x20
#define BLOCK_ERASE_32K				0x52
#define BLOCK_ERASE_64K				0xD8
#define CHIP_ERASE					0x60
#define CHIP_ERASE_ALT				0xC7
#define BYTE_PAGE_PROGRAM			0x02
#define SEQUENTIAL_PROGRAM			0xAD
#define SEQUENTIAL_PROGRAM_ALT		0xAF
// Protection commands
#define WRITE_ENABLE				0x06
#define WRITE_DISABLE				0x04
#define PROTECT_SECTOR				0x36
#define UNPROTECT_SECTOR			0x39
// Status register commands
#define READ_STATUS_REGISTER		0x05
#define WRITE_STATUS_REGISTER		0x01
// Miscellaneous commands
#define READ_MFG_AND_DEVICE_ID		0x9F
#define DEEP_POWER_DOWN				0xB9
#define RESUME_FROM_SLEEP			0xAB


void flashWriteAddress( uint32_t addr, uint8_t sendDummy );
void flashReadArray( uint32_t addr, uint8_t *buffer, uint32_t numBytes );
void flashWriteArray( uint32_t addr, uint8_t *buffer, uint16_t numBytes );
void flashEraseBlock( uint32_t addr, uint8_t blockSize );
void flashEraseChip();
void flashWriteEnable();
void flashWriteDisable();
uint8_t flashReadStatusRegister();
void flashWriteStatusRegister( uint8_t status );
void flashReadMfgDeviceID( uint8_t *mfgID, uint16_t *devID );
void flashDeepSleep();
void flashWakeUp();
uint8_t flashReady();


#endif // _AT26DF081A_CLARKDAWG
