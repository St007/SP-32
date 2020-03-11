#include "chartScreen.h"
#include <stdio.h>

#define CHART_POINT_NUM     120
#define CHARTS_QTY 5

static lv_obj_t *mainScreen;
static lv_obj_t *_prevScreen;

/*chart selector*/
lv_obj_t * ddlist;

static lv_obj_t * chart_CO2;
static lv_obj_t * chart_eCO2;
static lv_obj_t * chart_TVOC;
static lv_obj_t * chart_PM25;
static lv_obj_t * chart_PM10;

static lv_obj_t * chart_CO2_24h;
static lv_obj_t * chart_eCO2_24h;
static lv_obj_t * chart_TVOC_24h;
static lv_obj_t * chart_PM25_24h;
static lv_obj_t * chart_PM10_24h;

static lv_chart_series_t * co2_ser;
static lv_chart_series_t * eco2_ser;
static lv_chart_series_t * tvoc_ser;
static lv_chart_series_t * pm25_ser;
static lv_chart_series_t * pm10_ser;

static lv_chart_series_t * co2_24h_ser;
static lv_chart_series_t * eco2_24h_ser;
static lv_chart_series_t * tvoc_24h_ser;
static lv_chart_series_t * pm25_24h_ser;
static lv_chart_series_t * pm10_24h_ser;

static void reopenChart();
static void back_click_action(lv_obj_t * touch, lv_event_t event);
static void chart_selector_handler(lv_obj_t * obj, lv_event_t event);
static void switch_24h_handler(lv_obj_t * obj, lv_event_t event);
static void createChart(lv_obj_t **chartPointer, const char * x_strings, const char * y_strings, 
                            uint16_t min_y, uint16_t max_y);

enum chartIndex{ CO2CHART = 0, ECO2CHART, TVOCCHART, PM25CHART, PM10CHART};

static lv_obj_t * charts_1h[CHARTS_QTY] ;
static lv_obj_t * charts_24h[CHARTS_QTY];
static const char* headers[] = {"CO2 ppm", "eCO2 ppm", "TVOC ppb", "PM 2.5 ug/m3", "PM 10 ug/m3"};
lv_obj_t * header_label;
bool is_24_on = false;

uint8_t currentChart = CO2CHART;

lv_obj_t * createChartScreen(lv_obj_t * prevScreen) {

    static lv_style_t label_style_button;
    lv_style_copy(&label_style_button, &lv_style_plain);
    label_style_button.text.font = &lv_font_roboto_16 ;

    lv_coord_t hres = lv_disp_get_hor_res(NULL);
    lv_coord_t vres = lv_disp_get_ver_res(NULL);

    mainScreen = lv_cont_create(lv_scr_act(), NULL);
    lv_obj_set_hidden(mainScreen, true);
    lv_obj_set_size(mainScreen, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_pos(mainScreen, 0, 0);
    _prevScreen = prevScreen;

    header_label = lv_label_create(mainScreen, NULL);
    lv_label_set_style(header_label, LV_LABEL_STYLE_MAIN, &label_style_button);
    lv_obj_align(header_label, NULL, LV_ALIGN_CENTER, 0,-100);

    const char * list_of_values_x_1h = "60\n50\n40\n30\n20\n10\n0";
    const char * list_of_values_x_24h = "24\n20\n16\n12\n8\n4\n0";
    const char * list_of_values_y_CO2 = "4000\n3260\n2520\n1780\n1040\n300";
    const char * list_of_values_y_TVOC = "500\n400\n300\n200\n100\n0";
    const char * list_of_values_y_PM = "300\n240\n180\n120\n60\n0";

    createChart(&chart_CO2, list_of_values_x_1h, list_of_values_y_CO2, 300, 4000);
    createChart(&chart_eCO2, list_of_values_x_1h, list_of_values_y_CO2, 300, 4000);
    createChart(&chart_TVOC, list_of_values_x_1h, list_of_values_y_TVOC, 0, 500);
    createChart(&chart_PM25, list_of_values_x_1h, list_of_values_y_PM, 0, 300);
    createChart(&chart_PM10, list_of_values_x_1h, list_of_values_y_PM, 0, 300);
    charts_1h[CO2CHART] = chart_CO2;
    charts_1h[ECO2CHART] = chart_eCO2;
    charts_1h[TVOCCHART] = chart_TVOC;
    charts_1h[PM25CHART] = chart_PM25;
    charts_1h[PM10CHART] = chart_PM10;

    createChart(&chart_CO2_24h, list_of_values_x_24h, list_of_values_y_CO2, 300, 4000);
    createChart(&chart_eCO2_24h, list_of_values_x_24h, list_of_values_y_CO2, 300, 4000);
    createChart(&chart_TVOC_24h, list_of_values_x_24h, list_of_values_y_TVOC, 0, 500);
    createChart(&chart_PM25_24h, list_of_values_x_24h, list_of_values_y_PM, 0, 300);
    createChart(&chart_PM10_24h, list_of_values_x_24h, list_of_values_y_PM, 0, 300);
    charts_24h[CO2CHART] = chart_CO2_24h;
    charts_24h[ECO2CHART] = chart_eCO2_24h;
    charts_24h[TVOCCHART] = chart_TVOC_24h;
    charts_24h[PM25CHART] = chart_PM25_24h;
    charts_24h[PM10CHART] = chart_PM10_24h;

    co2_ser =  lv_chart_add_series(chart_CO2, LV_COLOR_BLUE);
    eco2_ser =  lv_chart_add_series(chart_eCO2, LV_COLOR_GRAY);
    tvoc_ser =  lv_chart_add_series(chart_TVOC, LV_COLOR_RED);
    pm25_ser =  lv_chart_add_series(chart_PM25, LV_COLOR_GREEN);
    pm10_ser =  lv_chart_add_series(chart_PM10, LV_COLOR_ORANGE);

    co2_24h_ser =  lv_chart_add_series(chart_CO2_24h, LV_COLOR_BLUE);
    eco2_24h_ser =  lv_chart_add_series(chart_eCO2_24h, LV_COLOR_GRAY);
    tvoc_24h_ser =  lv_chart_add_series(chart_TVOC_24h, LV_COLOR_RED);
    pm25_24h_ser =  lv_chart_add_series(chart_PM25_24h, LV_COLOR_GREEN);
    pm10_24h_ser =  lv_chart_add_series(chart_PM10_24h, LV_COLOR_ORANGE);

        /*Set the data series to zero*/
    uint16_t i;
    for(i = 0; i < CHART_POINT_NUM; i++) {
        lv_chart_set_next(chart_CO2, co2_ser, 400);
        lv_chart_set_next(chart_eCO2, eco2_ser, 400);
        lv_chart_set_next(chart_TVOC, tvoc_ser, 0);
        lv_chart_set_next(chart_PM10, pm10_ser, 0);
        lv_chart_set_next(chart_PM25, pm25_ser, 0);

        lv_chart_set_next(chart_CO2_24h, co2_24h_ser, 400);
        lv_chart_set_next(chart_eCO2_24h, eco2_24h_ser, 400);
        lv_chart_set_next(chart_TVOC_24h, tvoc_24h_ser, 0);
        lv_chart_set_next(chart_PM10_24h, pm10_24h_ser, 0);
        lv_chart_set_next(chart_PM25_24h, pm25_24h_ser, 0);
    }

    lv_obj_t * buttonBack = lv_btn_create(mainScreen, NULL);
    lv_obj_set_pos(buttonBack, LV_HOR_RES-70, LV_VER_RES-38);
    static lv_style_t btn_light_style;
    lv_style_copy(&btn_light_style, lv_btn_get_style(buttonBack, LV_BTN_STYLE_REL));
    btn_light_style.body.opa=LV_OPA_30;
    lv_btn_set_style(buttonBack, LV_BTN_STYLE_REL, &btn_light_style);

    lv_obj_t * labelBack = lv_label_create(mainScreen, NULL);
    lv_label_set_text(labelBack, LV_SYMBOL_LEFT "Back");
    lv_label_set_style(labelBack, LV_LABEL_STYLE_MAIN, &label_style_button);
    lv_obj_set_size(buttonBack, lv_obj_get_width(labelBack)+10, lv_obj_get_height(labelBack)+10);
    lv_obj_align(labelBack, buttonBack, LV_ALIGN_CENTER, 0, 0);


    lv_obj_t *switch_24h = lv_sw_create(mainScreen, NULL);
    lv_obj_set_pos(switch_24h, LV_HOR_RES-190, LV_VER_RES-40);
    lv_obj_set_event_cb(switch_24h, switch_24h_handler);

    lv_obj_t * label24h = lv_label_create(mainScreen, NULL);
    lv_label_set_text(label24h, "24h");
    lv_obj_align(label24h, switch_24h, LV_ALIGN_OUT_RIGHT_MID, 0, 0);
    lv_label_set_style(label24h, LV_LABEL_STYLE_MAIN, &label_style_button);

    lv_obj_t * label1h = lv_label_create(mainScreen, NULL);
    lv_label_set_text(label1h, "1h");
    lv_obj_align(label1h, switch_24h, LV_ALIGN_OUT_LEFT_MID, -10, 0);
    lv_label_set_style(label1h, LV_LABEL_STYLE_MAIN, &label_style_button);

    lv_obj_set_event_cb(buttonBack, back_click_action);

        /*Create a drop down chart selector*/
    ddlist = lv_ddlist_create(mainScreen, NULL);
    lv_ddlist_set_options(ddlist, "CO2\n"
            "eCO2\n"
            "TVOC\n"
            "PM2.5\n"
            "PM10");

    lv_ddlist_set_fix_width(ddlist, 80);
    lv_ddlist_set_fix_height(ddlist, 115);
    lv_ddlist_set_draw_arrow(ddlist, true);
    lv_obj_set_auto_realign(ddlist, true);
    lv_obj_set_event_cb(ddlist, chart_selector_handler);
    lv_obj_align(ddlist, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 10, -13);
    static lv_style_t ddlist_style;
    lv_style_copy(&ddlist_style, lv_ddlist_get_style(ddlist, LV_DDLIST_STYLE_BG));
    ddlist_style.text.font = &lv_font_roboto_16;
    lv_ddlist_set_style(ddlist, LV_DDLIST_STYLE_BG, &ddlist_style);
    return mainScreen;
}

void openCO2(){
    currentChart = CO2CHART;
    reopenChart();
}

void openECO2(){
    currentChart = ECO2CHART;
    reopenChart();
}

void openTVOC(){
    currentChart = TVOCCHART;
    reopenChart();
}

void openPM25(){
    currentChart = PM25CHART;
    reopenChart();
}

void openPM10(){
    currentChart = PM10CHART;
    reopenChart();
}

void closeChartScreen(){
        lv_obj_set_hidden(mainScreen, true);
        lv_obj_set_hidden(_prevScreen, false);
}

void processShortTick(uint16_t *pm25, uint16_t *pm10, uint16_t *co2, uint16_t *eco2, uint16_t *tvoc){
    if (*co2 > 5000) (*co2) = 400;
    static uint32_t pm25_24h = 0;
    static uint32_t pm10_24h = 0;
    static uint32_t co2_24h = 0;
    static uint32_t eco2_24h = 0;
    static uint32_t tvoc_24h = 0;
    static uint8_t count = 0;
    count++;
    pm25_24h += *pm25;
    pm10_24h += *pm10;
    co2_24h += *co2;
    eco2_24h += *eco2;
    tvoc_24h += *tvoc;

    /* this function is called 2 per minute, or 24 times per 12 minutes, one tick on 24h graph is per 12 minute*/
    if (count == 24) {
        count = 0;
        pm25_24h = pm25_24h/24;
        pm10_24h = pm10_24h/24;
        co2_24h = co2_24h/24;
        eco2_24h = eco2_24h/24;
        tvoc_24h = tvoc_24h/24;
        lv_chart_set_next(chart_CO2_24h, co2_24h_ser, co2_24h);
        lv_chart_set_next(chart_eCO2_24h, eco2_24h_ser, eco2_24h);
        lv_chart_set_next(chart_TVOC_24h, tvoc_24h_ser, tvoc_24h);
        lv_chart_set_next(chart_PM25_24h, pm25_24h_ser, pm25_24h);
        lv_chart_set_next(chart_PM10_24h, pm10_24h_ser, pm10_24h);
        pm25_24h = 0;
        pm10_24h = 0;
        co2_24h = 0;
        eco2_24h = 0;
        tvoc_24h = 0;
    }
    lv_chart_set_next(chart_CO2, co2_ser, *co2);
    lv_chart_set_next(chart_eCO2, eco2_ser, *eco2);
    lv_chart_set_next(chart_TVOC, tvoc_ser, *tvoc);
    lv_chart_set_next(chart_PM25, pm25_ser, *pm25);
    lv_chart_set_next(chart_PM10, pm10_ser, *pm10);
}

void processLongTick(uint16_t *pm25, uint16_t *pm10, uint16_t *co2, uint16_t *eco2, uint16_t *tvoc){

}

void back_click_action(lv_obj_t * touch, lv_event_t event){
    if (event == LV_EVENT_CLICKED){
        closeChartScreen();
    }
}

static void chart_selector_handler(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_VALUE_CHANGED) {
        uint8_t selection = lv_ddlist_get_selected(obj);
        switch (selection)
        {
        case CO2CHART:
            openCO2();
            break;
        case ECO2CHART:
            openECO2();
            break;
        case TVOCCHART:
            openTVOC();
            break;
        case PM25CHART:
            openPM25();
            break;
        case PM10CHART:
            openPM10();
            break;    
        default:
            break;
        }
    }
}

static void createChart(lv_obj_t **chartPointer, const char * x_strings, const char * y_strings, 
                            uint16_t min_y, uint16_t max_y ){
    
    lv_coord_t hres = lv_disp_get_hor_res(NULL);

    static lv_style_t chart_style_1h;
    lv_style_copy(&chart_style_1h, &lv_style_pretty);
    chart_style_1h.line.width = 1;
    chart_style_1h.line.opa = LV_OPA_30;
    chart_style_1h.body.radius = 1;

    (*chartPointer) = lv_chart_create(mainScreen, NULL); 
    lv_chart_set_style(*chartPointer, LV_CHART_STYLE_MAIN, &chart_style_1h);
    lv_obj_set_size(*chartPointer, hres-60, 100);
    lv_obj_set_pos(*chartPointer, 45, 40);
    lv_chart_set_point_count(*chartPointer, CHART_POINT_NUM);
    lv_chart_set_range(*chartPointer, min_y, max_y);
    lv_chart_set_type(*chartPointer, LV_CHART_TYPE_LINE);
    lv_chart_set_series_width(*chartPointer, 4);
    lv_chart_set_div_line_count(*chartPointer, 4, 5);
    lv_chart_set_x_tick_texts(*chartPointer, x_strings,2, LV_CHART_AXIS_SKIP_LAST_TICK);
    lv_chart_set_y_tick_texts(*chartPointer, y_strings,2, LV_CHART_AXIS_SKIP_LAST_TICK);
    lv_chart_set_x_tick_length(*chartPointer, 3, 2);
    lv_chart_set_y_tick_length(*chartPointer, 3, 2);
    lv_chart_set_margin(*chartPointer, 60);

    lv_obj_set_hidden(*chartPointer, true);
}

static void reopenChart(){
    for (int i =0; i < CHARTS_QTY; i++){
        lv_obj_set_hidden(charts_1h[i], true);
        lv_obj_set_hidden(charts_24h[i], true);
    }
    if (is_24_on){
        lv_obj_set_hidden(charts_24h[currentChart], false);
    } else {
        lv_obj_set_hidden(charts_1h[currentChart], false);
    }
    
    lv_ddlist_set_selected(ddlist, currentChart);
    lv_label_set_text(header_label, headers[currentChart]);
}

static void switch_24h_handler(lv_obj_t * obj, lv_event_t event){
    if(event == LV_EVENT_VALUE_CHANGED) {
         if (lv_sw_get_state(obj)) {
            is_24_on=true;
          } else{
            is_24_on=false;
          }
          reopenChart();
    }
}