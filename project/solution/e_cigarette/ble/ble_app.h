/**
******************************************************************************************************************************
*
*@file ble_app.h
*
*@brief Head file for ble application.User can handle custom ble data in this model.
*
*@Create date: 2023-05-27
*
*
*Copyright (c) 2023, BLUETRUM
******************************************************************************************************************************
**/
#ifndef __BLE_APP_H
#define __BLE_APP_H

#include "include.h"

/// Constant defining the role
enum
{
    ///Master role
    ROLE_MASTER,
    ///Slave role
    ROLE_SLAVE,
    
    ROLE_UNKNOWN = 0xff,
};

typedef struct
{
    uint8_t len;
    uint8_t type;
    uint8_t data[31 - 1];               //ADV_DATA_LEN - sizeof(len)
}__attribute__ ((__packed__)) ble_ltv_t;

typedef enum
{
    ADV_STATE_SCAN,
    ADV_STATE_ADV,
    ADV_STATE_CONNECTED,
    ADV_STATE_INVALID,
}__attribute__ ((__packed__)) adv_state_t;

typedef struct
{
    uint8_t conn_state;
    uint8_t adv_state;
    // Connection Index
    uint8_t conidx;

    // Connection role.
    uint8_t roles[MAX_BLE_CONNECTIONS];
}app_env_t;

void ble_adv_scan_switch(void);
void ble_app_env_init(void);
void ble_conn_state_set(uint8_t state, uint8_t conidx, uint8_t role);
void ble_advertising_report_callback(adv_report_t adv_report);
uint8_t ble_get_conn_cnt(uint8_t role);
void ble_adv_scan_state_correct(void);

#endif
