/**
 * @file main_slider.h
 *
 */

#ifndef MAIN_SLIDER_H
#define MAIN_SLIDER_H

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


/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize the system monitor
 */
void main_slider_create(void);
void (*getMeasure)(uint16_t *);
void (*getMeasurePMM)(uint16_t *, uint16_t*);
void (*getMeasureTVOC)(uint16_t *, uint16_t*);
void (*calibrateCO2)();
void (*getMeasureVoltage)(float *);

/**********************
 *      MACROS
 **********************/



#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* MAIN_SLIDER_H */