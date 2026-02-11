#include "include.h"
#include "driver_saradc.h"
#include "bsp_saradc_tsensor.h"

#if BSP_TSEN_EN

#define TSEN_RF_FREQ_ADJUST         1
#define TRACE_EN                    0

#if TRACE_EN
#define TRACE(...)                  printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

extern void ble_rf_vddvco_set(uint8_t level);
extern uint8_t ble_rf_vddvco_get(void);
extern void ble_rf_pll_set(uint32_t val);

typedef struct {
    uint16_t temper;
    uint32_t val;
}temper_cb_t;


/**
 * @brief ADC values and temperature mapping table.
 *        The actual error of the table is affected by TSEN sensitivity and SARADC accuracy.
 *        And there may be slight differences between different chip individuals.
 */
#define T_STEP                      5
#define T_MIN                       -30
#define T_TABLE_SIZE                33

const u16 tsen_adc_map[T_TABLE_SIZE] = {
//  -30   -25   -20   -15   -10   -5     0    5
    721,  733,  746,  759,  772,  786,  805,  820,
//  10    15    20    25    30    35    40    45
    834,  848,  865,  873,  889,  902,  914,  924,
//  50    55    60    65    70    75    80    85
    947,  956,  965,  993,  1000, 1012, 1027, 1042,
//  90    95    100   105   110   115   120   125
    1057, 1070, 1085, 1094, 1118, 1132, 1144, 1162,
//  130
    1180,
};

#if TSEN_RF_FREQ_ADJUST
const temper_cb_t pll_freq_adjust_tbl[]= {
    {95,  1370000},
    {100, 1350000},
    {105, 1340000},
    {110, 1315000},
    {115, 1300000},
    {120, 1275000},
    {125, 1250000},
    {130, 1220000},
    {135, 1190000},
    {140, 1160000},
    {145, 1120000},
};

static uint32_t temperature_to_pll(int temper)
{
    uint8_t cnt = sizeof(pll_freq_adjust_tbl) / sizeof(pll_freq_adjust_tbl[0]);
    uint8_t i;

    for (i = 0; i < cnt; i++) {
        if (temper <= pll_freq_adjust_tbl[i].temper) {
            return pll_freq_adjust_tbl[i].val;
        }
    }

    return 1398101;
}

static void tsensor_rf_freq_adjust(int temper)
{
    static u8 vco_adjust_flag = 0;
    static u8 bt_freq_offset_adjust_level = 0;

    static uint8_t vddbt_level, rf_vddvco_level;

    TRACE("temper: %d\n", temper);

    if ((temper >= 60) && (vco_adjust_flag == 0)) {
        vco_adjust_flag = 1;
        vddbt_level = pmu_get_vddbt();
        rf_vddvco_level = ble_rf_vddvco_get();
        pmu_set_vddbt(10);      //vddbt 1.35v
        ble_rf_vddvco_set(5);
        TRACE("vco_adjust_flag: %d\n", vco_adjust_flag);
    } else if ((temper <= 40) && (vco_adjust_flag == 1)) {
        vco_adjust_flag = 0;
        TRACE("vco_adjust_flag: %d\n", vco_adjust_flag);
        pmu_set_vddbt(vddbt_level);
        ble_rf_vddvco_set(rf_vddvco_level);
    }

    if ((temper <= 80) && (bt_freq_offset_adjust_level != 0)) {
        bt_freq_offset_adjust_level = 0;
        TRACE("bt_freq_offset_adjust_level: %d\n", bt_freq_offset_adjust_level);
        ble_rf_pll_set(temperature_to_pll(temper));
    } else if ((90 < temper) && (temper <= 100) && (bt_freq_offset_adjust_level != 1)) {
        bt_freq_offset_adjust_level = 1;
        TRACE("bt_freq_offset_adjust_level: %d\n", bt_freq_offset_adjust_level);
        ble_rf_pll_set(temperature_to_pll(temper));
    } else if ((110 <= temper) && (bt_freq_offset_adjust_level != 2)) {
        bt_freq_offset_adjust_level = 2;
        TRACE("bt_freq_offset_adjust_level: %d\n", bt_freq_offset_adjust_level);
        ble_rf_pll_set(temperature_to_pll(temper));
    } else {
        /* Ignore 85/90/105 for debounce */
    }
}

#endif

static void tsensor_adjust_handler(int temper)
{
#if TSEN_RF_FREQ_ADJUST
    tsensor_rf_freq_adjust(temper);
#endif
}


void bsp_tsensor_init(void)
{
    saradc_channel_init_typedef saradc_channel_init_struct;

    /* Initialize SARADC Channel Register */
    saradc_channel_init_struct.channel = ADC_CHANNEL_TSEN | REFERENCE_VOLTAGE_CHANNEL;
    saradc_channel_init_struct.interrupt_en = DISABLE;
    saradc_channel_init_struct.pullup_en = SARADC_PULLUP_DIS;
    saradc_channel_init_struct.setup_time = SARADC_ST_8_CLK;
    saradc_channel_init(&saradc_channel_init_struct);
}

AT(.text.app.proc.tsen)
void bsp_tsensor_process_do(void)
{
    u32 reference_adc_value = 0, tsen_adc_value = 0;
    u16 tsen_voltage_value;
    u8 i, loop = 5;

    /* Average Filtering */
    for (i = 0; i < loop; i++) {
        saradc_kick_start(ADC_CHANNEL_TSEN | REFERENCE_VOLTAGE_CHANNEL);
        while (!(saradc_get_channel_flag(ADC_CHANNEL_TSEN) && saradc_get_channel_flag(REFERENCE_VOLTAGE_CHANNEL)));
        reference_adc_value += saradc_get_data(REFERENCE_VOLTAGE_CHANNEL);
        tsen_adc_value += saradc_get_data(ADC_CHANNEL_TSEN);
    }
    reference_adc_value /= loop;
    tsen_adc_value /= loop;

    /* Calc TSEN Voltage(mV) */
    tsen_voltage_value = REFERENCE_VOLTAGE_VALUE * tsen_adc_value / reference_adc_value;


    //TRACE("tsen_voltage_value: %d\n", tsen_voltage_value);

    int temperature;

    /* Convert to Temperature */
    for (i = 0; i < T_TABLE_SIZE; i++) {
        if (tsen_voltage_value < tsen_adc_map[i]) {
            break;
        }
    }

    if (i == 0) {
        temperature = -30;
    } else if (i == T_TABLE_SIZE) {
        temperature = 130;
    } else {
        if ((tsen_voltage_value - tsen_adc_map[i - 1]) < (tsen_adc_map[i] - tsen_voltage_value)) {
            temperature = T_MIN + (i - 1) * T_STEP;
        } else {
            temperature = T_MIN + i * T_STEP;
        }
    }

    /* Process */
    tsensor_adjust_handler(temperature);
}

AT(.text.app.proc.tsen)
void bsp_tsensor_process(void)
{
    static u32 delay_cnt = 0;

    /* The Interval between each TSEN Sampling */
    if (tick_check_expire(delay_cnt, 1000)) {
        bsp_tsensor_process_do();
        delay_cnt = tick_get();
    }

}

#endif // BSP_TSEN_EN
