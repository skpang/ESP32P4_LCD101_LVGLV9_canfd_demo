/*
For use with :
https://www.skpang.co.uk/collections/esp32-boards/products/esp32p4-with-10-1-800x1280-ips-lcd-can-fd-and-can-bus

Ensure these libraries are installed: 
https://github.com/pierremolinaro/acan2517FD 
https://github.com/handmade0octopus/ESP32-TWAI-CAN

LVGL version: 9.1.0

Move the lv_conf.h file to your /libraries folder.

skpang.co.uk 09/2025

*/
#ifndef BOARD_HAS_PSRAM
#error "Error: This program requires PSRAM enabled, please enable PSRAM option in 'Tools' menu of Arduino IDE"
#endif

#include <ESP32-TWAI-CAN.hpp>
#include <lvgl.h>
#include "canfd.h"
#include "ui.h"
#include "ui_helpers.h"
#include "skp_lcd.h"


#define CAN_TX   32  // Connects to CTX
#define CAN_RX   27  // Connects to CRX

#define ON  LOW
#define OFF HIGH


int LED_R = 31;
int LED_B = 30;
int LED_G = 29;

uint8_t can_start = 1;
uint32_t frame_count = 0;
CanFrame rxFrame; 

void canReceiver() {
  String can2_data;
   char buff[5];

  // try to parse packet
  if(ESP32Can.readFrame(rxFrame, 0)) { // 1000 is the timeout value
    // Communicate that a packet was recieved
    sprintf(buff,"%02X",rxFrame.identifier);
    lv_label_set_text(uic_ccanID, buff);
    sprintf(buff,"%d",rxFrame.data_length_code);
    lv_label_set_text(uic_ccanLen, buff);

    Serial.printf("Classic CAN received ID: %03X Len:%d  Data: ", rxFrame.identifier,rxFrame.data_length_code);
    can2_data = " ";
    // Communicate packet information
    for(int i = 0; i <= rxFrame.data_length_code - 1; i ++) {
      Serial.printf("%02x ",rxFrame.data[i]); // Transmit value from the frame 
      sprintf(buff,"%02X",rxFrame.data[i]);
      can2_data += String(" ") + buff; 
      }
    Serial.println(" ");
     lv_label_set_text(uic_ccCANdata, can2_data.c_str());
  } 
}

void Driver_Loop(void *parameter)
{
  while(1)
  {
      if(can_start == 1)
      {
        canSender();  // call function to send data through CAN
       // canfd_sendframe();
  
      }

    vTaskDelay(pdMS_TO_TICKS(100));
    
  }
}


void setup(void) {
    pinMode(LED_R,OUTPUT);
    pinMode(LED_G,OUTPUT);
    pinMode(LED_B,OUTPUT);
    Serial.begin(115200);
    digitalWrite(LED_B, OFF);
    digitalWrite(LED_G, OFF);
    digitalWrite(LED_R, OFF); 
    digitalWrite(LED_R, ON);
    delay(200); 
    digitalWrite(LED_R, OFF);
    
    digitalWrite(LED_G, ON);
    delay(200); 
    digitalWrite(LED_G, OFF);
    
    digitalWrite(LED_B, ON);
    delay(200); 
    digitalWrite(LED_B, OFF);

    Serial.println("########################################");
    Serial.println("ESP32P4 10.1in LCD CAN and CAN FD demo skpang.co.uk 09/2025");
    psramInit();
    Serial.println((String)"Memory available in PSRAM : " +ESP.getFreePsram());

    // Set the pins
    ESP32Can.setPins(CAN_TX, CAN_RX);

    // Start the CAN bus at 500 kbps
    if(ESP32Can.begin(ESP32Can.convertSpeed(500))) {
      Serial.println("CAN bus started!");
    } else {
      Serial.println("CAN bus failed!");
    }
    canfd_init(); 

    lcd_setup();

    xTaskCreatePinnedToCore(
    Driver_Loop,     
    "Other Driver task",   
    4096,                
    NULL,                 
    3,                    
    NULL,                
    0                    
  );

}
void canSender() {
  static uint8_t i=0;
  digitalWrite(LED_B, HIGH);

  CanFrame testFrame = { 0 };
  testFrame.identifier = 0x7df;  // Sets the ID
  testFrame.extd = 0; // Set extended frame to false
  testFrame.data_length_code = 8; // Set length of data - change depending on data sent
  testFrame.data[0] = i++; // Write data to buffer. data is not sent until writeFrame() is called.
  testFrame.data[1] = 0x12;
  testFrame.data[2] = 0x34;
  testFrame.data[3] = 0x56;
  testFrame.data[4] = 0x78;
  testFrame.data[5] = 0x9a;
  testFrame.data[6] = 0xbc;
  testFrame.data[7] = 0xde;

  ESP32Can.writeFrame(testFrame); // transmit frame

  frame_count++;
  canfd_sendframe();
  digitalWrite(LED_B, LOW);
   
}
void loop() {
  char buff[20];
    
  sprintf(buff,  "%d", frame_count);
  lv_label_set_text(uic_frameCount, buff);
  lv_timer_handler();
  canReceiver();
  canfd_receiveframe();
  vTaskDelay(pdMS_TO_TICKS(5));
}