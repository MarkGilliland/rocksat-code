/*

******************************************************************************************************


ROCKSAT-X COLORADO SCHOOL OF MINES 2020 - GROUND DATA RECEPTION
 

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



The following code is intended to receive a transmitted image from the payload on ground, via use of a

modified Yagi-Uda antenna design, with FLRC protocol.



******************************************************************************************************



Receives Fast Long Range Communications (FLRC) mode packets according to the LoRa parameters in the flrcRX.h file. 

Results, RSSI, SNR, errors etc are displayed in the Arduino IDE serial monitor. The LED will flash when a packet is received,
 
you can add and enable a buzzer too. Used together with matching TX program. According to the SX1280 data sheet for LoRa at SF5 

and bandwidth 1625khz, the effective data rate is 203kbps with a sensitivity of -99dBm. For FLRC mode at a bit rate of 1.3Mbps
 
and bandwidth of 1.2Mhz the effective data rate is 975kbps at the same sensitivity (-99dBm) as 203kbps LoRa.



******************************************************************************************************

*/




#define programversion "V1.0"

#define Serial_Monitor_Baud 115300



#include <SPI.h>



/*


************************************* FLRC Definitions ***************************************

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

                     





/************************************* FLRC Modem Parameters ***************************************/



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



/************************************* Include SX1280 ***************************************/

#include <SX1280LT.h>





SX1280Class SX1280LT;



uint32_t RXpacketCount = 0;

uint32_t errors;

bool ENABLEBUZZER;



uint8_t RXBUFFER[RXBUFFER_SIZE];



uint8_t RXPacketL;                               //stores length of packet received

int8_t  PacketRSSI;                              //stores RSSI of received packet





void loop()

{

  SX1280LT.setRx(PERIOBASE_01_MS, 0);            //set no SX1280 RX timeout



  while (!digitalRead(DIO1));                    //wait for RxDone or timeout interrupt activating DIO1



  digitalWrite(LED1, HIGH);



  if (ENABLEBUZZER)

  {

    digitalWrite(BUZZER, HIGH);

  }



  SX1280LT.readPacketReceptionFLRC();

  RXPacketL = SX1280LT.readRXPacketL();

  PacketRSSI = SX1280LT.readPacketRSSI();



  if (SX1280LT.readIrqStatus() == (IRQ_RX_DONE + IRQ_SYNCWORD_VALID))

  {

    packet_is_OK();

  }

  else

  {

    packet_is_Error();

  }



  digitalWrite(LED1, LOW);



  if (ENABLEBUZZER)

  {

    digitalWrite(BUZZER, LOW);

  }

  Serial.println();

}





void packet_is_OK()

{

  uint16_t IRQStatus;

  uint8_t len;



  RXpacketCount++;

  len = SX1280LT.readPacketFLRC(RXBUFFER, RXBUFFER_SIZE);         //read the actual packet, maximum size specified in RXBUFFER_SIZE



  if (len == 0)

  {

    Serial.print(F("Buffer Error"));

  }

  else

  {

    SX1280LT.printASCIIPacket(RXBUFFER, len);                 //len same as RXPacketL

  }



  Serial.print(",RSSI,");



  Serial.print(PacketRSSI);

  Serial.print(F("dBm,Length,"));

  Serial.print(RXPacketL);

  Serial.print(F(",Packets,"));

  Serial.print(RXpacketCount);

  Serial.print(F(",Errors,"));

  Serial.print(errors);



  IRQStatus = SX1280LT.readIrqStatus();

  Serial.print(F(",IRQreg,"));

  Serial.print(IRQStatus, HEX);

}





void packet_is_Error()

{

  uint16_t IRQStatus;



  if (ENABLEBUZZER)

  {

    delay(50);

    digitalWrite(BUZZER, HIGH);

    delay(50);

    digitalWrite(BUZZER, LOW);

  }



  IRQStatus = SX1280LT.readIrqStatus();                    //get the IRQ status

  errors++;

  Serial.print(F("PacketError,RSSI"));

  Serial.print(PacketRSSI);

  Serial.print(F("dB,Length,"));

  Serial.print(RXPacketL);

  Serial.print(F(",IRQreg,"));

  Serial.print(IRQStatus, HEX);

  SX1280LT.printIrqStatus();

  digitalWrite(LED1, LOW);



  if (ENABLEBUZZER)

  {

    digitalWrite(BUZZER, LOW);

    delay(100);

    digitalWrite(BUZZER, HIGH);

  }

}





void PrintPacket()

{

  Serial.write(RXBUFFER, RXPacketL);                  //print whole packet, ASCII assumed

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

  SX1280LT.setDioIrqParams(IRQ_RADIO_ALL, IRQ_RX_DONE, 0, 0);

  SX1280LT.setSyncWord1(Sample_Syncword);

}





void setup(void)

{

  pinMode(LED1, OUTPUT);

  led_Flash(2, 125);



  Serial.begin(Serial_Monitor_Baud);

  Serial.println();



  Serial.println();

  Serial.print(F(__TIME__));

  Serial.print(F(" "));

  Serial.println(F(__DATE__));

  Serial.println(F(programversion));

  Serial.println();

  Serial.println();

  Serial.println(F("13_SX1280LT_FLRC_Simple_RX Starting"));

  Serial.println();



  if (BUZZER >= 0)

  {

    ENABLEBUZZER = true;

    pinMode(BUZZER, OUTPUT);

    digitalWrite(BUZZER, LOW);

    Serial.println(F("BUZZER Enabled"));

  }

  else

  {

    ENABLEBUZZER = false;

    Serial.println(F("BUZZER Not Enabled"));

  }



  SPI.begin();

  SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));



  if (SX1280LT.begin(NSS, NRESET, RFBUSY, DIO1, DIO2, DIO3))

  {

    Serial.println(F("Device found"));

    led_Flash(2, 125);

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



  Serial.print(F("Receiver ready - RXBUFFER_SIZE "));

  Serial.println(RXBUFFER_SIZE);

  Serial.println();

}
