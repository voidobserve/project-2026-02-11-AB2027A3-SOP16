#ifndef _COMPO_FORM_H
#define _COMPO_FORM_H

#define COMPO_FORM_MODE_SHOW_TITLE              BIT(0)          //标题栏显示文字
#define COMPO_FORM_MODE_SHOW_TIME               BIT(1)          //标题栏显示时间
#define COMPO_FORM_MODE_SHOW_ICON               BIT(2)          //标题栏显示自定义状态图标

typedef struct compo_textbox_t_ {
    COMPO_STRUCT_COMMON;
    widget_text_t *txt;
    compo_roll_cb_t roll_cb;
    bool multiline;             //多行
} compo_textbox_t;


typedef struct compo_form_t_ {
    COMPO_STRUCT_COMMON;
    widget_page_t *page;
    widget_page_t *page_body;
    widget_text_t *time;
    int mode;
} compo_form_t;

/**
 * @brief 创建窗体
          窗体为其他组件的容器
 * @param[in] flag_top : 是否放在界面的顶层
                         false在底层
                         true在顶层
 * @return 返回窗体指针
 **/
compo_form_t *compo_form_create(bool flag_top);

/**
 * @brief 销毁窗体
 * @param[in] frm : 窗体指针
 **/
void compo_form_destroy(compo_form_t *frm);

/**
 * @brief 设置窗体坐标及大小
          注意：该设置默认的坐标是以中心点作为参考点
 * @param[in] frm : 窗体指针
 * @param[in] x : x轴坐标
 * @param[in] y : y轴坐标
 * @param[in] width : 窗体宽度
 * @param[in] height : 窗体高度
 **/
void compo_form_set_location(compo_form_t *frm, s16 x, s16 y, s16 width, s16 height);

/**
 * @brief 设置窗体坐标
          注意：该设置默认的坐标是以中心点作为参考点
 * @param[in] frm : 窗体指针
 * @param[in] x : x轴坐标
 * @param[in] y : y轴坐标
 **/
void compo_form_set_pos(compo_form_t *frm, s16 x, s16 y);

/**
 * @brief 设置窗体模式
          通常和compo_form_set_title（窗体标题栏）一起调用
 * @param[in] frm : 窗体指针
 * @param[in] mode : COMPO_FORM_MODE_SHOW_TITLE  BIT(0) 标题栏显示文字
                     COMPO_FORM_MODE_SHOW_TIME   BIT(1) 标题栏显示时间
 **/
void compo_form_set_mode(compo_form_t *frm, int mode);

/**
 * @brief 窗体中添加图片
          注意：该设置默认的坐标是以中心点作为参考点
 * @param[in] frm : 窗体指针
 * @param[in] res_addr : 图片资源的地址
 * @param[in] x : x轴坐标
 * @param[in] y : y轴坐标
 **/
void compo_form_add_image(compo_form_t *frm, u32 res_addr, s16 x, s16 y);


#endif
