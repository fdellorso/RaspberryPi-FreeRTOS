//
// bcmvendor.h
//
#ifndef _rsta_bt_bcmvendor_h
#define _rsta_bt_bcmvendor_h

#include <rsta_bt/bluetooth.h>

// Vendor specific commands
#define OP_CODE_DOWNLOAD_MINIDRIVER	(OGF_VENDOR_COMMANDS | 0x02E)
#define OP_CODE_WRITE_RAM			(OGF_VENDOR_COMMANDS | 0x04C)
#define OP_CODE_LAUNCH_RAM			(OGF_VENDOR_COMMANDS | 0x04E)

typedef struct TBTHCIBcmVendorCommand
{
	TBTHCICommandHeader	Header;

	u8	Data[255];
}
TBTHCIBcmVendorCommand
PACKED;

#endif
