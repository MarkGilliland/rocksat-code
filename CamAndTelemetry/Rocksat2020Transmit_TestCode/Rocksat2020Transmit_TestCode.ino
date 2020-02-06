/*

******************************************************************************************************


ROCKSAT-X COLORADO SCHOOL OF MINES 2020 - PAYLOAD DATA TRANSMISSION TEST CODE
 

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

#define DIO1 2

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



/************************************* Include SX1280 ***************************************/

#include <SX1280LT.h>



SX1280Class SX1280LT;



boolean SendOK;

int8_t TestPower;

uint8_t TXPacketL;



void loop()

{

  digitalWrite(LED1, HIGH);

  Serial.print(PowerTX);

  Serial.print(F("dBm "));

  Serial.print(F("TestPacket> "));

  Serial.flush();



  if (Send_Test_Packet())

  {

    packet_is_OK();

  }

  else

  {

    packet_is_Error();

  }

  Serial.println();

  delay(packet_delay);

}





void packet_is_OK()

{

  Serial.print(F(" "));

  Serial.print(TXPacketL);

  Serial.print(F(" Bytes SentOK"));

}





void packet_is_Error()

{

  uint16_t IRQStatus;

  IRQStatus = SX1280LT.readIrqStatus();                    //get the IRQ status

  Serial.print(F("SendError,"));

  Serial.print(F("Length,"));

  Serial.print(TXPacketL);

  Serial.print(F(",IRQreg,"));

  Serial.print(IRQStatus, HEX);

  SX1280LT.printIrqStatus();

  digitalWrite(LED1, LOW);                       //this leaves the LED on slightly longer for a packet error

}





bool Send_Test_Packet()

{

  uint8_t bufffersize;

  uint8_t buff[] = "Hello World!";

  buff[12] = '#';                                //overwrite null character at end of buffer so we can see it in RX

  

  if (sizeof(buff) > TXBUFFER_SIZE)

  {

    bufffersize = TXBUFFER_SIZE;

  }

  else

  {

    bufffersize = sizeof(buff);

  }



  Serial.println();

  Serial.print(F("buffsize "));

  Serial.println(sizeof(buff));



  TXPacketL = bufffersize;



  SX1280LT.printASCIIPacket(buff, bufffersize);



  digitalWrite(LED1, HIGH);



  if (SX1280LT.sendPacketFLRC(buff, bufffersize, 1000, PowerTX, DIO1))

  {

    digitalWrite(LED1, LOW);

    return true;

  }

  else

  {

    return false;

  }

}





void led_Flash(uint16_t flashes, uint16_t delaymS)

{

  unsigned int index;

  for (index = 1; index <= flashes; index++)

  {

    digitalWrite(LED1, HIGH);

    delay(delaymS);

    digitalWrite(LED1, LOW);

    delay(delaymS);

  }

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

  SX1280LT.setDioIrqParams(IRQ_RADIO_ALL, (IRQ_TX_DONE + IRQ_RX_TX_TIMEOUT), 0, 0);              //set for IRQ on TX done and timeout on DIO1

  SX1280LT.setSyncWord1(Sample_Syncword);

}





void setup()

{

  // pinMode(LED1, OUTPUT);

  // led_Flash(2, 125);



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

    led_Flash(2, 150);

    delay(1000);

  }

  else

  {

    Serial.println(F("No device responding"));

    while (1)

    {

      led_Flash(50, 50);                                            //long fast speed flash indicates device error

    }

  }



  setup_FLRC();



  Serial.println(F("FLRC Transmitter ready"));

  Serial.println();

}
