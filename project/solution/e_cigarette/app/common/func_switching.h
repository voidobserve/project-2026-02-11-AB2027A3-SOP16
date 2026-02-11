#ifndef _FUNC_SWITCHING_H
#define _FUNC_SWITCHING_H

#define FUNC_SWITCH_AUTO                0x8000                  //自动完成切换

enum FUNC_SWITCH_MODE {
    FUNC_SWITCH_DIRECT,                                         //无切换/直接切换
    FUNC_SWITCH_CANCEL,                                         //取消切换

    //淡入淡出
    FUNC_SWITCH_FADE,
    FUNC_SWITCH_FADE_OUT,

    //功能切换
    FUNC_SWITCH_LR,
    FUNC_SWITCH_LR_LEFT = FUNC_SWITCH_LR,                       //平移切换
    FUNC_SWITCH_LR_RIGHT,
    FUNC_SWITCH_LR_LEFT_SMOOTH,
    FUNC_SWITCH_LR_RIGHT_SMOOTH,
    FUNC_SWITCH_LR_ZOOM_LEFT,                                   //缩放切换
    FUNC_SWITCH_LR_ZOOM_RIGHT,

    FUNC_SWITCH_UD,
    FUNC_SWITCH_UD_UP = FUNC_SWITCH_UD,                       	//平移切换
    FUNC_SWITCH_UD_DOWN,
    FUNC_SWITCH_UD_UP_SMOOTH,
    FUNC_SWITCH_UD_DOWN_SMOOTH,
    FUNC_SWITCH_UD_ZOOM_UP,                                   	//缩放切换
    FUNC_SWITCH_UD_ZOOM_DOWN,

    //上下帘切换
    FUNC_SWITCH_MENU,
    FUNC_SWITCH_MENU_DROPDOWN_UP = FUNC_SWITCH_MENU,            //下拉菜单
    FUNC_SWITCH_MENU_DROPDOWN_DOWN,
    FUNC_SWITCH_MENU_PULLUP_UP,                                 //上拉菜单
    FUNC_SWITCH_MENU_PULLUP_DOWN,
    FUNC_SWITCH_MENU_SIDE_POP,                                  //边栏
    FUNC_SWITCH_MENU_SIDE_BACK,

    //进出从图标缩放切换
    FUNC_SWITCH_ZOOM,
    FUNC_SWITCH_ZOOM_ENTER = FUNC_SWITCH_ZOOM,                  //进入应用
    FUNC_SWITCH_ZOOM_EXIT,                                      //退出应用

    //进出淡入切换
    FUNC_SWITCH_ZOOM_FADE,
    FUNC_SWITCH_ZOOM_FADE_ENTER = FUNC_SWITCH_ZOOM_FADE,        //进入应用
    FUNC_SWITCH_ZOOM_FADE_EXIT,                                 //退出应用
};

bool func_switching(u16 switch_mode, void *param);              //转场动画

#endif
