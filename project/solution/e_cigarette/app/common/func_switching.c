#include "include.h"

//转场速度控制
#define SWITCHING_TICK_AUTO             180                         	//快速松手视为自动模式(ms)
#define SWITCHING_TICK_EXPIRE           18                          	//松手后自动切换单位时间(ms)

#define SWITCHING_FADE_STEP             20                          	//淡入淡出速度

#define SWITCHING_MENU_STEP             (GUI_SCREEN_WIDTH / 16)     //上下菜单松手后自动切换单位时间步进
#define SWITCHING_MENU_DRAG_THRESHOLD   (GUI_SCREEN_WIDTH / 7)      //上下拖动松手自动的判断阈值(需要拉多少距离才能自动完成)

#define SWITCHING_LR_STEP               (GUI_SCREEN_WIDTH / 12)     //左右转场松手后自动切换单位步进
#define SWITCHING_LR_DRAG_THRESHOLD     (GUI_SCREEN_WIDTH / 3)      //左右转场松手自动的判断阈值(需要拉多少距离才能自动完成)

#define SWITCHING_UD_STEP               (GUI_SCREEN_HEIGHT / 12)    //上下转场松手后自动切换单位步进
#define SWITCHING_UD_DRAG_THRESHOLD     (GUI_SCREEN_HEIGHT / 3)     //上下转场松手自动的判断阈值(需要拉多少距离才能自动完成)

#define SWITCHING_ZOOM_ROLL_STEP        (GUI_SCREEN_WIDTH / 10)     //滚动步进
#define SWITCHING_ZOOM_ROLL_KICK_CNT    6                           	//滚动步进分几次动画
#define SWITCHING_ZOOM_ROLL_STEP_PER    (SWITCHING_ZOOM_ROLL_STEP / SWITCHING_ZOOM_ROLL_KICK_CNT)
#define SWITCHING_ZOOM_ROLL_TICK_EXPIRE 300                         	//滚动超时
#define SWITCHING_ZOOM_STEP             (GUI_SCREEN_WIDTH / 10)     //进出应用自动切换单位
#define SWITCHING_ZOOM_EXIT_THRESHOLD   (GUI_SCREEN_WIDTH / 4)      //退出应用自动切换判断阈值(需要拉多少距离才能自动退出)
#define SWITCHING_ZOOM_RATE_HEI         max(GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT)

#define SWITCHING_ZOOM_FADE_RATE        280                         	//放大倍数速度，越大越快
#define SWITCHING_ZOOM_FADE_MAX         0x1A000                     	//最大放大到多少
#define SWITCHING_ZOOM_FADE_1           (0x10000 + (SWITCHING_ZOOM_FADE_MAX - 0x10000) / 4)
#define SWITCHING_ZOOM_FADE_2           (0x10000 + (SWITCHING_ZOOM_FADE_MAX - 0x10000) * 3 / 4)

//左右转场的起始宽度
#define SWITCHING_LR_WIDTH              (GUI_SCREEN_WIDTH / 2)
#define SWITCHING_LR_HEIGHT             (GUI_SCREEN_HEIGHT / 2)

//上下转场的起始宽度
#define SWITCHING_UD_WIDTH              (GUI_SCREEN_WIDTH / 2)
#define SWITCHING_UD_HEIGHT             (GUI_SCREEN_HEIGHT / 2)

//左右两个场景的中心距离
#define SWITCHING_LR_DISTANCE           ((GUI_SCREEN_WIDTH + SWITCHING_LR_WIDTH) / 2 + (GUI_SCREEN_WIDTH / 8))

//上下两个场景的中心距离
#define SWITCHING_UD_DISTANCE           ((GUI_SCREEN_HEIGHT + SWITCHING_UD_HEIGHT) / 2 + (GUI_SCREEN_HEIGHT / 8))

#define GUI_SIDE_MENU_WIDTH             (GUI_SCREEN_WIDTH / 2)          //边菜单的宽度

enum {
    FLAG_POS_NORM,                          //中间转场过程
    FLAG_POS_START,                         //退回到起点
    FLAG_POS_END,                           //完成到结束
};

//手动场景切换中消息处理
static void func_switching_message(u16 msg)
{
//    evt_message(msg);
}

//左右切换任务
static bool func_switching_lr(u16 switch_mode, bool flag_auto)
{
    compo_form_t *frm_last = compo_pool_get_bottom();
    compo_form_t *frm_cur = compo_pool_get_top();
    bool flag_press = !flag_auto;
    u8 flag_pos;                            //当前坐标状态
    bool flag_change;
    s32 dx = 0;
    int dx_auto;
    u32 tick = tick_get();
    s32 xpos1 = GUI_SCREEN_CENTER_X;
    s32 xpos2, xpos2_start;
    s32 x_distance;
    if (frm_last == NULL || frm_cur == NULL) {
        halt(HALT_FUNC_SWITCH_LR_PTR);
    }

    if (switch_mode == FUNC_SWITCH_LR_ZOOM_LEFT || switch_mode == FUNC_SWITCH_LR_ZOOM_RIGHT) {
        x_distance = SWITCHING_LR_DISTANCE;     //带缩放的切换，两个场景中心距离要稍微近一点
    } else {
        x_distance = GUI_SCREEN_WIDTH;          //平移切换，按屏幕宽度做中心距离
    }
    switch (switch_mode) {
    case FUNC_SWITCH_LR_LEFT:
    case FUNC_SWITCH_LR_LEFT_SMOOTH:
    case FUNC_SWITCH_LR_ZOOM_LEFT:
        //左划，右侧任务向左移入
        xpos2_start = GUI_SCREEN_CENTER_X + x_distance;
        dx_auto = -SWITCHING_LR_STEP;
        break;

    case FUNC_SWITCH_LR_RIGHT:
    case FUNC_SWITCH_LR_RIGHT_SMOOTH:
    case FUNC_SWITCH_LR_ZOOM_RIGHT:
        //右划，左侧任务向右移入
        xpos2_start = GUI_SCREEN_CENTER_X - x_distance;
        dx_auto = SWITCHING_LR_STEP;
        break;

    default:
        halt(HALT_FUNC_SWITCH_LR_MODE);
        return false;
    }
    compo_form_set_pos(frm_cur, xpos2_start, GUI_SCREEN_CENTER_Y);

    for (;;) {
        flag_pos = FLAG_POS_NORM;
        flag_change = false;
        if (flag_press) {
            s32 dy;
            flag_press = ctp_get_dxy(&dx, &dy);
            if (flag_press) {
                if (tick_check_expire(tick, SWITCHING_TICK_AUTO)) {
                    switch (switch_mode) {
                    case FUNC_SWITCH_LR_LEFT:
                    case FUNC_SWITCH_LR_LEFT_SMOOTH:
                    case FUNC_SWITCH_LR_ZOOM_LEFT:
                        //左划
                        if (dx <= -SWITCHING_LR_DRAG_THRESHOLD) {
                            dx_auto = -SWITCHING_LR_STEP;
                        } else {
                            dx_auto = SWITCHING_LR_STEP;
                        }
                        break;

                    case FUNC_SWITCH_LR_RIGHT:
                    case FUNC_SWITCH_LR_RIGHT_SMOOTH:
                    case FUNC_SWITCH_LR_ZOOM_RIGHT:
                        //右划
                        if (dx >= SWITCHING_LR_DRAG_THRESHOLD) {
                            dx_auto = SWITCHING_LR_STEP;
                        } else {
                            dx_auto = -SWITCHING_LR_STEP;
                        }
                        break;

                    default:
                        halt(HALT_FUNC_SWITCH_MENU_MODE);
                        return false;
                    }
                }
            }
            flag_change = true;
        } else if (tick_check_expire(tick, SWITCHING_TICK_EXPIRE)) {
            tick = tick_get();
            dx += dx_auto;
            flag_change = true;
        }

        if (flag_change) {
            switch (switch_mode) {
            case FUNC_SWITCH_LR_LEFT:
            case FUNC_SWITCH_LR_LEFT_SMOOTH:
            case FUNC_SWITCH_LR_ZOOM_LEFT:
                //左划，右侧任务向左移入
                if (dx >= 0) {
                    dx = 0;
                    flag_pos = FLAG_POS_START;
                } else if (dx <= -x_distance) {
                    dx = -x_distance;
                    flag_pos = FLAG_POS_END;
                }
                break;

            case FUNC_SWITCH_LR_RIGHT:
            case FUNC_SWITCH_LR_RIGHT_SMOOTH:
            case FUNC_SWITCH_LR_ZOOM_RIGHT:
                //右划，左侧任务向右移入
                if (dx <= 0) {
                    dx = 0;
                    flag_pos = FLAG_POS_START;
                } else if (dx >= x_distance) {
                    dx = x_distance;
                    flag_pos = FLAG_POS_END;
                }
                break;

            default:
                halt(HALT_FUNC_SWITCH_LR_MODE);
                return false;
            }
            xpos1 = GUI_SCREEN_CENTER_X + dx;
            xpos2 = xpos2_start + dx;
            compo_form_set_pos(frm_last, xpos1, GUI_SCREEN_CENTER_Y);
            compo_form_set_pos(frm_cur, xpos2, GUI_SCREEN_CENTER_Y);
        }
        func_process();
        if (!flag_auto) {
            func_switching_message(msg_dequeue());
        }
        if (!flag_press && flag_pos != FLAG_POS_NORM) {
            break;
        }
    }
    return (flag_pos == FLAG_POS_END);
}

//上下切换任务
static bool func_switching_ud(u16 switch_mode, bool flag_auto)
{
    compo_form_t *frm_last = compo_pool_get_bottom();
    compo_form_t *frm_cur = compo_pool_get_top();
    bool flag_press = !flag_auto;
    u8 flag_pos;                            //当前坐标状态
    bool flag_change;
    s32 dy = 0;
    int dy_auto;
    u32 tick = tick_get();
    s32 ypos1 = GUI_SCREEN_CENTER_Y;
    s32 ypos2, ypos2_start;
    s32 y_distance;
    if (frm_last == NULL || frm_cur == NULL) {
        halt(HALT_FUNC_SWITCH_UD_PTR);
    }

    if (switch_mode == FUNC_SWITCH_UD_ZOOM_UP || switch_mode == FUNC_SWITCH_UD_ZOOM_DOWN) {
        y_distance = SWITCHING_UD_DISTANCE;	//带缩放的切换，两个场景中心距离要稍微近一点
    } else {
        y_distance = GUI_SCREEN_HEIGHT;	//平移切换，按屏幕高度做中心距离
    }

    switch (switch_mode) {
    case FUNC_SWITCH_UD_UP:
    case FUNC_SWITCH_UD_UP_SMOOTH:
    case FUNC_SWITCH_UD_ZOOM_UP:
        //上划，下面任务向上移动
        ypos2_start = GUI_SCREEN_CENTER_Y + y_distance;
        dy_auto = -SWITCHING_UD_STEP;
        break;

    case FUNC_SWITCH_UD_DOWN:
    case FUNC_SWITCH_UD_DOWN_SMOOTH:
    case FUNC_SWITCH_UD_ZOOM_DOWN:
        //下划，上面任务向下移动
        ypos2_start = GUI_SCREEN_CENTER_Y - y_distance;
        dy_auto = SWITCHING_UD_STEP;
        break;

    default:
        halt(HALT_FUNC_SWITCH_UD_MODE);
        return false;
    }

    compo_form_set_pos(frm_cur, GUI_SCREEN_CENTER_X, ypos2_start);

    for (;;) {
        flag_pos = FLAG_POS_NORM;
        flag_change = false;
        if (flag_press) {
            s32 dx;
            flag_press = ctp_get_dxy(&dx, &dy);
            if (flag_press) {
                if (tick_check_expire(tick, SWITCHING_TICK_AUTO)) {
                    switch (switch_mode) {
                    case FUNC_SWITCH_UD_UP:
                    case FUNC_SWITCH_UD_UP_SMOOTH:
                    case FUNC_SWITCH_UD_ZOOM_UP:
                        if (dy <= -SWITCHING_UD_DRAG_THRESHOLD) {
                            dy_auto = -SWITCHING_UD_STEP;
                        } else {
                            dy_auto = SWITCHING_UD_STEP;
                        }
                        break;

                    case FUNC_SWITCH_UD_DOWN:
                    case FUNC_SWITCH_UD_DOWN_SMOOTH:
                    case FUNC_SWITCH_UD_ZOOM_DOWN:
                        if (dy >= SWITCHING_UD_DRAG_THRESHOLD) {
                            dy_auto = SWITCHING_UD_STEP;
                        } else {
                            dy_auto = -SWITCHING_UD_STEP;
                        }
                        break;

                    default:
                        halt(HALT_FUNC_SWITCH_MENU_MODE);
                        return false;
                    }
                }
            }
            flag_change = true;
        } else if (tick_check_expire(tick, SWITCHING_TICK_EXPIRE)) {
            tick = tick_get();
            dy += dy_auto;
            flag_change = true;
        }

        if (flag_change) {
            switch (switch_mode) {
            case FUNC_SWITCH_UD_UP:
            case FUNC_SWITCH_UD_UP_SMOOTH:
            case FUNC_SWITCH_UD_ZOOM_UP:
                //上划，下侧任务向上移入
                if (dy >= 0) {
                    dy = 0;
                    flag_pos = FLAG_POS_START;
                } else if (dy <= -y_distance) {
                    dy = -y_distance;
                    flag_pos = FLAG_POS_END;
                }
                break;

            case FUNC_SWITCH_UD_DOWN:
            case FUNC_SWITCH_UD_DOWN_SMOOTH:
            case FUNC_SWITCH_UD_ZOOM_DOWN:
                //下划，上侧任务向下移入
                if (dy <= 0) {
                    dy = 0;
                    flag_pos = FLAG_POS_START;
                } else if (dy >= y_distance) {
                    dy = y_distance;
                    flag_pos = FLAG_POS_END;
                }
                break;

            default:
                halt(HALT_FUNC_SWITCH_UD_MODE);
                return false;
            }
            ypos1 = GUI_SCREEN_CENTER_Y + dy;
            ypos2 = ypos2_start + dy;
            compo_form_set_pos(frm_last, GUI_SCREEN_CENTER_X, ypos1);
            compo_form_set_pos(frm_cur, GUI_SCREEN_CENTER_X, ypos2);
        }
        func_process();
        if (!flag_auto) {
            func_switching_message(msg_dequeue());
        }
        if (!flag_press && flag_pos != FLAG_POS_NORM) {
            break;
        }
    }
    return (flag_pos == FLAG_POS_END);
}

//上下帘菜单等切换
static bool func_switching_menu(u16 switch_mode, bool flag_auto)
{
    compo_form_t *sub_frm = compo_pool_get_top();
    bool flag_press = !flag_auto;
    u8 flag_pos;                            //当前坐标状态
    bool flag_change;
    s32 dx = 0;
    s32 dy = 0;
    int dx_auto = 0;
    int dy_auto = 0;
    u32 tick = tick_get();
    int xpos = GUI_SCREEN_CENTER_X;
    int ypos = GUI_SCREEN_CENTER_Y;
    if (sub_frm == NULL) {
        halt(HALT_FUNC_SWITCH_MENU_PTR);
    }
    switch (switch_mode) {
    case FUNC_SWITCH_MENU_DROPDOWN_UP:
    case FUNC_SWITCH_MENU_PULLUP_UP:
        //下拉往上, 上拉往上
        dy_auto = -SWITCHING_MENU_STEP;
        break;

    case FUNC_SWITCH_MENU_DROPDOWN_DOWN:
    case FUNC_SWITCH_MENU_PULLUP_DOWN:
        //下拉往下, 上拉往下
        dy_auto = SWITCHING_MENU_STEP;
        break;

    case FUNC_SWITCH_MENU_SIDE_POP:
        //边菜单往右拉出
        dx_auto = SWITCHING_MENU_STEP;
        break;

    case FUNC_SWITCH_MENU_SIDE_BACK:
        //边菜单往左退回
        dx_auto = -SWITCHING_MENU_STEP;
        break;

    default:
        halt(HALT_FUNC_SWITCH_MENU_MODE);
        break;
    }
    for (;;) {
        flag_pos = FLAG_POS_NORM;
        flag_change = false;
        if (flag_press) {
            flag_press = ctp_get_dxy(&dx, &dy);
            if (flag_press) {
                if (tick_check_expire(tick, SWITCHING_TICK_AUTO)) {
                    switch (switch_mode) {
                    case FUNC_SWITCH_MENU_DROPDOWN_UP:
                    case FUNC_SWITCH_MENU_PULLUP_UP:
                        //下拉往上, 上拉往上
                        if (dy <= -SWITCHING_MENU_DRAG_THRESHOLD) {
                            dy_auto = -SWITCHING_MENU_STEP;
                        } else {
                            dy_auto = SWITCHING_MENU_STEP;
                        }
                        break;

                    case FUNC_SWITCH_MENU_DROPDOWN_DOWN:
                    case FUNC_SWITCH_MENU_PULLUP_DOWN:
                        //下拉往下, 上拉往下
                        if (dy >= SWITCHING_MENU_DRAG_THRESHOLD) {
                            dy_auto = SWITCHING_MENU_STEP;
                        } else {
                            dy_auto = -SWITCHING_MENU_STEP;
                        }
                        break;

                    case FUNC_SWITCH_MENU_SIDE_POP:
                        //边菜单往右拉出
                        if (dx >= SWITCHING_MENU_DRAG_THRESHOLD) {
                            dx_auto = SWITCHING_MENU_STEP;
                        } else {
                            dx_auto = -SWITCHING_MENU_STEP;
                        }
                        break;

                    case FUNC_SWITCH_MENU_SIDE_BACK:
                        //边菜单往左退回
                        if (dx <= -SWITCHING_MENU_DRAG_THRESHOLD) {
                            dx_auto = -SWITCHING_MENU_STEP;
                        } else {
                            dx_auto = SWITCHING_MENU_STEP;
                        }
                        break;

                    default:
                        halt(HALT_FUNC_SWITCH_MENU_MODE);
                        break;
                    }
                }
            }
            flag_change = true;
        } else if (tick_check_expire(tick, SWITCHING_TICK_EXPIRE)) {
            tick = tick_get();
            dx += dx_auto;
            dy += dy_auto;
            flag_change = true;
        }
        if (flag_change) {
            switch (switch_mode) {
            case FUNC_SWITCH_MENU_DROPDOWN_UP:
            case FUNC_SWITCH_MENU_PULLUP_UP:
                //下拉往上, 上拉往上
                if (dy >= 0) {
                    dy = 0;
                    flag_pos = FLAG_POS_START;
                    ypos = GUI_SCREEN_CENTER_Y;
                } else if (dy <= -GUI_SCREEN_HEIGHT) {
                    dy = -GUI_SCREEN_HEIGHT;
                    flag_pos = FLAG_POS_END;
                    ypos = GUI_SCREEN_CENTER_Y - GUI_SCREEN_HEIGHT;
                }
                ypos = flag_pos == FLAG_POS_NORM ? GUI_SCREEN_CENTER_Y + dy : ypos;
                if (switch_mode == FUNC_SWITCH_MENU_PULLUP_UP) {
                    ypos += GUI_SCREEN_HEIGHT;
                }
                break;

            case FUNC_SWITCH_MENU_DROPDOWN_DOWN:
            case FUNC_SWITCH_MENU_PULLUP_DOWN:
                //下拉往下, 上拉往下
                if (dy <= 0) {
                    dy = 0;
                    flag_pos = FLAG_POS_START;
                    ypos = GUI_SCREEN_CENTER_Y - GUI_SCREEN_HEIGHT;
                } else if (dy >= GUI_SCREEN_HEIGHT) {
                    dy = GUI_SCREEN_HEIGHT;
                    flag_pos = FLAG_POS_END;
                    ypos = GUI_SCREEN_CENTER_Y;
                }
                ypos = flag_pos == FLAG_POS_NORM ? - GUI_SCREEN_CENTER_Y + dy : ypos;
                if (switch_mode == FUNC_SWITCH_MENU_PULLUP_DOWN) {
                    ypos += GUI_SCREEN_HEIGHT;
                }
                break;

            case FUNC_SWITCH_MENU_SIDE_POP:
                //边菜单往右拉出
                if (dx <= 0) {
                    dx = 0;
                    flag_pos = FLAG_POS_START;
                } else if (dx >= GUI_SIDE_MENU_WIDTH) {
                    dx = GUI_SIDE_MENU_WIDTH;
                    flag_pos = FLAG_POS_END;
                }
                xpos = -(GUI_SIDE_MENU_WIDTH / 2) + dx;
                break;

            case FUNC_SWITCH_MENU_SIDE_BACK:
                //边菜单往左退回
                if (dx >= 0) {
                    dx = 0;
                    flag_pos = FLAG_POS_START;
                } else if (dx <= -GUI_SIDE_MENU_WIDTH) {
                    dx = -GUI_SIDE_MENU_WIDTH;
                    flag_pos = FLAG_POS_END;
                }
                xpos = (GUI_SIDE_MENU_WIDTH / 2) + dx;
                break;

            default:
                halt(HALT_FUNC_SWITCH_MENU_MODE);
                break;
            }
            compo_form_set_pos(sub_frm, xpos, ypos);
        }
        func_process();
        if (!flag_auto) {
            func_switching_message(msg_dequeue());
        }
        if (!flag_press && flag_pos != FLAG_POS_NORM) {
            break;
        }
    }
    return (flag_pos == FLAG_POS_END);
}

//切换场景
bool func_switching(u16 switch_mode, void *param)
{
    if (switch_mode == FUNC_SWITCH_DIRECT) {
        return true;
    }

    if (switch_mode == FUNC_SWITCH_CANCEL) {
        halt(HALT_FUNC_SWITCH_MODE);
    }

    bool flag_auto = ((switch_mode & FUNC_SWITCH_AUTO) != 0);
    switch_mode = switch_mode & 0x7FFF;

    printf("%s\n", __func__);

    if (switch_mode < FUNC_SWITCH_UD) {
        return func_switching_lr(switch_mode, flag_auto);                       //左右切换任务
    } else if (switch_mode < FUNC_SWITCH_MENU) {
        return func_switching_ud(switch_mode, flag_auto);                       //上下切换任务
    } else {
        return func_switching_menu(switch_mode, flag_auto);                     //切换上下帘及边栏
    }

}
