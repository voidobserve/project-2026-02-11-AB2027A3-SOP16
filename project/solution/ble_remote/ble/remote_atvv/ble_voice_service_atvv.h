#ifndef __BLE_VOICE_SERVICE_ATVV_H
#define __BLE_VOICE_SERVICE_ATVV_H
#include "include.h"
#include "ble_service_atvv.h"

#if (LE_REOMTE_PRODUCT_ID == LE_REMOTE_ATVV)

#define BLE_VOICE_CMD_BUF_LEN                4
#define BLE_VOICE_CMD_BUF_MASK               (BLE_VOICE_CMD_BUF_LEN - 1)
#define BLE_VOICE_RX_BUF_LEN                 20
   
#define BLE_VOICE_RING_BUF_LEN               1320      //one pack 132 byte
#define BLE_VOICE_ENC_DATA_LEN               (4+128)   //2byte presample + 2byte index + 128byte偏差值
#define BLE_VOICE_NOTIFY_CTL_BUF_LEN         9 
#define BLE_VOICE_NOTIFY_AUDIO_PKT_NUM       7       
#define BLE_VOICE_NOTIFY_AUDIO_PKT_LEN       20 
#define BLE_VOICE_NOTIFY_AUDIO_END_PKT_LEN   14 
#define BLE_VOICE_NOTIFY_AUDIO_BUF_TOTAL_LEN (6+128)   //6byte header + 128byte data 

//ATVV VOICE TIMEOUT
#define ATVV_VOICE_TIMEOUT                   7000      //ms

/***********************ATVV INFO：Remote <------- ATVV***********************/
//ATVV INFO
#define ATVV_INFO_VERSION                    0x0004
#define ATVV_INFO_CODECS_SUPPORTED           0x0001    //8Khz/16bit
#define ATVV_INFO_FRAME_LEN                  0x0086    //134 bytes
#define ATVV_INFO_CHAR_LEN                   0x0014    //20 bytes
#define ATVV_INFO_MIC_ERROR_CODE             0x0f01 

/***********************ATVV CMD：Remote <------- ATVV***********************/
//ATVV GET CAPS PACKET
#define ATVV_GET_CAPS_CMD                    0x0A
#define ATVV_GET_CAPS_PKT_VERSION_OFS        1         //2byte      
#define ATVV_GET_CAPS_PKT_CODECS_SP_OFS      3         //2byte
   
//ATVV MIC OPEN PACKET   
#define ATVV_MIC_OPEN_CMD                    0x0C   
#define ATVV_MIC_OPEN_PKT_CODEC_USED_OFS     1         //2byte  

//ATVV MIC CLOSE PACKET
#define ATVV_MIC_CLOSE_CMD                   0x0D

/**********************AUDIO DATA: Remote -------> ATVV**********************/
//AUDIO BUF
#define AUDIO_BUF_DATA_LEN                   128
#define AUDIO_BUF_SEQ_L_OFS                  0        //1byte
#define AUDIO_BUF_SEQ_H_OFS                  1        //1byte
#define AUDIO_BUF_ID_OFS                     2        //1byte
#define AUDIO_BUF_PREV_L_OFS                 3        //1byte
#define AUDIO_BUF_PREV_H_OFS                 4        //1byte
#define AUDIO_BUF_INDEX_OFS                  5        //1byte
#define AUDIO_BUF_DATA_OFS                   6        //128byte
    
/***********************CTL CMD: Remote -------> ATVV************************/       
//CTL AUDIO STOP PACKET
#define CTL_AUDIO_STOP_CMD                   0x00
#define CTL_AUDIO_STOP_PKT_LEN               1
#define CTL_AUDIO_STOP_PKT_CMD_OFS           0        //1byte  
  
//CTL AUDIO START PACKET  
#define CTL_AUDIO_START_CMD                  0x04  
#define CTL_AUDIO_START_PKT_LEN              1  
#define CTL_AUDIO_START_PKT_CMD_OFS          0        //1byte  
  
//CTL DPAD SELECT PACKET  
#define CTL_DPAD_SELECT_CMD                  0x07  
#define CTL_DPAD_SELECT_PKT_LEN              1  
#define CTL_DPAD_SELECT_PKT_CMD_OFS          0        //1byte  
  
//CTL START SEARCH PACKET  
#define CTL_START_SEARCH_CMD                 0x08  
#define CTL_START_SEARCH_PKT_LEN             1  
#define CTL_START_SEARCH_PKT_CMD_OFS         0        //1byte  
  
//CTL AUDIO SYNC PACKET  
#define CTL_AUDIO_SYNC_CMD                   0x0A  
#define CTL_AUDIO_SYNC_PKT_LEN               3  
#define CTL_AUDIO_SYNC_PKT_CMD_OFS           0        //1byte      
#define CTL_AUDIO_SYNC_PKT_FRAME_NUM_L_OFS   1        //1byte      
#define CTL_AUDIO_SYNC_PKT_FRAME_NUM_H_OFS   2        //1byte      
  
//CTL GET CAPS RESP PACKET  
#define CTL_GET_CAPS_RESP_CMD                0x0B  
#define CTL_GET_CAPS_RESP_PKT_LEN            9  
#define CTL_GET_CAPS_RESP_PKT_CMD_OFS        0        //1byte      
#define CTL_GET_CAPS_RESP_PKT_VERSION_OFS    1        //2byte      
#define CTL_GET_CAPS_RESP_PKT_CODECS_SP_OFS  3        //2byte      
#define CTL_GET_CAPS_RESP_PKT_FRAME_LEN_OFS  5        //2byte      
#define CTL_GET_CAPS_RESP_PKT_CHAR_LEN_OFS   7        //2byte      
  
//CTL MIC OPEN ERROR PACKET  
#define CTL_MIC_OPEN_ERROR_CMD               0x0C  
#define CTL_MIC_OPEN_ERROR_PKT_LEN            3  
#define CTL_MIC_OPEN_ERROR_PKT_CMD_OFS        0        //1byte      
#define CTL_MIC_OPEN_ERROR_PKT_ERR_CODE_OFS   1        //2byte         

typedef enum {
    VOICE_STATUS_STOP          = 0x00,
    VOICE_STATUS_START         = 0x01,
} VOICE_STATUS_TYPEDEF;

//RX
typedef struct{
    uint8_t len;
    uint8_t buf[BLE_VOICE_RX_BUF_LEN];
    uint16_t handle;
}ble_voice_rx_cmd_t;

typedef struct{
    ble_voice_rx_cmd_t cmd[BLE_VOICE_CMD_BUF_LEN];
    uint8_t cmd_rptr;
    uint8_t cmd_wptr;
}ble_voice_rx_cb_t;

//TX
typedef struct{
    uint8_t ctl_buf[BLE_VOICE_NOTIFY_CTL_BUF_LEN];
    uint8_t voice_buf[BLE_VOICE_NOTIFY_AUDIO_BUF_TOTAL_LEN];
}ble_voice_tx_t;

typedef struct{
    uint8_t voice_status;
    uint8_t exit_clk;
    uint16_t frame_num;
    uint16_t tx_client_config;
    uint16_t ctl_client_config;
    uint32_t timeout_tick;
}ble_voice_var_t;

void ble_voice_service_init(void);
void ble_voice_service_proc(void);
#endif  //LE_REOMTE_PRODUCT_ID

#endif
