#include "include.h"

#define GUI_PAGE_HEAD_HEIGHT            (GUI_SCREEN_HEIGHT / 8)
#define GUI_PAGE_BODY_HEIGHT            (GUI_SCREEN_HEIGHT - GUI_PAGE_HEAD_HEIGHT)
#define GUI_PAGE_BODY_CENTER_Y          (GUI_PAGE_HEAD_HEIGHT + GUI_PAGE_BODY_HEIGHT / 2)
#define GUI_PAGE_TIME_WIDTH             (GUI_SCREEN_WIDTH / 3)


/**
 * @brief 创建窗体
          窗体为其他组件的容器
 * @param[in] flag_top : 是否放在界面的顶层
                         false在底层
                         true在顶层
 * @return 返回窗体指针
 **/
compo_form_t *compo_form_create(bool flag_top)
{
    compo_form_t *frm = compo_pool_create(flag_top);
    widget_page_t *page = widget_pool_create(flag_top);
    widget_page_t *page_body = widget_page_create(page);
    widget_text_t *time = widget_text_create(page, 5);

    widget_set_location(page, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
    widget_set_location(page_body, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);

    widget_set_align_center(time, false);
    widget_set_location(time, GUI_SCREEN_WIDTH - GUI_PAGE_TIME_WIDTH, GUI_PAGE_HEAD_HEIGHT / 2, GUI_PAGE_TIME_WIDTH, 32);
    widget_set_visible(time, false);

    frm->type = COMPO_TYPE_FORM;
    frm->page = page;
    frm->time = time;
    frm->page_body = page_body;

    return frm;
}

/**
 * @brief 销毁窗体
 * @param[in] frm : 窗体指针
 **/
void compo_form_destroy(compo_form_t *frm)
{
    if (frm == NULL) {
        return;
    }
    widget_pool_clear(frm->page);
    compo_pool_clear(frm);
}

/**
 * @brief 设置窗体坐标及大小
          注意：该设置默认的坐标是以中心点作为参考点
 * @param[in] frm : 窗体指针
 * @param[in] x : x轴坐标
 * @param[in] y : y轴坐标
 * @param[in] width : 窗体宽度
 * @param[in] height : 窗体高度
 **/
void compo_form_set_location(compo_form_t *frm, s16 x, s16 y, s16 width, s16 height)
{
    if (frm == NULL || frm->page == NULL) {
        halt(HALT_GUI_COMPO_FORM_PTR);
    }
    widget_set_location(frm->page, x, y, width, height);
}


/**
 * @brief 设置窗体坐标
          注意：该设置默认的坐标是以中心点作为参考点
 * @param[in] frm : 窗体指针
 * @param[in] x : x轴坐标
 * @param[in] y : y轴坐标
 **/
void compo_form_set_pos(compo_form_t *frm, s16 x, s16 y)
{
    if (frm == NULL || frm->page == NULL) {
        halt(HALT_GUI_COMPO_FORM_PTR);
    }
    widget_set_pos(frm->page, x, y);
}

/**
 * @brief 更新窗体信息
 * @param[in] frm : 窗体指针
 **/
static void compo_form_page_update(compo_form_t *frm)
{
    if (frm->mode == 0) {
        widget_set_location(frm->page_body, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
        widget_page_set_client(frm->page_body, 0, 0);
    } else {
        widget_set_visible(frm->time, (frm->mode & COMPO_FORM_MODE_SHOW_TIME) != 0);
        widget_set_location(frm->page_body, GUI_SCREEN_CENTER_X, GUI_PAGE_BODY_CENTER_Y, GUI_SCREEN_WIDTH, GUI_PAGE_BODY_HEIGHT);
        widget_page_set_client(frm->page_body, 0, -GUI_PAGE_HEAD_HEIGHT);
    }
}

/**
 * @brief 设置窗体模式
          通常和compo_form_set_title（窗体标题栏）一起调用
 * @param[in] frm : 窗体指针
 * @param[in] mode : COMPO_FORM_MODE_SHOW_TITLE  BIT(0) 标题栏显示文字
                     COMPO_FORM_MODE_SHOW_TIME   BIT(1) 标题栏显示时间
 **/
void compo_form_set_mode(compo_form_t *frm, int mode)
{
    frm->mode = mode;
    compo_form_page_update(frm);
}


/**
 * @brief 窗体中添加图片
          注意：该设置默认的坐标是以中心点作为参考点
 * @param[in] frm : 窗体指针
 * @param[in] res_addr : 图片资源的地址
 * @param[in] x : x轴坐标
 * @param[in] y : y轴坐标
 **/
void compo_form_add_image(compo_form_t *frm, u32 res_addr, s16 x, s16 y)
{
    //背景图不需要复杂功能，使用widget_image更省Buffer
    widget_image_t *image = widget_image_create(frm->page_body, res_addr);
    widget_set_pos(image, x, y);
}

