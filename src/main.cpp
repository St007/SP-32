#include <Arduino.h>

#include <SPI.h>
#include <lvgl.h>
#include <Ticker.h>
#include <TFT_eSPI.h> // Hardware-specific library
#include <SDS011.h>

#define LVGL_TICK_PERIOD 20
#define LV_USE_TUTORIALS
#include "main_slider.h"
#define MHZ19_SERIAL_RX 33
#define MHZ19_SERIAL_TX 27
#define SDS011_SERIAL_RX 35
#define SDS011_SERIAL_TX 32
#define TFT_LED 23
#define BAT_PLUS 34 //A1.6
#define ADC_VOLT_CONST 0.00176

#define I2C_SDA 18 //redefined in Adafruit_CCS811.c

#include <MHZ19.h>
#include "Adafruit_CCS811.h"


Adafruit_CCS811 ccs;
MHZ19 co2;
SDS011 sds011;
Ticker tick; /* timer for interrupt handler */
TFT_eSPI tft = TFT_eSPI(); /* TFT instance */
static lv_disp_buf_t disp_buf;
static lv_color_t buf[LV_HOR_RES_MAX * 10];
lv_obj_t *label;
float _PM25 = 0;
float _PM10 = 0;
//void touch_calibrate();
#if USE_LV_LOG != 0
/* Serial debugging */
void my_print(lv_log_level_t level, const char * file, uint32_t line, const char * dsc)
{

  Serial.printf("%s@%d->%s\r\n", file, line, dsc);
  delay(100);
}
#endif
void touch_calibrate(){
  uint16_t calData[5];
  uint8_t calDataOK = 0;

  // Calibrate
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(20, 0);
  tft.setTextFont(2);
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);

  tft.println("Touch corners as indicated");

  tft.setTextFont(1);
  tft.println();

  tft.calibrateTouch(calData, TFT_MAGENTA, TFT_BLACK, 15);

  Serial.println(); Serial.println();
  Serial.println("// Use this calibration code in setup():");
  Serial.print("  uint16_t calData[5] = ");
  Serial.print("{ ");

  for (uint8_t i = 0; i < 5; i++)
  {
    Serial.print(calData[i]);
    if (i < 4) Serial.print(", ");
  }

  Serial.println(" };");
  Serial.print("  tft.setTouch(calData);");
  Serial.println(); Serial.println();

  tft.fillScreen(TFT_BLACK);
  
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.println("Calibration complete!");
  tft.println("Calibration code sent to Serial port.");

  delay(4000);
}

void measureCO2(uint16_t *value){
      if (co2.isReady()) { // Checking if sensor had preheated for 3 mins
      int co2ppm = co2.readValue(); // Reading CO2 value. (Returns -1 if response wasn't received)
      *value = co2ppm;
    } else {
      *value = 0;
    }
    
}
void measurePMM(uint16_t *pm25, uint16_t *pm10){
  *pm25 = _PM25;
  *pm10 = _PM10;
}

void calibrateCO2Action(){
  co2.calibrateZero();
}

void measureTVOC(uint16_t *tvoc, uint16_t *eco2){
  if(ccs.available()){
    if(!ccs.readData()){
      Serial.print("CO2: ");
      Serial.print(ccs.geteCO2());
      Serial.print("ppm, TVOC: ");
      Serial.print(ccs.getTVOC());
    }
    else{
      Serial.println("ERROR!");
      //while(1);
    }
  }
  *eco2 = ccs.geteCO2();
  *tvoc = ccs.getTVOC();

}

void measureBattVoltage(float *f){
  int rawValue = analogRead(BAT_PLUS);
  *f = ADC_VOLT_CONST*rawValue;
}
/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
  uint16_t c;

  tft.startWrite(); /* Start new TFT transaction */
  tft.setAddrWindow(area->x1, area->y1, (area->x2 - area->x1 + 1), (area->y2 - area->y1 + 1)); /* set the working window */
  for (int y = area->y1; y <= area->y2; y++) {
    for (int x = area->x1; x <= area->x2; x++) {
      c = color_p->full;
      tft.writeColor(c, 1);
      color_p++;
    }
  }
  tft.endWrite(); /* terminate TFT transaction */
  lv_disp_flush_ready(disp); /* tell lvgl that flushing is done */
}

/* Interrupt driven periodic handler */
static void lv_tick_handler(void)
{

  lv_tick_inc(LVGL_TICK_PERIOD);
}

/* Reading input device (simulated encoder here) */
bool read_touchpad(lv_indev_drv_t * indev, lv_indev_data_t * data)
{
    static lv_coord_t last_x = 0;
    static lv_coord_t last_y = 0;

    /*Save the state and save the pressed coordinate*/
  static uint16_t x=0;
  static uint16_t y=0;
  boolean pressed = tft.getTouch(&x, &y);

  // Draw a white spot at the detected coordinates
  if (pressed) {
    //tft.fillCircle(x, y, 2, TFT_WHITE);
    last_x = x;
    last_y = y;
  }
    data->state = pressed ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
    //if(data->state == LV_INDEV_STATE_PR) touchpad_get_xy(&last_x, &last_y);

    /*Set the coordinates (if released use the last pressed coordinates)*/
    data->point.x = x;
    data->point.y = y;

    return false; /*Return `false` because we are not buffering and no more data to read*/
}

void setup() {
  Serial.begin(115200); /* prepare for possible serial debug */
    pinMode(TFT_LED, OUTPUT);
   digitalWrite(TFT_LED, HIGH);
  if(!ccs.begin()){
    Serial.println("Failed to start sensor! Please check your wiring.");
    while(1);
  }
  while(!ccs.available());
  Serial2.begin(9600, SERIAL_8N1, MHZ19_SERIAL_RX, MHZ19_SERIAL_TX);
  Serial1.begin(9600, SERIAL_8N1, SDS011_SERIAL_RX, SDS011_SERIAL_TX);
  co2.setSerial(&Serial2); // pass swSerial to let it use it
  co2.setRange(5000);
  co2.enableABC();
  //co2.disableABC();
  sds011.setup(&Serial1);
    sds011.onData([](float pm25Value, float pm10Value) {
    _PM10 = pm10Value;
    _PM25 = pm25Value;
  });
  sds011.onResponse([](uint8_t data1, uint8_t data2, uint8_t data3){
    // command has been executed
  });
  sds011.onError([](int8_t error){
    // error happened
    // -1: CRC error
  });
  sds011.setWorkingPeriod(20);
  lv_init();

#if USE_LV_LOG != 0
  lv_log_register_print(my_print); /* register print function for debugging */
#endif
  tft.init();
  tft.begin(); /* TFT init */
  tft.setRotation(3); /* Landscape orientation */
  // touch_calibrate();
  //  tft.fillScreen(TFT_BLACK);
  // tft.drawCentreString("Touch screen to test!",tft.width()/2, tft.height()/2, 2);
  uint16_t calData[5] = { 211, 3492, 322, 3434, 7 };
  tft.setTouch(calData);
  lv_disp_buf_init(&disp_buf, buf, NULL, LV_HOR_RES_MAX * 10);

  /*Initialize the display*/
  lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = 320;
  disp_drv.ver_res = 240;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.buffer = &disp_buf;
  lv_disp_drv_register(&disp_drv);


  /*Initialize the touch pad*/
  lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = read_touchpad;
  lv_indev_drv_register(&indev_drv);

  /*Initialize the graphics library's tick*/
  tick.attach_ms(LVGL_TICK_PERIOD, lv_tick_handler);

  main_slider_create();
  getMeasure = measureCO2;
  getMeasurePMM = measurePMM;
  calibrateCO2 = calibrateCO2Action;
  getMeasureVoltage = measureBattVoltage;
  getMeasureTVOC =  measureTVOC;

  /* magic code to make Serial2 working after cold start */
  Serial2.begin(9600, SERIAL_8N1, MHZ19_SERIAL_RX, MHZ19_SERIAL_TX);
  co2.setSerial(&Serial2); // pass swSerial to let it use it
  co2.setRange(5000);
}


void loop() {
  lv_task_handler(); /* let the GUI do its work */
  sds011.loop();
}


