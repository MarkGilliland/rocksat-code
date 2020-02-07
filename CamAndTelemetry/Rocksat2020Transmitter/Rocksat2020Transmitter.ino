/*

************************************************************************************************


ROCKSAT-X COLORADO SCHOOL OF MINES 2020 - PAYLOAD DATA TRANSMISSION
 

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



The following code is intended to receive a JPEG image and transmit that image over the desired 160km

via use of a modified Dipole antenna design, with FLRC protocol.


See Also: https://www.deviceplus.com/how-tos/arduino-guide/jpeg-decoding-on-arduino-tutorial/

For reference on image deconstruction and reconstruction, courtesy of Jan Gromes. 



************************************************************************************************

ARDUINO IDE on RASPBERRY PI


This code was developed to be run in the Raspberry Pi development on RPi0. The following

should be useful in allowing the Arduino IDE to interact on the RPi environment. 

YouTube: Using Arduino IDE to compile and run sketches on the Pi https://www.youtube.com/watch?v=lZvhtfUlY8Y

‘Piduino’ on GitHub: https://github.com/me-no-dev/RasPiArduino

Windows Toolchain for RPi: https://gnutoolchains.com/raspberry/


************************************************************************************************

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

#define SD_CS  53               //Define Slave Select pin
                        



/******************************** FLRC Modem Parameters *************************************/

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

#include <JPEGDecoder.h>

SX1280Class SX1280LT;



void loop()

{
  
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


void initBuff(char* buff) { // Function to fill the packet buffer with zeros
  
  for(int i = 0; i < 240; i++) {
    
    buff[i] = 0;
    
  }
  
}


void start_spi()

{
 
  pinMode(13, OUTPUT); // Set pin 13 to output, otherwise SPI might hang
    
  digitalWrite(_NSS, LOW);  //this brings chip select low to start the SPI transfer
  
  for (index = 0; index < size; index++)    //now use a for loop to send the packet. This was explained in the JPEG decoding
  
  {

    // Start the SD
    
    if(!SD.begin(SD_CS)) 
    {
      
      continue // If the SD can't be started, loop forever
      
    }
  
    File root = SD.open("/"); // Open the root directory
    
    while(!Serial.available()); // Wait for the PC to signal
  
    
    while(true) { // Send all files on the SD card
      
      File jpgFile = root.openNextFile(); // Open the next file
  
      
      if(!jpgFile) { // We have sent all files
        
        break;
        
      }
  
      JpegDec.decodeSdFile(jpgFile); // Decode the JPEG file
  
      char dataBuff[240]; // Create a buffer for the packet
  
      initBuff(dataBuff); // Fill the buffer with zeros
  
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
  
      
      for(int j=0; j<240; j++) { // Send the header packet
        
        SPI.transfer(dataBuff[j]);
        
      }
  
      uint16_t *pImg; // Pointer to the current pixel
  
      uint16_t color; // Color of the current pixel
  
      // Create a data packet with the actual pixel colors
      
      strcpy(dataBuff, "$ITDAT");
      uint8_t i = 6;
  
      
      while(JpegDec.read()) { // Repeat for all MCUs in the image
        
        pImg = JpegDec.pImage; // Save pointer the current pixel
  
        // Get the coordinates of the MCU we are currently processing
        int mcuXCoord = JpegDec.MCUx;
        int mcuYCoord = JpegDec.MCUy;
  
        // Get the number of pixels in the current MCU
        uint32_t mcuPixels = JpegDec.MCUWidth * JpegDec.MCUHeight;
  
        while(mcuPixels--) { // Repeat for all pixels in the current MCU
          
          color = *pImg++; // Read the color of the pixel as 16-bit integer
          
          // Split it into two 8-bit integers
          dataBuff[i] = color >> 8;
          dataBuff[i+1] = color;
          i += 2;
 
          if(i == 240) { // If the packet is full, send it
            
            for(int j=0; j<240; j++) {
              
              SPI.transfer(dataBuff[j]);
              
            }
            
            i = 6;
            
          }
  
          // If we reach the end of the image, send a packet
          if((mcuXCoord == JpegDec.MCUSPerRow - 1) && 
            (mcuYCoord == JpegDec.MCUSPerCol - 1) && 
            (mcuPixels == 1)) {
            
            for(int j=0; j<i; j++) { // Send the pixel values
              
              SPI.transfer(dataBuff[j]);
              
            }
            
            for(int k=i; k<240; k++) { // Fill the rest of the packet with zeros
              
              Serial.write(0);
              
            }
          }
        }
      }
    }
  
    root.close(); // Safely close the root directory
    
  }
      
      SPI.transfer(buffer[index]);    //this part may look different, refer to the JPEG decoding example with buffer = 240
      
    }
  
  digitalWrite(_NSS, HIGH); //bring the CS high to end the SPI transmission // CS, NSS, needs to return high to complete the transfer
  
}


void setup()

{

  Serial.begin(Serial_Monitor_Baud);
  
  SPI.begin();
    
  SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));
  
  SPI.setBitOrder(MSBFIRST);           
  
  SPI.setClockDivider(4000000);  
  
  SPI.setDataMode(SPI_MODE0);
  
  setup_FLRC();

  digitalWrite(_NSS, HIGH); //bring the CS or NSS high, standard SPI needs a high to low transition to start communication

  if (digitalRead (!RFBUSY))  { //if RFBUSY is low, then call the spi function

    start_spi();
    
  }
}
