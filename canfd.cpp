/*



*/

#include "canfd.h"
#include <ACAN2517FD.h>
#include <SPI.h>
#include "ui.h"
#include "ui_helpers.h"

String can1_data;
static const byte MCP2517_SCK  = 20; // SCK input of MCP2517FD
static const byte MCP2517_MOSI = 21; // SDI input of MCP2517FD
static const byte MCP2517_MISO = 22; // SDO output of MCP2517FD

static const byte MCP2517_CS  = 23; // CS input of MCP2517FD
static const byte MCP2517_INT = 26; // INT output of MCP2517FD
ACAN2517FD can (MCP2517_CS, SPI, MCP2517_INT) ;


void canfd_init(void)
{
  Serial.println("Init MCP2518FD");
  SPI.begin (MCP2517_SCK, MCP2517_MISO, MCP2517_MOSI) ;
  Serial.print ("sizeof (ACAN2517FDSettings): ") ;
  Serial.print (sizeof (ACAN2517FDSettings)) ;
  Serial.println (" bytes") ;
  Serial.println ("Configure ACAN2517FD") ;
  
  ACAN2517FDSettings settings (ACAN2517FDSettings::OSC_40MHz, 500 * 1000, DataBitRateFactor::x4) ;
  settings.mDriverReceiveFIFOSize = 200 ;
  settings.mRequestedMode = ACAN2517FDSettings::NormalFD ; // Select loopback mode
  //--- RAM Usage
  Serial.print ("MCP2517FD RAM Usage: ") ;
  Serial.print (settings.ramUsage ()) ;
  Serial.println (" bytes") ;

  //--- Begin
  const uint32_t errorCode = can.begin (settings, [] { can.isr () ; }) ;
  if (errorCode == 0) {
    Serial.print ("Bit Rate prescaler: ") ;
    Serial.println (settings.mBitRatePrescaler) ;
    Serial.print ("Arbitration Phase segment 1: ") ;
    Serial.println (settings.mArbitrationPhaseSegment1) ;
    Serial.print ("Arbitration Phase segment 2: ") ;
    Serial.println (settings.mArbitrationPhaseSegment2) ;
    Serial.print ("Arbitration SJW:") ;
    Serial.println (settings.mArbitrationSJW) ;
    Serial.print ("Actual Arbitration Bit Rate: ") ;
    Serial.print (settings.actualArbitrationBitRate ()) ;
    Serial.println (" bit/s") ;
    Serial.print ("Exact Arbitration Bit Rate ? ") ;
    Serial.println (settings.exactArbitrationBitRate () ? "yes" : "no") ;
    Serial.print ("Arbitration Sample point: ") ;
    Serial.print (settings.arbitrationSamplePointFromBitStart ()) ;
    Serial.println ("%") ;
  }else{
    Serial.print ("Configuration error 0x") ;
    Serial.println (errorCode, HEX) ;
  }
}
void canfd_receiveframe(void)
{
  char buff[5];
  CANFDMessage message;

  if (can.receive(message)) 
  {

    Serial.printf("CAN FD received ID:  %03X Len:%d  Data: ",message.id,message.len);
    sprintf(buff,"%02X",message.id);
    lv_label_set_text(uic_canfdID,buff);
    sprintf(buff,"%d",message.len); 
    lv_label_set_text(uic_canFDLen,buff);
    can1_data = " ";
    // Communicate packet information
    for(int i = 0; i <= message.len - 1; i ++) {
      Serial.printf("%02x ",message.data[i]); // Transmit value from the frame 
      sprintf(buff,"%02X",message.data[i]);
       can1_data += String(" ") + buff;
    }
    Serial.println(" ");
    lv_label_set_text(uic_canFDdata, can1_data.c_str());
   Serial.println(can1_data);
   
  }



}

void canfd_sendframe(void)
{
    static uint8_t d=0;
    CANFDMessage frame ;
    frame.id =  0x7FE ;
    frame.len = 64 ;
    for (uint8_t i=0 ; i<frame.len ; i++) {
      frame.data [i] = i ;
    }

    frame.data[0] = d++;

    const bool ok = can.tryToSend (frame) ;
    if (ok) {
      //gSentFrameCount += 1 ;
    }else Serial.println("Send CAN FD error");


  }

