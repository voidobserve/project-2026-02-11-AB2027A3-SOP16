#include "include.h"

#define TRACE_EN                1

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

/**
 * @brief 创建一个按钮
 * @param[in] frm : 窗体指针
 * @return 返回按钮指针
 **/
compo_button_t *compo_button_create(compo_form_t *frm)
{
    compo_button_t *btn = compo_create(frm, COMPO_TYPE_BUTTON);
    widget_page_t *page = widget_page_create(frm->page_body);
    btn->widget = page;
    return btn;
}

/**
 * @brief 根据图像创建一个按钮
 * @param[in] frm : 窗体指针
 * @param[in] res_addr : 图像资源的地址
 * @return 返回按钮指针
 **/
compo_button_t *compo_button_create_by_image(compo_form_t *frm, u32 res_addr)
{
    compo_button_t *btn = compo_create(frm, COMPO_TYPE_BUTTON);
    widget_image_t *img = widget_image_create(frm->page_body, res_addr);
    btn->widget = img;
    return btn;
}

/**
 * @brief 设置按钮坐标及大小
          注意：该设置默认的坐标是以中心点作为参考点
 * @param[in] btn : 按钮指针
 * @param[in] x : x轴坐标
 * @param[in] y : y轴坐标
 * @param[in] width : 按钮宽度
 * @param[in] height : 按钮高度
 **/
void compo_button_set_location(compo_button_t *btn, s16 x, s16 y, s16 width, s16 height)
{
    widget_set_location(btn->widget, x, y, width, height);
}

/**
 * @brief 设置按钮坐标
          注意：该设置默认的坐标是以中心点作为参考点
 * @param[in] btn : 按钮指针
 * @param[in] x : x轴坐标
 * @param[in] y : y轴坐标
 **/
void compo_button_set_pos(compo_button_t *btn, s16 x, s16 y)
{
    widget_set_pos(btn->widget, x, y);
}

/**
 * @brief 设置按钮大小
 * @param[in] btn : 按钮指针
 * @param[in] width : 宽度
 * @param[in] height : 高度
 **/
void compo_button_set_size(compo_button_t *btn, s16 width, s16 height)
{
    widget_set_size(btn->widget, width, height);
}

/**
 * @brief 设置按键是否可见
 * @param[in] btn : 按钮指针
 * @param[in] visible : true  可见
                        false 不可见
 **/
void compo_button_set_visible(compo_button_t *btn, bool visible)
{
    widget_set_visible(btn->widget, visible);
}

/**
 * @brief 更换按钮资源地址
 * @param[in] btn : 按钮指针
 * @param[in] addr : 图像资源的地址
 **/
void compo_button_set_bgimg(compo_button_t *btn, u32 addr)
{
    if (!widget_is_page(btn->widget)) {
        widget_image_set(btn->widget, addr);
    }
}
