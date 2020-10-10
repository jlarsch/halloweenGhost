


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
#define button 7
#define in1 4

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
int currSong=1;
uint16_t count =0;

//===============

void setup() {

    Serial.begin(9600);
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
}

//=============

void loop() {

    bool btn = digitalRead(button);
    if ((btn == HIGH) && (state == 0)){ // If Switch is Activated
          state=1;
            mp3.setVolume(10);
            mp3.playGlobalTrack(currSong);
            currSong+=1;
            if (currSong>count)
              currSong=1;
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
