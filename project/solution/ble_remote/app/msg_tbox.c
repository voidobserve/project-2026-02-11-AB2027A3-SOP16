#include "include.h"

#if FUNC_TBOX_EN

#if PROD_TEST_KEY_EN
#include "tbox_key.h"
#endif //PROD_TEST_KEY_EN

//tbox message process
AT(.text.func.msg)
static void tbox_message(u16 msg)
{
    switch (msg) {

#if (BSP_KEY_EN && PROD_TEST_KEY_EN)
        case KEY_ID_K1 | KEY_SHORT:                        // K1按键 向上按键测试
            printf("K1 key_up short key\n");
            tbox_set_key_msg(TBOX_KEY_MSG_UP);
            break;
        case KEY_ID_K2 | KEY_SHORT:                        // K2按键 向下按键测试
            printf("K2 key_down short key\n");
            tbox_set_key_msg(TBOX_KEY_MSG_DOWN);
            break;
        case KEY_ID_K3 | KEY_SHORT:                        // K3按键 向左按键测试
            printf("K3 key_left short key\n");
            tbox_set_key_msg(TBOX_KEY_MSG_LEFT);
            break;
        case KEY_ID_K4 | KEY_SHORT:                        // K4按键 向右按键测试
            printf("K4 key_right short key\n");
            tbox_set_key_msg(TBOX_KEY_MSG_RIGHT);
            break;
        case KEY_ID_K5 | KEY_SHORT:                        // K5按键 菜单按键测试
            printf("K5 key_meun short key\n");
            tbox_set_key_msg(TBOX_KEY_MSG_MEUN);
            break;
        case KEY_ID_K6 | KEY_SHORT:                        // K6按键 HOME按键测试
            printf("K6 key_home short key\n");
            tbox_set_key_msg(TBOX_KEY_MSG_HOME);
            break;
        case KEY_ID_K7 | KEY_SHORT:                        // K7按键 确认按键测试
            printf("K7 key_ok short key\n");
            tbox_set_key_msg(TBOX_KEY_MSG_OK);
            break;
        case KEY_ID_K8 | KEY_SHORT:                        // K8按键 后退按键测试
            printf("K8 key_back short key\n");
            tbox_set_key_msg(TBOX_KEY_MSG_BACK);
            break;
        case KEY_ID_K9 | KEY_SHORT:                        // K9按键 音量加按键测试
            printf("K9 key_vol_up short key\n");
            tbox_set_key_msg(TBOX_KEY_MSG_VOL_UP);
            break;
        case KEY_ID_K10 | KEY_SHORT:                        // K10按键 音量减按键测试
            printf("K10 key_vol_down short key\n");
            tbox_set_key_msg(TBOX_KEY_MSG_VOL_DOWN);
            break;
        case KEY_ID_K11 | KEY_SHORT:                        // K11按键 语音按键测试
            printf("K11 key_voice short key\n");
            tbox_set_key_msg(TBOX_KEY_MSG_VOICE_KEY_TEST);
            break;
        case KEY_ID_K11 | KEY_LONG:                         // K11按键 长按语音测试开始
            printf("K11 key_voice_ctrl long key\n");
            tbox_set_key_msg(TBOX_KEY_MSG_VOICE_ON_TEST);
            break;
        case KEY_ID_K11 | KEY_LONG_UP:                      // K11按键 长按抬起语音测试结束
            printf("K11 key_voice_ctrl long key up\n");
            tbox_set_key_msg(TBOX_KEY_MSG_VOICE_OFF_TEST);
            break;    
        case KEY_ID_K12 | KEY_SHORT:                        // K12按键 power
            printf("K12 key_pwr short\n");
            tbox_set_key_msg(TBOX_KEY_MSG_POWER);
            break;

        default: {
            uint8_t key_id = msg & 0xff;
            uint16_t key_up = msg & 0xff00;
             if ((key_id <= KEY_ID_K12) && (key_id > KEY_ID_NO)) {
                if ((key_up == KEY_SHORT_UP) || (key_up == KEY_LONG_UP)) {
                    // printf("key up\n");
                    tbox_set_key_msg(TBOX_KEY_MSG_RELEASE);
                }
            } else if (tbox_get_key_msg() == TBOX_KEY_MSG_RELEASE){
                tbox_set_key_msg(TBOX_KEY_MSG_NONE);
            }
        } break;
#else

        default:
            break;
#endif
    }
}

AT(.text.func.msg)
static void func_tbox_message_do(u16 msg)
{
    switch (msg) {
        case MSG_SYS_1S:
            //printf("MSG_SYS_1S\n");
        default:
            tbox_message(msg);
            break;
    }
}

AT(.text.app.proc.msg)
void func_tbox_message(u16 msg)
{
    if(msg){
        func_tbox_message_do(msg);
    }
}
#endif  //FUNC_TBOX_EN
