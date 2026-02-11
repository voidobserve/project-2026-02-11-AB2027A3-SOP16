#ifndef _LANG_H
#define _LANG_H

/*****************************************************************************
 * Module    : 多国语言配置选择列表
 *****************************************************************************/
#define LANG_NULL                   0
#define LANG_ENGLISH_EN             1               //英文(英国)
#define LANG_CHINESE_ZH_CH          2               //中文(简体)
#define LANG_ITALIAN_IT_IT          3               //意大利语(意大利)
#define LANG_SPANISH_ES_ES          4               //西班牙语(西班牙)
#define LANG_PORTUGUESE_PT_PT       5               //葡萄牙语(葡萄牙)
#define LANG_RUSSIAN_RU_RU          6               //俄语(俄罗斯)
#define LANG_JAPANESE_JA_JP         7               //日语(日本)
#define LANG_CHINESE_ZH_TW          8               //中文(繁体)
#define LANG_GERMAN_DE_DE           9               //德语(德国)
#define LANG_KOREAN_KO              10              //韩语
#define LANG_THAI_TH_TH             11              //泰语(泰国)
#define LANG_ARABIC_AR_AE           12              //阿拉伯语(阿拉伯联合酋长国)
#define LANG_TURKISH_TR_TR          13              //土耳其语(土耳其)
#define LANG_FRENCH_FR_FR           14              //法语(法国)
#define LANG_VIETNAMESE_VI          15              //越南语
#define LANG_POLISH_PL_PL           16              //波兰语(波兰)
#define LANG_DUTCH_NL_NL            17              //荷兰语(荷兰)
#define LANG_HEBREW_IW              18              //希伯来语
#define LANG_PERSIAN_FA             19              //波斯语
#define LANG_GREEK_EL_GR            20              //希腊语(希腊)
#define LANG_MALAYSIA_MS            21              //马来语
#define LANG_BURMESE                22              //缅甸语
#define LANG_DANISH_DA_DK           23              //丹麦语(丹麦)
#define LANG_UKRAINIAN_UK_UA        24              //乌克兰语(乌克兰)
#define LANG_INDONESIAN_ID          25              //印尼语(印度尼西亚)
#define LANG_CZECH_CS_CZ            26              //捷克语(捷克共和国)
#define LANG_HINDI                  27              //印地语(印度)
#define LANG_MAX                    28

#define LANG_DEVICE                 (BIT(LANG_ENGLISH_EN) | \
                                    BIT(LANG_CHINESE_ZH_CH) | \
                                    BIT(LANG_ITALIAN_IT_IT) | \
                                    BIT(LANG_SPANISH_ES_ES) | \
                                    BIT(LANG_PORTUGUESE_PT_PT) | \
                                    BIT(LANG_RUSSIAN_RU_RU) | \
                                    BIT(LANG_JAPANESE_JA_JP) | \
                                    BIT(LANG_GERMAN_DE_DE) | \
                                    BIT(LANG_KOREAN_KO) | \
                                    BIT(LANG_THAI_TH_TH) | \
                                    BIT(LANG_ARABIC_AR_AE) | \
                                    BIT(LANG_TURKISH_TR_TR) | \
                                    BIT(LANG_FRENCH_FR_FR) | \
                                    BIT(LANG_VIETNAMESE_VI) | \
                                    BIT(LANG_MALAYSIA_MS) | \
                                    BIT(LANG_HINDI) | \
                                    BIT(LANG_CHINESE_ZH_TW) | \
                                    BIT(LANG_BURMESE) | \
                                    BIT(LANG_HEBREW_IW) | \
                                    BIT(LANG_PERSIAN_FA) | \
                                    BIT(LANG_DUTCH_NL_NL))          //支持的语言

#define LANG_DEFAULT                LANG_CHINESE_ZH_CH              //默认语言

#define LANG_NUM                    (lang_num_get())                //支持的语言数量
#define LAND_IS_SUPPORT(x)          (LANG_DEVICE & BIT(x))          //判断当前系统语言是否支持

enum {
    STR_MESSAGE,
    STR_LANGUAGE_CN,
    STR_LANGUAGE_ENG,
    STR_LANGUAGE_IT,
    STR_LANGUAGE_ES,
    STR_LANGUAGE_PT,
    STR_LANGUAGE_RU,
    STR_LANGUAGE_JP,
    STR_LANGUAGE_ZH_TW,
    STR_LANGUAGE_DE,
    STR_LANGUAGE_KO,
    STR_LANGUAGE_TH,
    STR_LANGUAGE_AR_AE,
    STR_LANGUAGE_TR,
    STR_LANGUAGE_FR,
    STR_LANGUAGE_VI,
    STR_LANGUAGE_PL,
    STR_LANGUAGE_NL,
    STR_LANGUAGE_IW,
    STR_LANGUAGE_FA,
    STR_LANGUAGE_EL_GR,
    STR_LANGUAGE_MS,
    STR_LANGUAGE_BM,//BURMESE
    STR_LANGUAGE_DA_DK,
    STR_LANGUAGE_UK_KU,
    STR_LANGUAGE_ID,
    STR_LANGUAGE_CS_CZ,
    STR_LANGUAGE_HD,//HINDI

    STR_SUNDAY,
    STR_MONDAY,
    STR_TUESDAY,
    STR_WEDNESDAY,
    STR_THURSDAY,
    STR_FRIDAY,
    STR_SATURDAY,
    STR_CLOUDY,
    STR_UNKNOWN,

    STR_EQUALIZER,
    STR_SPATIAL_SOUND,
    STR_SCREEN_BRIGHT,
    STR_SETTING_LANGUAGE,
    STR_SCREEN_WALLPAPER,
    STR_BG_WALLPAPER,
    STR_FIND_MY_EARPHONE,
    STR_TAKE_OFF_EARPHONE,
    STR_EARPHONE_BT_CON,
    STR_SCAN_DEV,
    STR_PAIR_OR_NOT,
    STR_PAIRING,
    STR_PAIR_FAIL,
    STR_PAIR_OK,
    STR_CONNECTED,
    STR_DISCONNECT,
    STR_CLEAR_PAIRING,
    STR_SEARCHING_DEV,
    STR_TAKE_OUT_EARPHONE,
    STR_CHARGE_EARPHONE_CONNECTED,
    STR_CHARGE_EARPHONE_DISCONNECT,
    STR_PAIR_YES_OR_NO,

    STR_POPULAR,
    STR_JASS,
    STR_ROCK,
    STR_CLASSIC,
    STR_COUNTRY,

    STR_LIGHT_LOW,
    STR_LIGHT_MID,
    STR_LIGHT_HIGH,
    STR_CANCEL,
    STR_REPEAT,
    STR_STOP,
    STR_START,
    STR_VERIFY,

    STR_RESTART,
    STR_POWEROFF,
    STR_RESTORE,
};

extern const char * const *i18n;

/**
 * @brief 选择系统语言
 * @param[in] lang_id:系统语言id
 **/
void lang_select(int lang_id);


/**
 * @brief 获取当前系统语言id
 * @return 系统语言id
 **/
int lang_get(void);

/**
 * @brief 获取当前支持的系统语言数量
 * @return 支持的系统语言数量
 **/
u8 lang_num_get(void);
#endif
