

// SimpleRxAckPayload- the slave or the receiver

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <DFMiniMp3.h>
//#include <EEPROM.h>
//#include <JC_Button.h>
//#include <MFRC522.h>
#include <SoftwareSerial.h>
//#include "DFRobotDFPlayerMini.h"
//#include <avr/sleep.h>

#define CE_PIN   9
#define CSN_PIN 10
#define in1 4

const byte thisSlaveAddress[5] = {'R','x','A','A','B'};

RF24 radio(CE_PIN, CSN_PIN);
//SoftwareSerial mySoftwareSerial(2, 3); // RX, TX

// implement a notification class,
// its member methods will get called 
//
class Mp3Notify
{
public:
  static void PrintlnSourceAction(DfMp3_PlaySources source, const char* action)
  {
    if (source & DfMp3_PlaySources_Sd) 
    {
        Serial.print("SD Card, ");
    }
    if (source & DfMp3_PlaySources_Usb) 
    {
        Serial.print("USB Disk, ");
    }
    if (source & DfMp3_PlaySources_Flash) 
    {
        Serial.print("Flash, ");
    }
    Serial.println(action);
  }
  static void OnError(uint16_t errorCode)
  {
    // see DfMp3_Error for code meaning
    Serial.println();
    Serial.print("Com Error ");
    Serial.println(errorCode);
  }
  static void OnPlayFinished(DfMp3_PlaySources source, uint16_t track)
  {
    Serial.print("Play finished for #");
    Serial.println(track);  
  }
  static void OnPlaySourceOnline(DfMp3_PlaySources source)
  {
    PrintlnSourceAction(source, "online");
  }
  static void OnPlaySourceInserted(DfMp3_PlaySources source)
  {
    PrintlnSourceAction(source, "inserted");
  }
  static void OnPlaySourceRemoved(DfMp3_PlaySources source)
  {
    PrintlnSourceAction(source, "removed");
  }
};

// instance a DFMiniMp3 object, 
// defined with the above notification class and the hardware serial class
//
//DFMiniMp3<HardwareSerial, Mp3Notify> mp3(Serial1);

// Some arduino boards only have one hardware serial port, so a software serial port is needed instead.
// comment out the above definition and uncomment these lines
SoftwareSerial secondarySerial(2, 3); // RX, TX
DFMiniMp3<SoftwareSerial, Mp3Notify> mp3(secondarySerial);



int dataReceived; // this must match dataToSend in the TX
int ackData[2] = {0, 0}; // the two values to be sent to the master
bool newData = false;
bool on_state = false;

int state = 0;
int trigger = 0;
int currSong=1;
uint16_t count =0;


//==============

void setup() {

    Serial.begin(9600);

    pinMode(in1, INPUT);

    Serial.println("initializing DFplayer...");
    
    mp3.begin();
    mp3.reset(); 

        // show some properties and set the volume
    uint16_t volume = mp3.getVolume();
    Serial.print("volume ");
    Serial.println(volume);
    mp3.setVolume(5);
    
    count = mp3.getTotalTrackCount(DfMp3_PlaySource_Sd);
    Serial.print("files ");
    Serial.println(count);
    uint16_t mode = mp3.getPlaybackMode();
    Serial.print("playback mode ");
    Serial.println(mode);
    
    Serial.println("starting...");

    Serial.println("SimpleRxAckPayload Starting");
    radio.begin();
    radio.setDataRate( RF24_250KBPS );
    radio.openReadingPipe(1, thisSlaveAddress);

    radio.enableAckPayload();
    
    radio.startListening();

    radio.writeAckPayload(1, &ackData, sizeof(ackData)); // pre-load data
}

//==========

void loop() {
    getData();
    showData();
    mp3.loop();
    delay(1);
}

//============

void getData() {
    bool mp3Busy = digitalRead(in1);

    if ( radio.available() ) {
        radio.read( &dataReceived, sizeof(dataReceived) );
        trigger=dataReceived;
        if (trigger == 1){
          if (state ==0){
            state=1;
            trigger = 0;
            //Serial.println("playing new song");
            mp3.setVolume(10);
            mp3.playGlobalTrack(currSong);
            currSong+=1;
            if (currSong>count)
              currSong=1;
       
//            mp3.playRandomTrackFromAll(); // random of all folders on sd

//            if (mp3Busy==0){
//              Serial.println("now Stop");
//            mp3.stop();
//            }
            state=0;
          }
        }
        updateReplyData();
        newData = true;
    }
}

//================

void showData() {
    if (newData == true) {
        Serial.print("Data received ");
        Serial.println(dataReceived);
        Serial.print(" ackPayload sent ");
        Serial.print(ackData[0]);
        Serial.print(", ");
        Serial.println(ackData[1]);
        newData = false;
    }
}

//================

void updateReplyData() {
    ackData[0] = state;
    ackData[1] = state;

    radio.writeAckPayload(1, &ackData, sizeof(ackData)); // load the payload for the next time
}
