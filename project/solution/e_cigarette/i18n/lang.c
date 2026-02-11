#include "include.h"

#define TRACE_EN                1

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

extern const char * const i18n_zh[];
extern const char * const i18n_en[];
extern const char * const i18n_de[];
extern const char * const i18n_es[];
extern const char * const i18n_it[];
extern const char * const i18n_jp[];
extern const char * const i18n_ko[];
extern const char * const i18n_th[];
extern const char * const i18n_ar_ae[];
extern const char * const i18n_tr[];
extern const char * const i18n_pt[];
extern const char * const i18n_ru[];
extern const char * const i18n_fr[];
extern const char * const i18n_vi[];
extern const char * const i18n_ms[];
extern const char * const i18n_hd[];
extern const char * const i18n_zh_tw[];
extern const char * const i18n_fa[];
extern const char * const i18n_iw[];
extern const char * const i18n_bm[];
extern const char * const i18n_nl[];

const char * const *i18n;

void lang_select(int lang_id)
{
    switch (lang_id) {
#if (LANG_DEVICE & BIT(LANG_ENGLISH_EN))
    case LANG_ENGLISH_EN:
        i18n = i18n_en;
        break;
#endif
#if (LANG_DEVICE & BIT(LANG_CHINESE_ZH_CH))
    case LANG_CHINESE_ZH_CH:
        i18n = i18n_zh;
        break;
#endif
#if (LANG_DEVICE & BIT(LANG_ITALIAN_IT_IT))
    case LANG_ITALIAN_IT_IT:
        i18n = i18n_it;
        break;
#endif
#if (LANG_DEVICE & BIT(LANG_SPANISH_ES_ES))
    case LANG_SPANISH_ES_ES:
        i18n = i18n_es;
        break;
#endif
#if (LANG_DEVICE & BIT(LANG_PORTUGUESE_PT_PT))
    case LANG_PORTUGUESE_PT_PT:
        i18n = i18n_pt;
        break;
#endif
#if (LANG_DEVICE & BIT(LANG_RUSSIAN_RU_RU))
    case LANG_RUSSIAN_RU_RU:
        i18n = i18n_ru;
        break;
#endif
#if (LANG_DEVICE & BIT(LANG_JAPANESE_JA_JP))
    case LANG_JAPANESE_JA_JP:
        i18n = i18n_jp;
        break;
#endif
#if (LANG_DEVICE & BIT(LANG_GERMAN_DE_DE))
    case LANG_GERMAN_DE_DE:
        i18n = i18n_de;
        break;
#endif
#if LAND_IS_SUPPORT(LANG_KOREAN_KO)
    case LANG_KOREAN_KO:
        i18n = i18n_ko;
        break;
#endif
#if LAND_IS_SUPPORT(LANG_THAI_TH_TH)
    case LANG_THAI_TH_TH:
        i18n = i18n_th;
        break;
#endif
#if LAND_IS_SUPPORT(LANG_ARABIC_AR_AE)
    case LANG_ARABIC_AR_AE:
        i18n = i18n_ar_ae;
        break;
#endif
#if LAND_IS_SUPPORT(LANG_TURKISH_TR_TR)
    case LANG_TURKISH_TR_TR:
        i18n = i18n_tr;
        break;
#endif
#if LAND_IS_SUPPORT(LANG_FRENCH_FR_FR)
    case LANG_FRENCH_FR_FR:
        i18n = i18n_fr;
        break;
#endif
#if LAND_IS_SUPPORT(LANG_VIETNAMESE_VI)
    case LANG_VIETNAMESE_VI:
        i18n = i18n_vi;
        break;
#endif
#if LAND_IS_SUPPORT(LANG_MALAYSIA_MS)
    case LANG_MALAYSIA_MS:
        i18n = i18n_ms;
        break;
#endif
#if LAND_IS_SUPPORT(LANG_HINDI)
    case LANG_HINDI:
        i18n = i18n_hd;
        break;
#endif
#if LAND_IS_SUPPORT(LANG_CHINESE_ZH_TW)
    case LANG_CHINESE_ZH_TW:
        i18n = i18n_zh_tw;
        break;
#endif
#if LAND_IS_SUPPORT(LANG_HEBREW_IW)
    case LANG_HEBREW_IW:
        i18n = i18n_iw;
        break;
#endif
#if LAND_IS_SUPPORT(LANG_PERSIAN_FA)
    case LANG_PERSIAN_FA:
        i18n = i18n_fa;
        break;
#endif
#if LAND_IS_SUPPORT(LANG_BURMESE)
    case LANG_BURMESE:
        i18n = i18n_bm;
        break;
#endif
#if LAND_IS_SUPPORT(LANG_DUTCH_NL_NL)
    case LANG_DUTCH_NL_NL:
        i18n = i18n_nl;
        break;
#endif
    default:
        TRACE("Language Select Error[%d]!\n", lang_id);
        halt(HALT_I18N_INIT);
        break;
    }

    TRACE("%s:%d %p\n", __func__, lang_id, i18n);
}

int lang_get(void)
{
    int lang_id = LANG_DEFAULT;

    if (i18n == i18n_en) {
#if (LANG_DEVICE & BIT(LANG_ENGLISH_EN))
        lang_id = LANG_ENGLISH_EN;
#endif
#if (LANG_DEVICE & BIT(LANG_CHINESE_ZH_CH))
    } else if (i18n == i18n_zh) {
        lang_id = LANG_CHINESE_ZH_CH;
#endif
#if (LANG_DEVICE & BIT(LANG_ITALIAN_IT_IT))
    } else if (i18n == i18n_it) {
        lang_id = LANG_ITALIAN_IT_IT;
#endif
#if (LANG_DEVICE & BIT(LANG_SPANISH_ES_ES))
    } else if (i18n == i18n_es) {
        lang_id = LANG_SPANISH_ES_ES;
#endif
#if (LANG_DEVICE & BIT(LANG_PORTUGUESE_PT_PT))
    } else if (i18n == i18n_pt) {
        lang_id = LANG_PORTUGUESE_PT_PT;
#endif
#if (LANG_DEVICE & BIT(LANG_RUSSIAN_RU_RU))
    } else if (i18n == i18n_ru) {
        lang_id = LANG_RUSSIAN_RU_RU;
#endif
#if (LANG_DEVICE & BIT(LANG_JAPANESE_JA_JP))
    } else if (i18n == i18n_jp) {
        lang_id = LANG_JAPANESE_JA_JP;
#endif
#if (LANG_DEVICE & BIT(LANG_GERMAN_DE_DE))
    } else if (i18n == i18n_de) {
        lang_id = LANG_GERMAN_DE_DE;
#endif
#if (LANG_DEVICE & BIT(LANG_KOREAN_KO))
    } else if (i18n == i18n_ko) {
        lang_id = LANG_KOREAN_KO;
#endif
#if (LANG_DEVICE & BIT(LANG_THAI_TH_TH))
    } else if (i18n == i18n_th) {
        lang_id = LANG_THAI_TH_TH;
#endif
#if LAND_IS_SUPPORT(LANG_ARABIC_AR_AE)
    } else if (i18n == i18n_ar_ae) {
        lang_id = LANG_ARABIC_AR_AE;
#endif
#if (LANG_DEVICE & BIT(LANG_TURKISH_TR_TR))
    } else if (i18n == i18n_tr) {
        lang_id = LANG_TURKISH_TR_TR;
#endif
#if (LANG_DEVICE & BIT(LANG_FRENCH_FR_FR))
    } else if (i18n == i18n_fr) {
        lang_id = LANG_FRENCH_FR_FR;
#endif
#if LAND_IS_SUPPORT(LANG_VIETNAMESE_VI)
    } else if (i18n == i18n_vi) {
        lang_id = LANG_VIETNAMESE_VI;
#endif
#if LAND_IS_SUPPORT(LANG_MALAYSIA_MS)
    } else if (i18n == i18n_ms) {
        lang_id = LANG_MALAYSIA_MS;
#endif
#if LAND_IS_SUPPORT(LANG_HINDI)
    } else if (i18n == i18n_hd) {
        lang_id = LANG_HINDI;
#endif
#if LAND_IS_SUPPORT(LANG_CHINESE_ZH_TW)
    } else if (i18n == i18n_zh_tw) {
        lang_id = LANG_CHINESE_ZH_TW;
#endif
#if LAND_IS_SUPPORT(LANG_HEBREW_IW)
    } else if (i18n == i18n_iw) {
        lang_id = LANG_HEBREW_IW;
#endif
#if LAND_IS_SUPPORT(LANG_PERSIAN_FA)
    } else if (i18n == i18n_fa) {
        lang_id = LANG_PERSIAN_FA;
#endif
#if LAND_IS_SUPPORT(LANG_BURMESE)
    } else if (i18n == i18n_bm) {
        lang_id = LANG_BURMESE;
#endif
#if LAND_IS_SUPPORT(LANG_DUTCH_NL_NL)
    } else if (i18n == i18n_nl) {
        lang_id = LANG_DUTCH_NL_NL;
#endif
    }

    TRACE("%s:%d %p\n", __func__, lang_id, i18n);
    return lang_id;
}

u8 lang_num_get(void)
{
    u8 lang_num = 0;
    for (u8 i = LANG_ENGLISH_EN; i < LANG_MAX; i++) {
        if (LANG_DEVICE & BIT(i)) {
            lang_num++;
        }
    }

    TRACE("%s:%d\n", __func__, lang_num);
    return lang_num;
}
