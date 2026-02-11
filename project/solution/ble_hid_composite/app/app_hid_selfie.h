#ifndef _APP_HID_SELFIE_H
#define _APP_HID_SELFIE_H

/*
 以当前digitizer报表举例,X轴逻辑小值为0,逻辑最大值为LOGICAL_MAX_X,
                        Y轴逻辑小值为0,逻辑最大值为LOGICAL_MAX_Y,
 以竖持手机为例，原点位于手机屏幕左上角,坐标最大值位于屏幕右下角。
                         Screen
(0,0)---------------------------------------(LOGICAL_MAX_X, 0)
  |                                                        |
  |                                                        |
  |                                                        |
  |                                                        |
  |                                                        |
  |                                                        |
  |                                                        |
  |                                                        |
  |                                                        |
  |                                                        |
  |                                                        |
  |                                                        |
  |                                                        |
  |                                                        |
  |                                                        |
  |                                                        |
  |                                                        |
  |                                                        |
  |                                                        |
  |                                                        |
  |                                                        |
  |                                                        |
  |                                                        |
  |                                                        |
  |                                                        |
  |                                                        |
  |                                                        |
  |                                                        |
  |                                                        |
(0, LOGICAL_MAX_Y)----------------------(LOGICAL_MAX_X, LOGICAL_MAX_Y)
*/

/*android*/
#define ANDROID_LOGICAL_MAX_NUM_X           (1000)                              // 该值务必与report map中配置的值保持一致
#define ANDROID_LOGICAL_MAX_NUM_Y           (1000)                              // 该值务必与report map中配置的值保持一致

#define ANDROID_ZOOM_IN_OUT_ORIGIN_Y        (ANDROID_LOGICAL_MAX_NUM_Y * 0.3)   // Y轴配置在屏幕上半部

#define ANDROID_ZOOM_IN_ORIGIN_X            (ANDROID_LOGICAL_MAX_NUM_Y * 0.35)  // 放大时手指1的x坐标设定在屏幕左侧
#define ANDROID_ZOOM_IN_ORIGIN_X1           (ANDROID_LOGICAL_MAX_NUM_Y * 0.65)  // 放大时手指2的x坐标设定在屏幕右侧
#define ANDROID_ZOOM_IN_LIMIT_X             (ANDROID_LOGICAL_MAX_NUM_X * 0.1)   // 放大时手指1的x坐标的边界值

#define ANDROID_ZOOM_OUT_ORIGIN_X           (ANDROID_LOGICAL_MAX_NUM_X * 0.1)   // 缩小时手指1的x坐标设定在屏幕左侧
#define ANDROID_ZOOM_OUT_ORIGIN_X1          (ANDROID_LOGICAL_MAX_NUM_X * 0.9)   // 缩小时手指2的x坐标设定在屏幕右侧
#define ANDROID_ZOOM_OUT_LIMIT_X            (ANDROID_LOGICAL_MAX_NUM_X * 0.35)  // 缩小时手指1的x坐标的边界值

#define ANDROID_ZOOM_IN_STEP_SLOW           (5)
#define ANDROID_ZOOM_IN_STEP_FAST           (25)

#define ANDROID_ZOOM_OUT_STEP_SLOW          (5)
#define ANDROID_ZOOM_OUT_STEP_FAST          (25)

/*ios*/
#define IOS_LOGICAL_MAX_X                   (2729)                              // 该值务必与report map中配置的值保持一致
#define IOS_LOGICAL_MAX_Y                   (1364)                              // 该值务必与report map中配置的值保持一致

#define IOS_ZOOM_IN_OUT_ORIGIN_Y            (IOS_LOGICAL_MAX_Y * 0.4)     		// 初始Y坐标配置在屏幕上半部

#define IOS_ZOOM_IN_ORIGIN_X                (IOS_LOGICAL_MAX_X * 0.4)     		// 放大时手指1的初始x坐标设定在屏幕左侧
#define IOS_ZOOM_IN_ORIGIN_X1               (IOS_LOGICAL_MAX_X * 0.6)     		// 放大时手指2的初始x坐标设定在屏幕右侧
#define IOS_ZOOM_IN_LIMIT_X                 (IOS_LOGICAL_MAX_X * 0.1)     		// 放大时手指1的x坐标的边界值

#define IOS_ZOOM_OUT_ORIGIN_X               (IOS_LOGICAL_MAX_X * 0.2)     		// 缩小时手指1的x坐标设定在屏幕左侧
#define IOS_ZOOM_OUT_ORIGIN_X1              (IOS_LOGICAL_MAX_X * 0.8)     		// 缩小时手指2的x坐标设定在屏幕右侧
#define IOS_ZOOM_OUT_LIMIT_X                (IOS_LOGICAL_MAX_X * 0.5)     		// 缩小时手指1的x坐标的边界值

/*通过调整步进来调节放大速度*/
#define IOS_ZOOM_IN_STEP_SLOW               (5)
#define IOS_ZOOM_IN_STEP_FAST               (25)

/*通过调整步进来调节缩小速度*/
#define IOS_ZOOM_OUT_STEP_SLOW              (5)
#define IOS_ZOOM_OUT_STEP_FAST              (25)


bool ble_hid_event_selfie(u16 msg);

#endif // _APP_HID_SELFIE_H
