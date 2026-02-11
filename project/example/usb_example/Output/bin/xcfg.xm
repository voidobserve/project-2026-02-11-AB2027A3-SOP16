depend(0x01020000)

config(SUB, "系统配置", "系统相关配置");
config(LEVEL, 0x03);
config(LIST, "OSC基础电容", "同时配置24晶振OSCI与OSCO负载电容", UOSC_BOTH_CAP, 2, "0PF", "6PF", 0);
config(BYTE, "自定义OSCI电容", "配置24M晶振OSCI负载电容（0.25PF）。电容大小：n * 0.25PF + 基础电容", UOSCI_CAP, 0, 63, 45);
config(BYTE, "自定义OSCO电容", "配置24M晶振OSCO负载电容（0.25PF）。电容大小：n * 0.25PF + 基础电容", UOSCO_CAP, 0, 63, 45);
config(CHECK,  "优先使用产测电容值",  "是否优先使用产测校准的OSC电容值，没过产测或者关闭时使用<自定义OSCI电容>、<自定义OSCO电容>",  FT_OSC_CAP_EN,  1);

config(LEVEL, 0x0E);
config(SUB, "产测保留", "产测时保留的一些参数");
config(KEEP);
config(BYTE, "产测OSC基础电容", "同时配置24晶振OSCI与OSCO负载电容,0: 0PF, 1:6PF", OSC_BOTH_CAP, 0, 1, 0);
config(BYTE, "产测OSCI电容", "独立配置24M晶振OSCI负载电容，单位0.25PF。OSCI电容大小：n * 0.25PF + 基础电容, 不要改动默认值0", OSCI_CAP, 0, 63, 0);
config(BYTE, "产测OSCO电容", "独立配置24M晶振OSCO负载电容，单位0.25PF。OSCO电容大小：n * 0.25PF + 基础电容, 不要改动默认值0", OSCO_CAP, 0, 63, 0);
#config(BUF, "加密狗授权密钥", "程序运行需要判断的合法授权密钥", dongle_soft_key, 20);

config(UPD, 0x1F, 0x0);

config(LEVEL, 0x00);
makecfgfile(xcfg.bin);
makecfgdef(xcfg.h);
xcopy(xcfg.h, ../../xcfg.h);
