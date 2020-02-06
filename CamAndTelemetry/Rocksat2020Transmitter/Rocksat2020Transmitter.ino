/*

******************************************************************************************************


ROCKSAT-X COLORADO SCHOOL OF MINES 2020 - PAYLOAD DATA TRANSMISSION
 

******************************************************************************************************


Base Code is Copyright of the author Stuart Robinson



http://www.LoRaTracker.uk



These programs may be used free of charge for personal, recreational and educational purposes only.



This program, or parts of it, may not be used for or in connection with any commercial purpose without

the explicit permission of the author Stuart Robinson.



******************************************************************************************************


Edits completed by Niklas Geschwentner, 2019-2020 Colorado School of Mines. 



The following code was designed for use by the Colorado Mines RockSat-X Team for purposes of 

telemetry between the satellite payload at 160km above ground to Earth, on a project testing the 

removal of untrackable space debris. The SX1280 used in the following code was used in a low-cost 

module for digital data transmission via radio frequency, the DLP-RFS1280. 



The following code is intended to receive a JPEG image and transmit that image over the desired 160km

via use of a modified Dipole antenna design, with FLRC protocol.


See: https://www.deviceplus.com/how-tos/arduino-guide/jpeg-decoding-on-arduino-tutorial/



******************************************************************************************************

*/




#define programversion "V1.0"

#define Serial_Monitor_Baud 115200



#include <SPI.h>



/************************************* Pin Definitions ***************************************/

#define NSS 10

#define RFBUSY A1

#define NRESET 9

#define DIO1 -1                 //not used

#define DIO2 -1                 //not used 

#define DIO3 -1                 //not used                                  



/************************************* FLRC Modem Parameters ***************************************/

#define Frequency 2445000000                     //frequency of transmissions

#define Offset 0                                 //offset frequency for calibration purposes  



#define BandwidthBitRate FLRC_BR_1_300_BW_1_2    //FLRC bandwidth and bit rate, 1.3Mbs               

#define CodingRate FLRC_CR_1_2                   //FLRC coding rate

#define BT RADIO_MOD_SHAPING_BT_1_0              //FLRC BT

#define Sample_Syncword 0x01234567               //FLRC uses syncword



#define PowerTX  1                               //power for transmissions in dBm



#define packet_delay 1000                        //mS delay between packets



#define TXBUFFER_SIZE 16                         //default TX buffer size  

#define RXBUFFER_SIZE 16                         //default RX buffer size

#define SD_CS  53                                // Define Slave Select pin




/************************************* Include SX1280 ***************************************/

#include <SX1280LT.h>

SX1280Class SX1280LT;



void loop(){
    // Nothing here
    // We don't need to send the same images over and over again

}


void setup_FLRC()

{
  SX1280LT.pinInit(NSS, NRESET, RFBUSY, DIO1, DIO2, DIO3)

  SX1280LT.setStandby(MODE_STDBY_RC);

  SX1280LT.setRegulatorMode(USE_LDO);

  SX1280LT.setPacketType(PACKET_TYPE_FLRC);

  SX1280LT.setRfFrequency(Frequency, Offset);

  SX1280LT.setBufferBaseAddress(0, 0);

  SX1280LT.setModulationParams(BandwidthBitRate, CodingRate, BT);

  SX1280LT.setPacketParams(PREAMBLE_LENGTH_32_BITS, FLRC_SYNC_WORD_LEN_P32S, RADIO_RX_MATCH_SYNCWORD_1, RADIO_PACKET_VARIABLE_LENGTH, 127, RADIO_CRC_3_BYTES, RADIO_WHITENING_OFF);

  SX1280LT.setDioIrqParams(IRQ_RADIO_ALL, (IRQ_TX_DONE + IRQ_RX_TX_TIMEOUT), 0, 0);              //set for IRQ on TX done and timeout on DIO1

  SX1280LT.setSyncWord1(Sample_Syncword);

}





void setup()

{



  Serial.begin(Serial_Monitor_Baud);

  Serial.println();

  Serial.print(__TIME__);

  Serial.print(F(" "));

  Serial.println(__DATE__);

  Serial.println(F(programversion));

  Serial.println();



  Serial.println(F("14_SX1280LT_FLRC_Simple_TX Starting"));



  SPI.begin();

  SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));



  if (SX1280LT.begin(NSS, NRESET, RFBUSY, DIO1, DIO2, DIO3))

  {

    Serial.println(F("Device found"));

    delay(1000);

  }


  setup_FLRC();



  Serial.println(F("FLRC Transmitter ready"));

  Serial.println();


  /************************************* JPEG --> BITMAP *******************************

  // Set pin 13 to output, otherwise SPI might hang
  pinMode(13, OUTPUT);

  // Begin Serial port for communication with PC
  Serial.begin(115200);

  // Start the SD
  if(!SD.begin(SD_CS)) {
    // If the SD can't be started, loop forever
    Serial.println("SD failed or not present!");
    while(1);
  }

  // Open the root directory
  File root = SD.open("/");
  
  // Wait for the PC to signal
  while(!Serial.available());

  // Send all files on the SD card
  while(true) {
    // Open the next file
    File jpgFile = root.openNextFile();

    // We have sent all files
    if(!jpgFile) {
      break;
    }

    // Decode the JPEG file
    JpegDec.decodeSdFile(jpgFile);

    // Create a buffer for the packet
    char dataBuff[240];

    // Fill the buffer with zeros
    initBuff(dataBuff);

    // Create a header packet with info about the image
    String header = "$ITHDR,";
    header += JpegDec.width;
    header += ",";
    header += JpegDec.height;
    header += ",";
    header += JpegDec.MCUSPerRow;
    header += ",";
    header += JpegDec.MCUSPerCol;
    header += ",";
    header += jpgFile.name();
    header += ",";
    header.toCharArray(dataBuff, 240);

    // Send the header packet
    for(int j=0; j<240; j++) {
      Serial.write(dataBuff[j]);
    }

    // Pointer to the current pixel
    uint16_t *pImg;

    // Color of the current pixel
    uint16_t color;

    // Create a data packet with the actual pixel colors
    strcpy(dataBuff, "$ITDAT");
    uint8_t i = 6;

    // Repeat for all MCUs in the image
    while(JpegDec.read()) {
      // Save pointer the current pixel
      pImg = JpegDec.pImage;

      // Get the coordinates of the MCU we are currently processing
      int mcuXCoord = JpegDec.MCUx;
      int mcuYCoord = JpegDec.MCUy;

      // Get the number of pixels in the current MCU
      uint32_t mcuPixels = JpegDec.MCUWidth * JpegDec.MCUHeight;

      // Repeat for all pixels in the current MCU
      while(mcuPixels--) {
        // Read the color of the pixel as 16-bit integer
        color = *pImg++;
        
        // Split it into two 8-bit integers
        dataBuff[i] = color >> 8;
        dataBuff[i+1] = color;
        i += 2;

        // If the packet is full, send it
        if(i == 240) {
          for(int j=0; j<240; j++) {
            Serial.write(dataBuff[j]);
          }
          i = 6;
        }

        // If we reach the end of the image, send a packet
        if((mcuXCoord == JpegDec.MCUSPerRow - 1) && 
          (mcuYCoord == JpegDec.MCUSPerCol - 1) && 
          (mcuPixels == 1)) {
          
          // Send the pixel values
          for(int j=0; j<i; j++) {
            Serial.write(dataBuff[j]);
          }
          
          // Fill the rest of the packet with zeros
          for(int k=i; k<240; k++) {
            Serial.write(0);
          }
        }
      }
    }
  }

  // Safely close the root directory
  root.close(); 
}

// Function to fill the packet buffer with zeros
void initBuff(char* buff) {
  for(int i = 0; i < 240; i++) {
    buff[i] = 0;
  }
}
  






  ************************************* JPEG --> BITMAP ********************************/


  

}
