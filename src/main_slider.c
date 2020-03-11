#include "main_slider.h"
#include <stdio.h>
#include "chartScreen.h"

#define REFR_TIME           2000

static lv_obj_t * container;
static lv_task_t * refr_task;

static lv_obj_t * label_CO2;
static lv_obj_t * label_eCO2;
static lv_obj_t * label_TVOC;
static lv_obj_t * label_PM25;
static lv_obj_t * label_PM10;
static lv_obj_t * lable_sys;
static lv_obj_t * label_units1;
static lv_obj_t * label_units2;
static lv_obj_t * label_units3;
static lv_obj_t * label_units4;
static lv_obj_t * label_units5;
static lv_obj_t * chartScreen;


static void sysmon_task(lv_task_t * param);
static void co2_click_action(lv_obj_t * touch, lv_event_t event);
static void eco2_click_action(lv_obj_t * touch, lv_event_t event);
static void tvoc_click_action(lv_obj_t * touch, lv_event_t event);
static void pm25_click_action(lv_obj_t * touch, lv_event_t event);
static void pm10_click_action(lv_obj_t * touch, lv_event_t event);

void main_slider_create(void)
{
    getMeasure = NULL;
    getMeasurePMM = NULL;
    calibrateCO2 =  NULL;
    getMeasureVoltage = NULL;
    refr_task = lv_task_create(sysmon_task, REFR_TIME, LV_TASK_PRIO_LOW, NULL);

    static lv_style_t label_style_big;
    lv_style_copy(&label_style_big, &lv_style_plain);
    label_style_big.text.opa = LV_OPA_100;
    label_style_big.text.font = &lv_font_roboto_28 ;
    
    static lv_style_t label_style_units;
    lv_style_copy(&label_style_units, &lv_style_plain);
    label_style_units.text.opa = LV_OPA_50;

    static lv_style_t label_style_label;
    lv_style_copy(&label_style_label, &lv_style_plain);
    label_style_label.text.opa = LV_OPA_50;
    label_style_label.text.font = &lv_font_roboto_16 ;

    container = lv_cont_create(lv_scr_act(), NULL);
    lv_obj_set_size(container, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_pos(container, 0, 0);

    /*container: just a label*/
    label_CO2 = lv_label_create(container, NULL);
    lv_label_set_text(label_CO2, "0");
    lv_obj_set_pos(label_CO2, 20, 30);
    lv_label_set_style(label_CO2, LV_LABEL_STYLE_MAIN, &label_style_big);

    lv_obj_t * label = lv_label_create(container, NULL);
    lv_label_set_text(label, "CO2");
    lv_label_set_style(label, LV_LABEL_STYLE_MAIN, &label_style_label);
    lv_obj_align(label, label_CO2, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0);

    label_units1 = lv_label_create(container, NULL);
    lv_obj_align(label_units1, label_CO2, LV_ALIGN_OUT_RIGHT_TOP, 0, 0);
    lv_label_set_text(label_units1, "ppm");
    lv_label_set_style(label_units1, LV_LABEL_STYLE_MAIN, &label_style_units);

    label_eCO2 = lv_label_create(container, NULL);
    lv_label_set_text(label_eCO2, "0");
    lv_obj_set_pos(label_eCO2, 120, 30);
    lv_label_set_style(label_eCO2, LV_LABEL_STYLE_MAIN, &label_style_big);

    label = lv_label_create(container, NULL);
    lv_label_set_text(label, "eCO2");
    lv_label_set_style(label, LV_LABEL_STYLE_MAIN, &label_style_label);
    lv_obj_align(label, label_eCO2, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0);

    label_units2 = lv_label_create(container, NULL);
    lv_obj_align(label_units2, label_eCO2, LV_ALIGN_OUT_RIGHT_TOP, 0, 0);
    lv_label_set_text(label_units2, "ppm");
    lv_label_set_style(label_units2, LV_LABEL_STYLE_MAIN, &label_style_units);

    label_TVOC = lv_label_create(container, NULL);
    lv_label_set_text(label_TVOC, "0");
    lv_obj_set_pos(label_TVOC, 220, 30);
    lv_label_set_style(label_TVOC, LV_LABEL_STYLE_MAIN, &label_style_big);
    
    label = lv_label_create(container, NULL);
    lv_label_set_text(label, "TVOC");
    lv_label_set_style(label, LV_LABEL_STYLE_MAIN, &label_style_label);
    lv_obj_align(label, label_TVOC, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0);

    label_units3 = lv_label_create(container, NULL);
    lv_obj_align(label_units3, label_TVOC, LV_ALIGN_OUT_RIGHT_TOP, 0, 0);
    lv_label_set_text(label_units3, "ppb");
    lv_label_set_style(label_units3, LV_LABEL_STYLE_MAIN, &label_style_units);

    label_PM25 = lv_label_create(container, NULL);
    lv_label_set_text(label_PM25, "0");
    lv_obj_set_pos(label_PM25, 20, 130);
    lv_label_set_style(label_PM25, LV_LABEL_STYLE_MAIN, &label_style_big);

    label = lv_label_create(container, NULL);
    lv_label_set_text(label, "PM2.5");
    lv_label_set_style(label, LV_LABEL_STYLE_MAIN, &label_style_label);
    lv_obj_align(label, label_PM25, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0);

    label_units4 = lv_label_create(container, NULL);
    lv_obj_align(label_units4, label_PM25, LV_ALIGN_OUT_RIGHT_TOP, 0, 0);
    lv_label_set_text(label_units4, "ug/m3");
    lv_label_set_style(label_units4, LV_LABEL_STYLE_MAIN, &label_style_units);

    label_PM10 = lv_label_create(container, NULL);
    lv_label_set_text(label_PM10, "0");
    lv_obj_set_pos(label_PM10, 120, 130);
    lv_label_set_style(label_PM10, LV_LABEL_STYLE_MAIN, &label_style_big);

    label = lv_label_create(container, NULL);
    lv_label_set_text(label, "PM10");
    lv_label_set_style(label, LV_LABEL_STYLE_MAIN, &label_style_label);
    lv_obj_align(label, label_PM10, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0);

    label_units5 = lv_label_create(container, NULL);
    lv_obj_align(label_units5, label_PM10, LV_ALIGN_OUT_RIGHT_TOP, 0, 0);
    lv_label_set_text(label_units5, "ug/m3");
    lv_label_set_style(label_units5, LV_LABEL_STYLE_MAIN, &label_style_units);

    lable_sys = lv_label_create(container, NULL);
    lv_label_set_text(lable_sys, "");
    lv_label_set_style(lable_sys, LV_LABEL_STYLE_MAIN, &label_style_label);
    lv_obj_set_pos(lable_sys, 220, 130);

    lv_obj_t* touchCO2 = lv_obj_create(container, NULL);
    lv_obj_set_pos(touchCO2, 20, 30);
    lv_obj_set_size(touchCO2, 80, 80);
    lv_obj_set_event_cb(touchCO2, co2_click_action);
    lv_obj_set_style(touchCO2, &lv_style_transp);

    lv_obj_t* touchECO2 = lv_obj_create(container, NULL);
    lv_obj_set_pos(touchECO2, 120, 30);
    lv_obj_set_size(touchECO2, 80, 80);
    lv_obj_set_event_cb(touchECO2, eco2_click_action);
    lv_obj_set_style(touchECO2, &lv_style_transp);


    lv_obj_t* touchTVOC = lv_obj_create(container, NULL);
    lv_obj_set_pos(touchTVOC, 220, 30);
    lv_obj_set_size(touchTVOC, 80, 80);
    lv_obj_set_event_cb(touchTVOC, tvoc_click_action);
    lv_obj_set_style(touchTVOC, &lv_style_transp);


    lv_obj_t* touchPM25 = lv_obj_create(container, NULL);
    lv_obj_set_pos(touchPM25, 20, 130);
    lv_obj_set_size(touchPM25, 80, 80);
    lv_obj_set_event_cb(touchPM25, pm25_click_action);
    lv_obj_set_style(touchPM25, &lv_style_transp);

    lv_obj_t* touchPM10 = lv_obj_create(container, NULL);
    lv_obj_set_pos(touchPM10, 120, 130);
    lv_obj_set_size(touchPM10, 80, 80);
    lv_obj_set_event_cb(touchPM10, pm10_click_action);
    lv_obj_set_style(touchPM10, &lv_style_transp);

    chartScreen = createChartScreen(container);
}

static void sysmon_task(lv_task_t * param)
{

    (void) param;    /*Unused*/

    LV_LOG_TRACE("sys_mon task started");

    /*Get CPU and memory information */
    uint8_t cpu_busy;
    cpu_busy = 100 - lv_task_get_idle();

    uint8_t mem_used_pct = 0;
#if  LV_MEM_CUSTOM == 0
    lv_mem_monitor_t mem_mon;
    lv_mem_monitor(&mem_mon);
    mem_used_pct = mem_mon.used_pct;
#endif
    static uint16_t measureCO2 = 0;
    static uint16_t measurePMM10 = 0;
    static uint16_t measurePMM25 = 0;
    static uint16_t measureTVOC = 0;
    static uint16_t measureECO2 = 0;
    static uint16_t med_measureCO2 = 0;
    static uint16_t med_measurePMM10 = 0;
    static uint16_t med_measurePMM25 = 0;
    static uint16_t med_measureTVOC = 0;
    static uint16_t med_measureECO2 = 0;
    static uint8_t count = 0;
    uint16_t old_measureCO2 =measureCO2;
    uint16_t old_measurePMM10 = measurePMM10;
    uint16_t old_measurePMM25 = measurePMM25;
    uint16_t old_measureTVOC = measureTVOC;
    uint16_t old_measureECO2 = measureECO2;
    float voltage = 0.0f;
    getMeasure(&measureCO2);
    getMeasurePMM(&measurePMM10, &measurePMM25);
    getMeasureVoltage(&voltage);
    getMeasureTVOC(&measureTVOC, &measureECO2);
    char buf_long[256];
    sprintf(buf_long, "Battery: %.2f V\nCPU: %d %%\nRAM: %d %%",
            voltage,
            cpu_busy,
            mem_mon.used_pct);
    lv_label_set_text(lable_sys, buf_long);


    if (old_measureCO2 != measureCO2) {
        sprintf(buf_long, "%d", measureCO2);
        lv_label_set_text(label_CO2, buf_long);
        lv_obj_align(label_units1, label_CO2, LV_ALIGN_OUT_RIGHT_TOP, 0, 0);
    }
    if (old_measurePMM10 != measurePMM10) {
        sprintf(buf_long, "%d", measurePMM10);
        lv_label_set_text(label_PM10, buf_long);
        lv_obj_align(label_units5, label_PM10, LV_ALIGN_OUT_RIGHT_TOP, 0, 0);
    }
    if (old_measurePMM25 != measurePMM25) {
        sprintf(buf_long, "%d", measurePMM25);
        lv_label_set_text(label_PM25, buf_long);
        lv_obj_align(label_units4, label_PM25, LV_ALIGN_OUT_RIGHT_TOP, 0, 0);
    }
    if (old_measureTVOC != measureTVOC) {
        sprintf(buf_long, "%d", measureTVOC);
        lv_label_set_text(label_TVOC, buf_long);
        lv_obj_align(label_units3, label_TVOC, LV_ALIGN_OUT_RIGHT_TOP, 0, 0);
    }
    if (old_measureECO2 != measureECO2) {
        sprintf(buf_long, "%d", measureECO2);
        lv_label_set_text(label_eCO2, buf_long);
        lv_obj_align(label_units2, label_eCO2, LV_ALIGN_OUT_RIGHT_TOP, 0, 0);
    }
    count ++;
    med_measureCO2 += (measureCO2*10)/15;
    med_measurePMM10 += (measurePMM10*100)/15;
    med_measurePMM25 += (measurePMM25*100)/15;
    med_measureTVOC += (measureTVOC*100)/15;
    med_measureECO2 += (measureECO2*10)/15;
    if (count == 15){
        count =  0;
        med_measureCO2 = med_measureCO2/10;
        med_measurePMM10 = med_measurePMM10/100;
        med_measurePMM25 = med_measurePMM25/100;
        med_measureTVOC = med_measureTVOC/100;
        med_measureECO2 = med_measureECO2/10;
        processShortTick(&med_measurePMM25, &med_measurePMM10, &med_measureCO2, &med_measureECO2, &med_measureTVOC);
        med_measureCO2 = 0;
        med_measurePMM10 = 0;
        med_measurePMM25 = 0;
        med_measureTVOC = 0;
        med_measureECO2 = 0;
    } 

    LV_LOG_TRACE("sys_mon task finished");
}

void co2_click_action(lv_obj_t * touch, lv_event_t event){
    (void) touch;    /*Unused*/
    if (event == LV_EVENT_CLICKED){
        //lv_obj_set_parent(container, NULL);
        lv_obj_set_hidden(container, true);
        lv_obj_set_hidden(chartScreen, false);
    }
    openCO2();
}

void eco2_click_action(lv_obj_t * touch, lv_event_t event){
    (void) touch;    /*Unused*/
    if (event == LV_EVENT_CLICKED){
        //lv_obj_set_parent(container, NULL);
        lv_obj_set_hidden(container, true);
        lv_obj_set_hidden(chartScreen, false);
    }
    openECO2();
}

void tvoc_click_action(lv_obj_t * touch, lv_event_t event){
    (void) touch;    /*Unused*/
    if (event == LV_EVENT_CLICKED){
        //lv_obj_set_parent(container, NULL);
        lv_obj_set_hidden(container, true);
        lv_obj_set_hidden(chartScreen, false);
    }
    openTVOC();
}

void pm25_click_action(lv_obj_t * touch, lv_event_t event){
    (void) touch;    /*Unused*/
    if (event == LV_EVENT_CLICKED){
        //lv_obj_set_parent(container, NULL);
        lv_obj_set_hidden(container, true);
        lv_obj_set_hidden(chartScreen, false);
    }
    openPM25();
}

void pm10_click_action(lv_obj_t * touch, lv_event_t event){
    (void) touch;    /*Unused*/
    if (event == LV_EVENT_CLICKED){
        //lv_obj_set_parent(container, NULL);
        lv_obj_set_hidden(container, true);
        lv_obj_set_hidden(chartScreen, false);
    }
    openPM10();
}