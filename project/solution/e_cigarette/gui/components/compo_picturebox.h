#ifndef _COMPO_PICTUREBOX_H
#define _COMPO_PICTUREBOX_H

typedef struct compo_picturebox_t_ {
    COMPO_STRUCT_COMMON;
    u8 radix;
    widget_image_t *img;
} compo_picturebox_t;

/**
 * @brief 创建一个图像框组件
 * @param[in] frm : 窗体指针
 * @param[in] res_addr : 图像资源地址
 * @return 返回图像指针
 **/
compo_picturebox_t *compo_picturebox_create(compo_form_t *frm, u32 res_addr);

/**
 * @brief 设置图像
          用于改变图像组件中的图片
 * @param[in] picbox : 图像指针
 * @param[in] res_addr : 图像资源地址
 **/
void compo_picturebox_set(compo_picturebox_t *picbox, u32 res_addr);

/**
 * @brief 剪切图像
          用于剪切图像组件中的图片
 * @param[in] picbox : 图像指针
 * @param[in] index :  截取索引：0 ~ total_cnt-1
 * @param[in] total_cnt : 几等份
 **/
void compo_picturebox_cut(compo_picturebox_t *picbox, u8 index, u8 total_cnt);

/**
 * @brief 设置图像框组件的坐标
          注意：该设置默认的坐标是以中心点作为参考点
 * @param[in] picbox : 图像指针
 * @param[in] x : x轴坐标
 * @param[in] y : y轴坐标
 **/
void compo_picturebox_set_pos(compo_picturebox_t *picbox, s16 x, s16 y);

/**
 * @brief 设置图像框组件的大小
          图片缩放时，需要经过转换工具勾上 需要旋转压缩 的选项
 * @param[in] picbox : 图像指针
 * @param[in] width : 图像框宽度
 * @param[in] height : 图像框高度
 **/
void compo_picturebox_set_size(compo_picturebox_t *picbox, s16 width, s16 height);

/**
 * @brief 设置图像框是否可见
 * @param[in] picbox : 图像指针
 * @param[in] visible : true  可见
                        false 不可见
 **/
void compo_picturebox_set_visible(compo_picturebox_t *picbox, bool visible);

/**
 * @brief 判断图像框是否可见
 * @param[in] picbox : 图像指针
 * @return 图像框是否可见
 **/
bool compo_picturebox_get_visible(compo_picturebox_t *picbox);

/**
 * @brief 获取图像框的坐标和大小
 * @param[in] picbox : 图像指针
 * @return 返回图像的坐标和大小
 **/
rect_t compo_picturebox_get_size(compo_picturebox_t *picbox);

#endif
