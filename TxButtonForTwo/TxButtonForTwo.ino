
// arrow led: https://github.com/simonwongwong/Bluetooth-enabled_bicycle_turn_signals/blob/master/code_for_lights.ino

// MultiTxAckPayload - the master or the transmitter
//   works with two Arduinos as slaves
//     each slave should the SimpleRxAckPayload program
//       one with the adress {'R','x','A','A','A'}
//         and the other with {'R','x','A','A','B'}

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#include <DFMiniMp3.h>
#include <SoftwareSerial.h>


#define CE_PIN   9
#define CSN_PIN 10
#define button 4
#define flash A0 // control pin for mosfet -> flash LED strip
//#define in1 4
#define enA 5 // control pin for transistor -> glowing button LED

int pressTrack = 6; // sound to play when button is pressed.

unsigned long lastTime  = 0;     // this variable will be overwritten by micros() each iteration of loop
unsigned long startTime     = 0;     // A record of the time before this read
float ElapsedTime  = 0;     // Elapsed time in uS
float restTime = 40000; // time to wait before releasing the ghose again. (make sure there is enough time for full fly back.)
int soundVol = 30; //0-30
float BlinkFreq = 0.0;
int flashCount=0;
int nextWait=0;

unsigned long startArrow = 0;
float ElapsedArrow = 0;

#define flashLen 31
int flashTimings[flashLen] = {100,200,1000,100,100,100,100,100,5000,100,100,700,400,100,100,900,100,300,400,100,6000,100,100,2000,400,100,100,800,100,300,400};

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


SoftwareSerial secondarySerial(2, 3); // RX, TX
DFMiniMp3<SoftwareSerial, Mp3Notify> mp3(secondarySerial);



const byte numSlaves = 2;
const byte slaveAddress[numSlaves][5] = {
        // each slave needs a different address
                            {'R','x','A','A','A'},
                            {'R','x','A','A','B'}
                        };

RF24 radio(CE_PIN, CSN_PIN); // Create a Radio

//~ char dataToSend[10] = "Message 0";
char dataToSend[10] = "ToSlvN  0";
char txNum = '0';
int ackData[2] = {-1, -1}; // to hold the two values coming from the slave
bool newData = false;

unsigned long currentMillis;
unsigned long prevMillis;
unsigned long txIntervalMillis = 1000; // send once per second

int state = 0; // 0: rest, 1: go, 2:arrive, 3: return
int ghostState =0;
uint16_t count =0;

#include "LedControl.h"

LedControl lc = LedControl(6, 8, 7, 1); //DIN, CLK, CS
int dlay = 100; // delay of 50ms per frame

//left turn animation
const byte LEFT[][8] = {
  {
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00011000
  }, {
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00011000,
    B00000000
  }, {
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00011000,
    B00000000,
    B00111100
  }, {
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00011000,
    B00000000,
    B00111100,
    B00000000
  }, {
    B00000000,
    B00000000,
    B00000000,
    B00011000,
    B00000000,
    B00111100,
    B00000000,
    B01111110
  }, {
    B00000000,
    B00000000,
    B00011000,
    B00000000,
    B00111100,
    B00000000,
    B01111110,
    B00000000
  }, {
    B00000000,
    B00011000,
    B00000000,
    B00111100,
    B00000000,
    B01111110,
    B00000000,
    B11111111
  }, {
    B00011000,
    B00000000,
    B00111100,
    B00000000,
    B01111110,
    B00000000,
    B11111111,
    B00000000
  }, {
    B00000000,
    B00111100,
    B00000000,
    B01111110,
    B00000000,
    B11111111,
    B00000000,
    B00000000
  }, {
    B00111100,
    B00000000,
    B01111110,
    B00000000,
    B11111111,
    B00000000,
    B00000000,
    B00000000
  }, {
    B00000000,
    B01111110,
    B00000000,
    B11111111,
    B00000000,
    B00000000,
    B00000000,
    B00000000
  }, {
    B01111110,
    B00000000,
    B11111111,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000
  }, {
    B00000000,
    B11111111,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000
  }, {
    B11111111,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000
  }
};
int i = 0;
int len = sizeof(LEFT) / 8;

//===============

void setup() {

    Serial.begin(9600);
    BlinkFreq = 1000.0;
    pinMode(enA, OUTPUT);
    pinMode(flash,OUTPUT);
    pinMode(button,INPUT);
    
    Serial.println(F("Source File = /mnt/SGT/SGT-Prog/Arduino/ForumDemos/nRF24Tutorial/MultiTxAckPayload.ino "));
    Serial.println("SimpleTxAckPayload Starting");
    radio.begin();
    radio.setDataRate( RF24_250KBPS );
    radio.enableAckPayload();
    radio.setRetries(3,5); // delay, count

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
    startTime = millis();

      lc.shutdown(0, false); // Wake up displays
  lc.setIntensity(0, 2); // Set intensity levels
  lc.clearDisplay(0);  // Clear Displays
}

//=============

void loop() {
    
    // button glow
    lastTime = millis();
    float tmp = sin(lastTime/BlinkFreq);
    int buttonOut = (tmp+1)*(255.0/2.0);

    //-------


      
    
    bool btn = digitalRead(button);
    ElapsedTime = lastTime-startTime;
    if ((btn == HIGH) && (state == 0) && (ElapsedTime> restTime)){ // If Switch is Activated
          startTime = millis();
          flashCount=0;
          nextWait = flashTimings[flashCount];
          state=1;
          BlinkFreq = 100.0;
            mp3.setVolume(soundVol);
            mp3.playGlobalTrack(pressTrack);


    }
    if (ElapsedTime< restTime){
                 BlinkFreq = 100.0;
                 analogWrite(enA, 0);
                 lc.clearDisplay(0);
    }
    else{
      BlinkFreq = 1000.0;
      analogWrite(enA, buttonOut);
      ElapsedArrow = lastTime-startArrow;

      if (ElapsedArrow > dlay){
        startArrow=millis();
        for (int j = 0; j < 8; j++) {
          lc.setRow(0, j, LEFT[i][j]);
          lc.setRow(1, j, LEFT[i][j]);
        }
  
        if (i < len - 1) {
        i++;
      } else {
        i = 0;
      }
      
      }
    }

    
        if (ElapsedTime> nextWait){
          if(flashCount<=flashLen){
                flashCount++;
                nextWait = nextWait+flashTimings[flashCount];
                analogWrite(flash, 250*(flashCount % 2));
          }
                
    }

    currentMillis = millis(); 
   
    if (currentMillis - prevMillis >= txIntervalMillis) { 
        send();
        if (ghostState==0)
        state=0;
    }
    mp3.loop();
    delay(1);
}

//================



void send() {
    for (byte n = 0; n < numSlaves; n++){

        radio.openWritingPipe(slaveAddress[n]);
        bool rslt;
        rslt = radio.write( &state, sizeof(state) );

        Serial.print("  ========  For Slave ");
        Serial.print(n);
        Serial.println("  ========");
        Serial.print("  Data Sent ");
        Serial.print(state);
        if (rslt) {
            if ( radio.isAckPayloadAvailable() ) {
                radio.read(&ackData, sizeof(ackData));
                newData = true;
                ghostState=ackData[0];
                Serial.print("  GhostState received: ");
                Serial.print(ghostState);                
            }
        }
        else {
            Serial.println("  Tx failed");
        }
        showData();
        Serial.print("\n");
    }
    prevMillis = millis();
 }


//=================

void showData() {
    if (newData == true) {
        Serial.print("  Acknowledge data ");
        Serial.print(ackData[0]);
        Serial.print(", ");
        Serial.println(ackData[1]);
        Serial.println();
        newData = false;
    }
}
