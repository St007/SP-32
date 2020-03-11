/**
 * @file chartScreen.h
 *
 */

#ifndef CHARTSCREEN_H
#define CHARTSCREEN_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#ifdef LV_CONF_INCLUDE_SIMPLE
#include "lvgl.h"
#include "lv_ex_conf.h"
#else
#include "lvgl.h"
#include "lv_conf.h"
#endif

    lv_obj_t * createChartScreen(lv_obj_t * prevScreen);
    void openCO2();
    void openECO2();
    void openTVOC();
    void openPM25();
    void openPM10();
    void closeChartScreen();
    void processShortTick(uint16_t *pm25, uint16_t *pm10, uint16_t *co2, uint16_t *eco2, uint16_t *tvoc);
    void processLongTick(uint16_t *pm25, uint16_t *pm10, uint16_t *co2, uint16_t *eco2, uint16_t *tvoc);





#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* CHARTSCREEN_H */
