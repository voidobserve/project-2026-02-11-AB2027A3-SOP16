#include "config.h"
depend(0x01010200);
setflash(1, FLASH_SIZE, FLASH_ERASE_4K, FLASH_DUAL_READ, FLASH_QUAD_READ);
setdataseg(0x1000);
setspace(FLASH_RESERVE_SIZE);
#if (FLASH_SIZE == FSIZE_256K)
setunpack(unpack.bin);
setpkgarea(0x23000, 0x19000);
#endif
make(dcf_buf, header.bin, app.bin, res.bin, xcfg.bin, null);
save(dcf_buf, app.dcf);
