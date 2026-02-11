#ifndef __COMMON_H__
#define __COMMON_H__


/* 采样点数 -> PCM16字节数 */
#define SAMPLE_2_PCM16_BYTE(x)              (x*2)
/* PCM16字节数 -> 采样点数 */
#define PCM16_BYTE_2_SAMPLE(x)              (x/2)
/* 采样点数 -> ADPCM块字节数 */
#define SAMPLE_2_ADPCM_BYTE(x)              ((x-1)/2+4)
/* ADPCM块字节数 -> 采样点数 */
#define ADPCM_BYTE_2_SAMPLE(x)              (((x-4)*2)+1)
/* ADPCM块字节数 -> PCM16字节数 */
#define ADPCM_BYTE_2_PCM16_BYTE(x)          (((x-4)*4)+2)

//ERROR CODE
#define USB_ERR_OK                      0x01
#define USB_ERR_BUSY                    0x02
#define USB_ERR_PARSE                   0x03
#define USB_ERR_NO_CAP                  0x04
#define USB_ERR_NO_CONN                 0x05
#define USB_ERR_PKT_TOO_LONG            0x06
#define USB_ERR_UNKNOW                  0xff



#endif // __COMMON_H__
