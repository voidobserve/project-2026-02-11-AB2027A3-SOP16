#include "include.h"

#define TRACE_EN                1

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#define COMPO_TEXT_AUTOROLL_NORMAL            0							//单方向滚动后循环滚动
#define COMPO_TEXT_AUTOROLL_LR                1							//左右回弹
#define COMPO_TEXT_AUTOROLL_DIRECTION         COMPO_TEXT_AUTOROLL_NORMAL//文本滚动方向

#define COMPO_TEXT_AUTOROLL_START_TICK        1000                      //文本滚动开始延时
#define COMPO_TEXT_AUTOROLL_STOP_TICK         1500                      //文本滚动结束延时
#define COMPO_TEXT_AUTOROLL_TICK              30                        //文本滚动

/**
 * @brief 根据ID获取组件对象
 * @param[in] id : 组件的ID
 * @return 返回组件的指针
 **/
void *compo_getobj_byid(u16 id)
{
    if (id == 0) {
        return NULL;
    }
    component_t *compo = compo_get_head();
    while (compo != NULL) {
        if (compo->id == id) {
            return compo;
        }
        compo = compo_get_next(compo);          //遍历组件
    }
    return NULL;
}

///**
// * @brief 更新组件时间到TE时间
// **/
ALIGNED(256)
void compo_update(void)
{
    u32 rtc_cnt, rtc_cnt2, rtc_pr2;
    bool rtc_update;

    GLOBAL_INT_DISABLE();
    rtc_update = compo_cb.rtc_update;
    rtc_cnt = RTCCNT;
    rtc_cnt2 = RTCCON2 >> 17 << 2;
    rtc_pr2 = (RTCCON2 & 0x1FFFF) + 1;
    compo_cb.rtc_update = false;
    GLOBAL_INT_RESTORE();

    if (!rtc_update) {
        return;
    }
    compo_cb.tm = time_to_tm(rtc_cnt);
    compo_cb.mtime = rtc_cnt2 * 1000 / rtc_pr2;
    compo_set_update(compo_cb.tm, compo_cb.mtime);
}
//
/**
 * @brief 设置绑定数据
 * @param[in] compo : 组件
 * @param[in] tm : 时间tm结构体

 **/
void compo_set_bonddata(component_t *compo, tm_t tm)
{
    bool flag_update = true;
    int value = 0;
    char value_str[16];
    memset(value_str, '\0', 16);

    switch (compo->bond_data) {
    case COMPO_BOND_YEAD:
        value = tm.year;
        sprintf(value_str, "%d", value);
        break;

    case COMPO_BOND_MONTH:
        value = tm.mon;
        sprintf(value_str, "%d", value);
        break;

    case COMPO_BOND_DAY:
        value = tm.day;
        sprintf(value_str, "%d", value);
        break;

    case COMPO_BOND_WEEKDAY:
        value = tm.weekday;
        sprintf(value_str, "%d", value);
        strcpy(value_str, i18n[STR_SUNDAY + value]);
        break;

    case COMPO_BOND_HOUR:
        value = tm.hour;
        sprintf(value_str, "%d", value);
        break;

    case COMPO_BOND_MINUTE:
        value = tm.min;
        sprintf(value_str, "%02d", value);
        break;

    case COMPO_BOND_HOUR_H:
        value = tm.hour / 10;
        sprintf(value_str, "%d", value);
        break;

    case COMPO_BOND_HOUR_L:
        value = tm.hour % 10;
        sprintf(value_str, "%d", value);
        break;

    case COMPO_BOND_MINUTE_H:
        value = tm.min / 10;
        sprintf(value_str, "%02d", value);
        break;

    case COMPO_BOND_MINUTE_L:
        value = tm.min % 10;
        sprintf(value_str, "%02d", value);
        break;

    case COMPO_BOND_SECOND:
        value = tm.sec;
        sprintf(value_str, "%02d", value);
        break;

    case COMPO_BOND_DATE:
        value = tm.mon * 100 + tm.day;
        sprintf(value_str, "%02d/%02d", tm.mon, tm.day);
        break;


    default:
        flag_update = false;
        break;
    }

    if (flag_update) {
//        printf("bond_data:%d, type:%d, value:%d\n", compo->bond_data, compo->type, value);
        if (compo->type == COMPO_TYPE_NUMBER) {
            compo_number_t *num = (compo_number_t *)compo;
            if (num->num_part) {
                if (num->num_part > 1) {
                    value /= soft_pow(10, num->num_part - 1);
                }
                value %= 10;
            }
            compo_number_set(num, value);
            compo_number_set_visible(num, true);
        } else if (compo->type == COMPO_TYPE_TEXTBOX) {
            compo_textbox_t *txt = (compo_textbox_t *)compo;
            compo_textbox_set(txt, value_str);
            compo_textbox_set_visible(txt, true);
        } else if (compo->type == COMPO_TYPE_PICTUREBOX) {
            compo_picturebox_t *pic = (compo_picturebox_t *)compo;
            compo_picturebox_cut(pic, value, pic->radix);
            compo_picturebox_set_visible(pic, true);
        }
    }
}

/**
 * @brief 设置滚动
 * @param[in] rcb : 滚动控制结构体
 * @param[in] txt : 滚动Widget Text
 * @param[in] multiline : 是否为多行竖向滚动

 **/
static void compo_set_roll(compo_roll_cb_t *rcb, widget_text_t *txt, bool multiline)
{
    if (!rcb->mode && !rcb->is_drag) {
        return;
    }

    static bool flag_drag = false;
    static s32 ddy = 0;
    s32 dx=0,dy=0;
    bool align_center = widget_get_align_center(txt);
    area_t rel_textbox_area = widget_text_get_box_area_rel(txt);
    area_t rel_text_area = widget_text_get_area(txt);
    u16 circ_pixel = widget_text_get_autoroll_circ_pixel(txt);
    int end_hei = rel_textbox_area.hei * 2 / 3;
    bool txt_directoion = widget_text_get_right_align(txt);

    flag_drag = ctp_get_dxy(&dx, &dy);
    if (flag_drag) {    //按下

    } else {        //抬起
        ddy = rcb->offset;
    }

    switch (rcb->sta) {
    case COMPO_ROLL_STA_IDLE:
        if (flag_drag && rcb->is_drag && multiline) {           //多行显示可拖拽
            rcb->tick = tick_get();
            rcb->sta = COMPO_ROLL_STA_ROLL;
        } else if (tick_check_expire(rcb->tick, COMPO_TEXT_AUTOROLL_START_TICK)) {
            rcb->tick = tick_get();
            rcb->sta = COMPO_ROLL_STA_ROLL;
        }
        break;

    case COMPO_ROLL_STA_ROLL:
        if (flag_drag && rcb->is_drag && multiline) {           //多行显示可拖拽
            rcb->tick = tick_get();
            rcb->offset = ddy + dy;
            if (rcb->offset > 0) {
                rcb->offset = 0;
            } else if (rcb->offset < end_hei - rel_text_area.hei) {
                rcb->offset = end_hei - rel_text_area.hei;
            }
            widget_text_set_client(txt, 0, rcb->offset);
        } else if (tick_check_expire(rcb->tick, COMPO_TEXT_AUTOROLL_TICK)) {
            rcb->tick = tick_get();
            if (rcb->mode) {
                rcb->offset += rcb->direction;
            }
            if (multiline) {
                if (rel_text_area.hei + rcb->offset <= end_hei) {
                    rcb->sta = COMPO_ROLL_STA_STOP;
                }
                widget_text_set_client(txt, 0, rcb->offset);

            } else {
                int end_wid = 0;
				rel_text_area.wid = align_center ? rel_text_area.wid / 2 : rel_text_area.wid;  //文本居中时，宽度需要/2，防止滚完导致C208蓝屏
                if (txt_directoion) {
#if COMPO_TEXT_AUTOROLL_DIRECTION
                    end_wid = rel_textbox_area.wid * 1 / 3;
                    if ((rcb->offset <= -(rel_text_area.wid * 1 / 3 + end_wid)) || (rcb->offset >= end_wid)) {
                        rcb->direction = -rcb->direction;
                    }
#else
                    if (rcb->mode == TEXT_AUTOROLL_MODE_SROLL_CIRC) {
                        end_wid = 0;
                    } else if (rcb->mode == TEXT_AUTOROLL_MODE_SROLL) {
                        end_wid = rel_text_area.wid;
                        if (rel_textbox_area.wid < rel_text_area.wid / 3) {
                            end_wid += (rel_text_area.wid / 3 - rel_textbox_area.wid);
                        }
                    }

                    if (rcb->offset > end_wid) {
                        if (rcb->mode == TEXT_AUTOROLL_MODE_SROLL_CIRC) {
							rcb->offset = -(rel_text_area.wid * (1 + align_center)  + circ_pixel);
						} else if (rcb->mode == TEXT_AUTOROLL_MODE_SROLL) {
                            rcb->sta = COMPO_ROLL_STA_STOP;
						}
                    }
#endif
                } else {
#if COMPO_TEXT_AUTOROLL_DIRECTION
                    end_wid = rel_textbox_area.wid * 1 / 3;
                    if ((rcb->offset <= -(rel_text_area.wid * 1 / 3 + end_wid)) || (rcb->offset >= end_wid)) {
                        rcb->direction = -rcb->direction;
                    }
#else
                    s16 circ_end_wid = align_center ? (rel_text_area.wid * 2 + circ_pixel) : (rel_text_area.wid + circ_pixel);
                    if (rcb->mode == TEXT_AUTOROLL_MODE_SROLL_CIRC) {
                        end_wid = circ_end_wid;
                    } else if (rcb->mode == TEXT_AUTOROLL_MODE_SROLL) {
                        end_wid = rel_text_area.wid * 2 / 3;
                    }

                    if (rel_textbox_area.wid < rel_text_area.wid / 3) {
                        end_wid += (rel_text_area.wid / 3 - rel_textbox_area.wid);
                    }
                    if (rcb->offset < -end_wid) {
						if (rcb->mode == TEXT_AUTOROLL_MODE_SROLL_CIRC) {
							rcb->offset = 0;
						} else if (rcb->mode == TEXT_AUTOROLL_MODE_SROLL){
                          rcb->sta = COMPO_ROLL_STA_STOP;
						}

                    }
#endif
                }
                widget_text_set_client(txt, rcb->offset, 0);
            }
        }
        break;

    case COMPO_ROLL_STA_STOP:
        if (flag_drag && rcb->is_drag && multiline) {               //多行显示可拖拽
            rcb->tick = tick_get();
            rcb->sta = COMPO_ROLL_STA_ROLL;
        } else if (tick_check_expire(rcb->tick, COMPO_TEXT_AUTOROLL_STOP_TICK)) {
            rcb->tick = tick_get();
            rcb->sta = COMPO_ROLL_STA_IDLE;
            if (multiline) {
                rcb->offset = 0;
                widget_text_set_client(txt, 0, rcb->offset);
            } else {
                rcb->offset = (txt_directoion && rcb->mode) ?  -rel_text_area.wid : 0;
                if (txt_directoion && (rcb->mode == TEXT_AUTOROLL_MODE_SROLL_CIRC)) {
                    rcb->offset = -rel_text_area.wid;
                } else {
                    rcb->offset = 0;
                }
                //居中对齐且存在滚动时从最左边开始滚动
                if (rcb->direction && widget_get_align_center(txt)) {
                    rcb->offset = (rel_text_area.wid - rel_textbox_area.wid) / 2;
                }
                widget_text_set_client(txt, rcb->offset, 0);
            }
        }
        break;

    default:
        halt(HALT_GUI_COMPO_ROLL);
        break;
    }
}

/**
 * @brief 设置组件更新
 * @param[in] tm : 时间tm结构体
 * @param[in] mtime : 毫秒
 **/
void compo_set_update(tm_t tm, u16 mtime)
{
    component_t *compo = compo_get_head();
    char time_str[8];
    while (compo != NULL) {
        switch (compo->type) {
        case COMPO_TYPE_FORM:
            {
                compo_form_t *frm = (compo_form_t *)compo;
                if (widget_get_visble(frm->time)) {
                    sprintf(time_str, "%02d:%02d", tm.hour, tm.min);
                    widget_text_set(frm->time, time_str);
                }
            }
            break;

        case COMPO_TYPE_NUMBER:
        case COMPO_TYPE_PICTUREBOX:
            compo_set_bonddata(compo, tm);
            break;

        case COMPO_TYPE_TEXTBOX:
            {
                compo_set_bonddata(compo, tm);                                                  //设置绑定数据
                compo_textbox_t *textbox = (compo_textbox_t *)compo;
                compo_set_roll(&textbox->roll_cb, textbox->txt, textbox->multiline);            //滚动
            }
            break;

        case COMPO_TYPE_ANIMATION:
//            compo_animation_process((compo_animation_t *)compo);
            break;

        default:
            break;
        }
        compo = compo_get_next(compo);                                                      //遍历组件
    }
}

/**
 * @brief 获取按键ID
 **/
int compo_get_button_id(void)
{
    point_t pt = ctp_get_sxy();
    component_t *compo = (component_t *)compo_pool_get_top();
    while (compo != NULL) {
        if (compo->type == COMPO_TYPE_BUTTON) {
            compo_button_t *btn = (compo_button_t *)compo;
            rect_t rect = widget_get_absolute(btn->widget);
            if (abs_s(pt.x - rect.x) * 2 <= rect.wid && abs_s(pt.y - rect.y) * 2 <= rect.hei) {
                return btn->id;
            }
        }
        compo = compo_get_next(compo);          //遍历组件
    }
    return ID_NULL;
}
