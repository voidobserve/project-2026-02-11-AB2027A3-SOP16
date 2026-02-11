/**
******************************************************************************************************************************
*
*@file ble_app.c
*
*@brief Source file for ble application.User can handle custom ble data in this model.
*
*@Create date: 2023-05-27
*
*
*Copyright (c) 2023, BLUETRUM
******************************************************************************************************************************
**/
#include "ble_app.h"
#include "ble_init.h"

app_env_t app_env;

static u32 random_tick;

void ble_app_env_init(void)
{
    memset(&app_env, 0, sizeof(app_env_t));

    memset(app_env.roles, ROLE_UNKNOWN, MAX_BLE_CONNECTIONS);

    app_env.adv_state = ADV_STATE_ADV;
}

uint8_t ble_get_conn_cnt(uint8_t role)
{
    uint8_t conn_cnt = 0;

    uint8_t loop_cnt = MAX_BLE_CONNECTIONS - 1;

    do {
        //printf("app_env.roles[%d] %x, role %x\n", loop_cnt, app_env.roles[loop_cnt], role);
        if (app_env.roles[loop_cnt] == role) {
            conn_cnt ++;
        }
    }while (loop_cnt --);

    return conn_cnt;
}

void ble_adv_scan_state_correct(void)
{
    uint8_t conn_cnt_master = ble_get_conn_cnt(ROLE_MASTER);
    uint8_t conn_cnt_slave  = ble_get_conn_cnt(ROLE_SLAVE);
    printf("conn_cnt_master %d, conn_cnt_slave %d\n", conn_cnt_master, conn_cnt_slave);

    if ((conn_cnt_master < MAX_MASTER_NR) && (conn_cnt_slave < MAX_SLAVE_NR)) {
        app_env.adv_state = ADV_STATE_ADV;
    } else if(conn_cnt_master < MAX_MASTER_NR) {
        ble_adv_dis();
        delay_ms(10);
        ble_scan_en();
    } else if(conn_cnt_slave < MAX_SLAVE_NR) {
        ble_adv_en();
    }
}

void ble_conn_state_set(uint8_t state, uint8_t conidx, uint8_t role)
{
    app_env.conn_state = state;

    if (state) {
        app_env.adv_state = ADV_STATE_CONNECTED;
        random_tick = 0;
    }

    app_env.roles[conidx] = role;
    
    ble_adv_scan_state_correct();
}

uint8_t ble_conn_state_get(void)
{
    return app_env.conn_state;
}

void ble_scan_params_update()
{
    scan_param_t param;

    memset(&param, 0, sizeof(scan_param_t));

    param.scan_type = 0;

    if (ble_conn_state_get()) {
        param.scan_interval = 0x64;
        param.scan_window = 0x50;
    } else {
        param.scan_interval = 0x1e0;
        param.scan_window = 0x50;
    }

    ble_set_scan_param(&param);
}

/*Don't disable adv after connected, only reduce switching time between adv and scan*/
void ble_adv_scan_switch(void)
{
    static u32 tick = 0;

    #if ((MAX_MASTER_NR == 0) || ((MAX_SLAVE_NR == 0)))
    return;
    #endif

    if (1) {
        if(tick_check_expire(tick, random_tick)){
            tick = tick_get();
            //printf("app_env.adv_state %x,",app_env.adv_state);

            if (ADV_STATE_CONNECTED == app_env.adv_state) {
                app_env.adv_state = ADV_STATE_INVALID;
            } else if (ADV_STATE_ADV == app_env.adv_state) {
                ble_scan_dis();
                ble_adv_en();
                
                random_tick = 300;

                /*To avoid thread conflicts */
                if (ADV_STATE_CONNECTED == app_env.adv_state) {                    
                    app_env.adv_state = ADV_STATE_INVALID;
                } else {
                    app_env.adv_state = ADV_STATE_SCAN;
                }
            } else if (ADV_STATE_SCAN == app_env.adv_state) {
                ble_adv_dis();
                ble_scan_en();
                
                random_tick = 200;

                /*To avoid thread conflicts */
                if (ADV_STATE_CONNECTED == app_env.adv_state) {                    
                    app_env.adv_state = ADV_STATE_INVALID;
                } else {
                    app_env.adv_state = ADV_STATE_ADV;
                }
            }
        }
    }
}

void ble_advertising_report_callback(adv_report_t adv_report)
{
    //printf("evt_type %x, addr_type %x, Adv Addr:", adv_report.event_type, adv_report.addr_type);
    //print_r(adv_report.addr, 6);
    //printf("Adv Data: %d###", adv_report.report_len);
    //print_r(adv_report.report, adv_report.report_len);

    uint8_t con_dev_name1[] = "LE_AB2020";
    uint8_t find1 = 0;
    
    for (uint8_t offset = 0; offset < adv_report.report_len - 1;) {
        ble_ltv_t *beacon_ltv = (ble_ltv_t *)&adv_report.report[offset];

        if (beacon_ltv->type == 0x09) {
            if (!strncmp((char *)beacon_ltv->data, (const char *)con_dev_name1, beacon_ltv->len - 1)) {
                find1 = 1;
                printf ("find device %s\n", con_dev_name1);
            }
        }

        if(find1){
            if (ble_get_conn_cnt(ROLE_MASTER) < MAX_MASTER_NR) {
                ble_scan_dis();

                printf("ble_connect_by_addr: ");
                print_r(adv_report.addr, 6);

                le_conn_initiate_err_code_t ret;
                ret = ble_connect_by_addr(adv_report.addr, adv_report.addr_type, 24, 0, 400);
                
                if (ret) {
                    printf("Central initiate a connecetion failed!, reason = %x\n", ret);
                }

                app_env.adv_state = ADV_STATE_CONNECTED;
            }else {
                printf("Master device(%d) reaches the upper limit(%d)\n", ble_get_conn_cnt(ROLE_MASTER), MAX_MASTER_NR);
            }

            break;
        }
        
        offset += beacon_ltv->len + 1;
    }
}

