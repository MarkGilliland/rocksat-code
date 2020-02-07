/*

************************************************************************************************


ROCKSAT-X COLORADO SCHOOL OF MINES 2020 - GROUND DATA RECEPTION
 

************************************************************************************************


Base Code is Copyright of the author Stuart Robinson



http://www.LoRaTracker.uk



These programs may be used free of charge for personal, recreational and educational purposes only.



This program, or parts of it, may not be used for or in connection with any commercial purpose without

the explicit permission of the author Stuart Robinson.



************************************************************************************************


Edits completed by Niklas Geschwentner, 2019-2020 Colorado School of Mines. 



The following code was designed for use by the Colorado Mines RockSat-X Team for purposes of 

telemetry between the satellite payload at 160km above ground to Earth, on a project testing the 

removal of untrackable space debris. The SX1280 used in the following code was used in a low-cost 

module for digital data transmission via radio frequency, the DLP-RFS1280. 



The following code is intended to receive a transmitted image from the payload on ground, via use of a

modified Yagi-Uda antenna design, with FLRC protocol.



************************************************************************************************

*/




#define programversion "V1.0"

#define Serial_Monitor_Baud 115300



#include <SPI.h>



/*


********************************** FLRC Definitions ***************************************

//FLRC bandwidth and bit rate
#define FLRC_BR_1_300_BW_1_2 0x45   //1.3Mbs  
#define FLRC_BR_1_000_BW_1_2 0x69   //1.04Mbs 
#define FLRC_BR_0_650_BW_0_6 0x86   //0.65Mbs
#define FLRC_BR_0_520_BW_0_6 0xAA   //0.52Mbs
#define FLRC_BR_0_325_BW_0_3 0xC7   //0.325Mbs
#define FLRC_BR_0_260_BW_0_3 0xEB   //0.26Mbs`
.
//FLRC coding rate`
#define FLRC_CR_1_2  0x00           //coding rate 1:2
#define FLRC_CR_3_4  0x02           //coding rate 3:4
#define FLRC_CR_1_0  0x04           //coding rate 1 

*/



/************************************* Pin Definitions ***************************************/

#define NSS 10

#define RFBUSY A1

#define NRESET 9

#define DIO1 2

#define DIO2 -1                 //not used 

#define DIO3 -1                 //not used                      

                     





/******************************** FLRC Modem Parameters ***********************************/



//FLRC Modem Parameters

#define Frequency 2445000000                     //frequency of transmissions

#define Offset 0                                 //offset frequency for calibration purposes  



#define BandwidthBitRate FLRC_BR_1_300_BW_1_2    //FLRC bandwidth and bit rate, 1.3Mbs               

#define CodingRate FLRC_CR_1_2                   //FLRC coding rate

#define BT RADIO_MOD_SHAPING_BT_1_0              //FLRC BT

#define Sample_Syncword 0x01234567               //FLRC uses syncword





#define packet_delay 500                         //mS delay between packets





#define TXBUFFER_SIZE 16                         //default TX buffer size

#define RXBUFFER_SIZE 16                         //default RX buffer size



/************************************* Include SX1280 **************************************/

#include <SX1280LT.h>

import processing.serial.*;

SX1280Class SX1280LT;

Serial port;


String trimmed; // String to save the trimmed input

byte[] byteBuffer = new byte[240]; // Buffer to save data incoming from Serial port

int x, y, mcuX, mcuY; // The coordinate variables

long startTime; // A variable to measure how long it takes to receive the image

long currentTime; // A variable to save the current time

boolean received = false; // Flag to signal end of transmission

boolean headerRead = false; // Flag to signal reception of header packet

int inColor, r, g, b; // The color of the current pixel

int jpegWidth, jpegHeight, jpegMCUSPerRow, jpegMCUSPerCol, mcuWidth, mcuHeight, mcuPixels; // Image information variables


void loop()

{
  
    // Nothing here
    
    // We don't need to receive the same images over and over again

}





void setup_FLRC()

{

  SX1280LT.setStandby(MODE_STDBY_RC);

  SX1280LT.setRegulatorMode(USE_LDO);

  SX1280LT.setPacketType(PACKET_TYPE_FLRC);

  SX1280LT.setRfFrequency(Frequency, Offset);

  SX1280LT.setBufferBaseAddress(0, 0);

  SX1280LT.setModulationParams(BandwidthBitRate, CodingRate, BT);

  SX1280LT.setPacketParams(PREAMBLE_LENGTH_32_BITS, FLRC_SYNC_WORD_LEN_P32S, RADIO_RX_MATCH_SYNCWORD_1, RADIO_PACKET_VARIABLE_LENGTH, 127, RADIO_CRC_3_BYTES, RADIO_WHITENING_OFF);

  SX1280LT.setDioIrqParams(IRQ_RADIO_ALL, IRQ_RX_DONE, 0, 0);

  SX1280LT.setSyncWord1(Sample_Syncword);

}



void setup(void)

{

  Serial.begin(Serial_Monitor_Baud);

  SPI.begin();

  SPI.setBitOrder(MSBFIRST);           
  
  SPI.setClockDivider(4000000);  
  
  SPI.setDataMode(SPI_MODE0);

  setup_FLRC();
  

  /* Setup the Window to Display */

  size(200, 200); // Set the default window size to 200 by 200 pixels
  
  background(#888888); // Set the background to grey
  
  frameRate(1000000); // Set as high frame rate as we can
  
  port = new Serial(this, "COM30", 115200); // Start the COM port communication, replace "COM30" with the Arduino COM port number
  
  port.buffer(240); // Read 240 bytes at a time
  

  digitalWrite(_NSS, HIGH); //bring the CS or NSS high, standard SPI needs a high to low transition to start communication

  if (digitalRead (!RFBUSY))  { //if RFBUSY is low, then call the spi function

    
    
  }


}
 
