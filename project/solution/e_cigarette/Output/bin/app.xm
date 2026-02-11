#include "config.h"
depend(0x01010200);
setflash(1, FLASH_SIZE, FLASH_ERASE_4K, FLASH_DUAL_READ, FLASH_QUAD_READ);
setspace(FLASH_RESERVE_SIZE);
#if (AB_FOT_EN && (AB_FOT_TYPE == AB_FOT_TYPE_PACK))
setunpack(unpack.bin);
setpkgarea(FOT_PACK_START, FOT_PACK_SIZE);
#endif

setuserbin(FLASH_UI_BASE, FLASH_UI_SIZE, ui.bin, 1);   #起始地址要与ui.xm同步

make(dcf_buf, header.bin, app.bin, res.bin, xcfg.bin, updater.bin);
save(dcf_buf, app.dcf);
