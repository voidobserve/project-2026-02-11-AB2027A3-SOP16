#ifndef _BSP_H
#define _BSP_H

#include "bsp_param.h"
#include "bsp_sys.h"
#include "ble_init.h"
#include "ble_proc.h"
#include "bsp_uart_debug.h"
#include "bsp_saradc.h"
#include "bsp_saradc_vbat.h"
#include "bsp_sdadc_voice.h"
#include "bsp_key.h"
#include "bsp_charge.h"
#include "bsp_huart_iodm.h"
#include "bsp_keyscan.h"
#include "func.h"
#include "msg.h"

#include "prod_test.h"

#if (LE_REOMTE_PRODUCT_ID == LE_REMOTE_DEFAULT) 
#include "ble_service.h"
#elif (LE_REOMTE_PRODUCT_ID == LE_REMOTE_ATVV) 
#include "ble_service_atvv.h"
#endif

#if BSP_IR_EN
#include "bsp_ir.h"
#endif

#endif
