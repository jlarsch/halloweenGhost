
// eyes from https://www.instructables.com/Animated-Halloween-Evil-Eyes/
// nrf24 code inspired by https://forum.arduino.cc/index.php?topic=520515.0

// SimpleRxAckPayload- the slave or the receiver

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <DFMiniMp3.h>
#include <SoftwareSerial.h>

#include <avr/pgmspace.h>
#include <LedControl.h>

#define CE_PIN   9
#define CSN_PIN 10
#define in1 5

int soundVol = 30; //0-30
int frownTime = 10000; //time evil eyes frown (during flight down + some scare time)
int stareTime = 10000; //time evil eyes stare (during return flight)

int playList[6]={24,24,16,17,23,1};


const int numDevices = 2;      // number of MAX7219s used
const int dataPin = 4;
const int clkPin = 3;
const int csPin = 2;
LedControl lc=LedControl(dataPin,clkPin,csPin,numDevices);
#define DELAY_US             1000   // Delay time in MicroSeconds
unsigned long CurrentTime  = 0;     // this variable will be overwritten by micros() each iteration of loop
unsigned long PastTime     = 0;     // A record of the time before this read
unsigned long ElapsedTime  = 0;     // Elapsed time in uS

#define MAX_INT 32767
#define EMARKER ,{MAX_INT,MAX_INT,MAX_INT}

#define SCREEN1 0

// Drop this code into Arduino development environment    
    
#define LeftEye1 0    
#define RightEye1 1   
#define LeftEye2 2    
#define RightEye2 3   
#define LeftEye3 4    
#define RightEye3 5   
#define LeftEye4 6  
#define RightEye4 7   
#define LeftEye5 8    
#define RightEye5 9   
#define LeftEye6 10   
#define RightEye6 11    
#define LeftEye7 12   
#define RightEye7 13    
#define LeftEye8 14   
#define RightEye8 15    
#define LeftEye9 16   
#define RightEye9 17    
#define LeftEye10 18    
#define RightEye10 19   
#define LeftEye11 20    
#define RightEye11 21   
#define LeftEye12 22    
#define RightEye12 23   
#define LeftEye13 24    
#define RightEye13 25   
#define LeftEye14 26    
#define RightEye14 27   
#define LeftEye15 28    
#define RightEye15 29   
#define LeftEye16 30    
#define RightEye16 31   
#define LeftEye17 32    
#define RightEye17 33   
#define LeftEye18 34    
#define RightEye18 35   
#define LeftEye19 36    
#define RightEye19 37   
#define LeftEye20 38    
#define RightEye20 39   
#define LeftEye21 40    
#define RightEye21 41   
#define LeftEye22 42    
#define RightEye22 43   
#define LeftEye23 44    
#define RightEye23 45   
#define LeftEye24 46    
#define RightEye24 47   
#define LeftEye25 48    
#define RightEye25 49   
#define LeftEye26 50    
#define RightEye26 51   
#define LeftEye27 52    
#define RightEye27 53   
#define LeftEye28 54    
#define RightEye28 55   
#define LeftEye29 56    
#define RightEye29 57   
    
typedef struct {    
  const unsigned char array1[8]; 
} binaryArrayType;    
    
binaryArrayType binaryArray[58] =   
{   
  { // LeftEye1, 0  
    B01111110,
    B10000001,
    B10000001,
    B10011001,
    B10011001,
    B10000001,
    B10000001,
    B01111110
  },  
  { // RightEye1, 1 
    B01111110,
    B10000001,
    B10000001,
    B10011001,
    B10011001,
    B10000001,
    B10000001,
    B01111110
  },  
  { // LeftEye2, 2  
    B00000000,
    B00111100,
    B01000010,
    B01011010,
    B01011010,
    B01000010,
    B00111100,
    B00000000
  },  
  { // RightEye2, 3 
    B00000000,
    B00111100,
    B01000010,
    B01011010,
    B01011010,
    B01000010,
    B00111100,
    B00000000
  },  
  { // LeftEye3, 4  
    B00000000,
    B00111100,
    B00100100,
    B00110100,
    B00110100,
    B00100100,
    B00111100,
    B00000000
  },  
  { // RightEye3, 5 
    B00000000,
    B00111100,
    B00100100,
    B00110100,
    B00110100,
    B00100100,
    B00111100,
    B00000000
  },  
  { // LeftEye4, 6  
    B00011000,
    B00011000,
    B00011000,
    B00011000,
    B00011000,
    B00011000,
    B00011000,
    B00000000
  },  
  { // RightEye4, 7 
    B00000000,
    B00011000,
    B00011000,
    B00011000,
    B00011000,
    B00011000,
    B00011000,
    B00011000
  },  
  { // LeftEye5, 8  
    B01111110,
    B10000001,
    B10000001,
    B10011001,
    B10011001,
    B10000001,
    B10000010,
    B01111100
  },  
  { // RightEye5, 9 
    B01111100,
    B10000010,
    B10000001,
    B10011001,
    B10011001,
    B10000001,
    B10000001,
    B01111110
  },  
  { // LeftEye6, 10 
    B01111110,
    B10000001,
    B10000001,
    B10011001,
    B10011001,
    B10000010,
    B10000100,
    B01111000
  },  
  { // RightEye6, 11  
    B01111000,
    B10000100,
    B10000010,
    B10011001,
    B10011001,
    B10000001,
    B10000001,
    B01111110
  },  
  { // LeftEye7, 12 
    B01111110,
    B11000001,
    B10000001,
    B10011001,
    B10011010,
    B10000100,
    B10001000,
    B01110000
  },  
  { // RightEye7, 13  
    B01110000,
    B10001000,
    B10000100,
    B10011010,
    B10011001,
    B10000001,
    B11000001,
    B01111110
  },  
  { // LeftEye8, 14 
    B00111110,
    B01000001,
    B10000001,
    B10011001,
    B10011010,
    B10000100,
    B01001000,
    B00110000
  },  
  { // RightEye8, 15  
    B00110000,
    B01001000,
    B10000100,
    B10011010,
    B10011001,
    B10000001,
    B01000001,
    B00111110
  },  
  { // LeftEye9, 16 
    B01111110,
    B10000001,
    B10000001,
    B10011001,
    B10011001,
    B10000001,
    B10000001,
    B01111110
  },  
  { // RightEye9, 17  
    B01111110,
    B10000001,
    B10000001,
    B10011001,
    B10011001,
    B10000001,
    B10000001,
    B01111110
  },  
  { // LeftEye10, 18  
    B01111110,
    B10000001,
    B10000001,
    B10000001,
    B10011001,
    B10011001,
    B10000001,
    B01111110
  },  
  { // RightEye10, 19 
    B01111110,
    B10000001,
    B10000001,
    B10000001,
    B10011001,
    B10011001,
    B10000001,
    B01111110
  },  
  { // LeftEye11, 20  
    B01111110,
    B10000001,
    B10000001,
    B10000001,
    B10011001,
    B10011001,
    B10000001,
    B01111110
  },  
  { // RightEye11, 21 
    B01111110,
    B10000001,
    B10000001,
    B10000001,
    B10011001,
    B10011001,
    B10000001,
    B01111110
  },  
  { // LeftEye12, 22  
    B01111110,
    B10000001,
    B10000001,
    B10000001,
    B10000001,
    B10011001,
    B10011001,
    B01111110
  },  
  { // RightEye12, 23 
    B01111110,
    B10000001,
    B10000001,
    B10000001,
    B10000001,
    B10011001,
    B10011001,
    B01111110
  },  
  { // LeftEye13, 24  
    B01111110,
    B10000001,
    B10000001,
    B10000001,
    B10000001,
    B10000001,
    B10011001,
    B01111110
  },  
  { // RightEye13, 25 
    B01111110,
    B10000001,
    B10000001,
    B10000001,
    B10000001,
    B10000001,
    B10011001,
    B01111110
  },  
  { // LeftEye14, 26  
    B00000000,
    B00111100,
    B01000010,
    B01000010,
    B01000010,
    B01011010,
    B00111100,
    B00000000
  },  
  { // RightEye14, 27 
    B00000000,
    B00111100,
    B01000010,
    B01000010,
    B01000010,
    B01011010,
    B00111100,
    B00000000
  },  
  { // LeftEye15, 28  
    B00000000,
    B00111100,
    B00100100,
    B00100100,
    B00100100,
    B00111100,
    B00111100,
    B00000000
  },  
  { // RightEye15, 29 
    B00000000,
    B00111100,
    B00100100,
    B00100100,
    B00100100,
    B00111100,
    B00111100,
    B00000000
  },  
  { // LeftEye16, 30  
    B00011000,
    B00011000,
    B00011000,
    B00011000,
    B00011000,
    B00011000,
    B00011000,
    B00000000
  },  
  { // RightEye16, 31 
    B00000000,
    B00011000,
    B00011000,
    B00011000,
    B00011000,
    B00011000,
    B00011000,
    B00011000
  },  
  { // LeftEye17, 32  
    B00010000,
    B00010000,
    B00010000,
    B00010000,
    B00010000,
    B00010000,
    B00010000,
    B00000000
  },  
  { // RightEye17, 33 
    B00000000,
    B00010000,
    B00010000,
    B00010000,
    B00010000,
    B00010000,
    B00010000,
    B00010000
  },  
  { // LeftEye18, 34  
    B01111110,
    B10000001,
    B10000001,
    B10000001,
    B10000001,
    B10000001,
    B10001101,
    B01111110
  },  
  { // RightEye18, 35 
    B01111110,
    B10000001,
    B10000001,
    B10000001,
    B10000001,
    B10000001,
    B10001101,
    B01111110
  },  
  { // LeftEye19, 36  
    B01111110,
    B10000001,
    B10000001,
    B10000001,
    B10000001,
    B10000001,
    B10000111,
    B01111110
  },  
  { // RightEye19, 37 
    B01111110,
    B10000001,
    B10000001,
    B10000001,
    B10000001,
    B10000001,
    B10000111,
    B01111110
  },  
  { // LeftEye20, 38  
    B01111110,
    B10000001,
    B10000001,
    B10000001,
    B10000001,
    B10000011,
    B10000011,
    B01111110
  },  
  { // RightEye20, 39 
    B01111110,
    B10000001,
    B10000001,
    B10000001,
    B10000001,
    B10000011,
    B10000011,
    B01111110
  },  
  { // LeftEye21, 40  
    B00111110,
    B01000001,
    B10000001,
    B10011001,
    B10010010,
    B10000100,
    B01001000,
    B00110000
  },  
  { // RightEye21, 41 
    B00110000,
    B01001000,
    B10000100,
    B10010010,
    B10011001,
    B10000001,
    B01000001,
    B00111110
  },  
  { // LeftEye22, 42  
    B01111110,
    B10000001,
    B10000001,
    B10000001,
    B10000011,
    B10000011,
    B10000001,
    B01111110
  },  
  { // RightEye22, 43 
    B01111110,
    B10000001,
    B10000001,
    B10000001,
    B10000011,
    B10000011,
    B10000001,
    B01111110
  },  
  { // LeftEye23, 44  
    B01111110,
    B10000001,
    B10000001,
    B10000001,
    B10000111,
    B10000111,
    B10000001,
    B01111110
  },  
  { // RightEye23, 45 
    B01111110,
    B10000001,
    B10000001,
    B10000001,
    B10000111,
    B10000111,
    B10000001,
    B01111110
  },  
  { // LeftEye24, 46  
    B01111110,
    B10000001,
    B10000001,
    B10001101,
    B10001101,
    B10000001,
    B10000001,
    B01111110
  },  
  { // RightEye24, 47 
    B01111110,
    B10000001,
    B10000001,
    B10001101,
    B10001101,
    B10000001,
    B10000001,
    B01111110
  },  
  { // LeftEye25, 48  
    B01111110,
    B10000001,
    B10000001,
    B10000001,
    B10000001,
    B10000001,
    B10001101,
    B01111110
  },  
  { // RightEye25, 49 
    B01111110,
    B10000001,
    B10000001,
    B10000001,
    B10000001,
    B10001101,
    B10001101,
    B01111110
  },  
  { // LeftEye26, 50  
    B01111110,
    B10000001,
    B10011001,
    B10011001,
    B10000001,
    B10000001,
    B10000001,
    B01111110
  },  
  { // RightEye26, 51 
    B01111110,
    B10000001,
    B10011001,
    B10011001,
    B10000001,
    B10000001,
    B10000001,
    B01111110
  },  
  { // LeftEye27, 52  
    B01111110,
    B10011001,
    B10011001,
    B10000001,
    B10000001,
    B10000001,
    B10000001,
    B01111110
  },  
  { // RightEye27, 53 
    B01111110,
    B10011001,
    B10011001,
    B10000001,
    B10000001,
    B10000001,
    B10000001,
    B01111110
  },  
  { // LeftEye28, 54  
    B01111110,
    B10011001,
    B10000001,
    B10000001,
    B10000001,
    B10000001,
    B10000001,
    B01111110
  },  
  { // RightEye28, 55 
    B01111110,
    B10011001,
    B10000001,
    B10000001,
    B10000001,
    B10000001,
    B10000001,
    B01111110
  },  
  { // LeftEye29, 56 
    B00000000,
    B00000000,
    B00000000,
    B00011000,
    B00011000,
    B00000000,
    B00000000,
    B00000000
  },  
  { // RightEye29, 57 
    B00000000,
    B00000000,
    B00000000,
    B00011000,
    B00011000,
    B00000000,
    B00000000,
    B00000000
  } 
};    
  
  
  

    
typedef struct myframeType {      
  int animationIndex;  // Index pointer into binaryArray signifying animation frame   
  int frameDelay;      // Approx delay in MilliSeconds to hold display this animated frame
        int frameLuminance;  // 0 ... 15, The intensity of the led matrix for a given frame 
} frameType;
  

frameType movie1[] = 
{
// Blink
{LeftEye1,6000,1}, {LeftEye2,5,1}, {LeftEye3,10,1}, {LeftEye4,10,1},   {LeftEye17,100,1}, {LeftEye4,10,1}, {LeftEye3,10,1}, {LeftEye2,5,1},
// Full wide again
{LeftEye1,3000,2},
// Frown
{LeftEye5,5,3},    {LeftEye6,5,4}, {LeftEye7,5,5},  {LeftEye8,4000,11}, {LeftEye7,5,5},    {LeftEye6,5,4},   {LeftEye5,5,0},   {LeftEye29,5,0}
EMARKER
};


frameType movie2[] = 
{
// Look Right
{LeftEye9,6000,1},  {LeftEye10,30,1},   {LeftEye11,30,1},   {LeftEye12,30,1},    {LeftEye13,1000,1},  
// Blink Right
{LeftEye14,100,1},  {LeftEye15,5,1},   {LeftEye16,10,1},   {LeftEye17,10,1},   {LeftEye16,10,1},   {LeftEye15,10,1},   {LeftEye14,5,1},
// Full wide eyes 
{LeftEye13,400,1},  {LeftEye12,200,1},    {LeftEye11,100,1},    {LeftEye10,100,1},    {LeftEye9,100,1},    {LeftEye26,100,1},    {LeftEye27,200,1},    {LeftEye28,400,1},    {LeftEye27,100,1},    {LeftEye26,200,1},      {LeftEye9,800,1},  
// Frown
{LeftEye5,5,3},    {LeftEye6,5,4}, {LeftEye7,5,5},  {LeftEye8,4000,11}, {LeftEye7,5,5},    {LeftEye6,5,4},   {LeftEye5,5,3}
EMARKER
};


frameType movie3[] = 
{
// Frown
{LeftEye1,4000,1}, {LeftEye5,5,3},    {LeftEye6,5,4}, {LeftEye7,5,5},  {LeftEye21,2000,11}, {LeftEye7,5,5},    {LeftEye6,5,4},   {LeftEye5,5,3}
EMARKER
};

frameType movie4[] = 
{
// Blink
{LeftEye1,6000,1}, {LeftEye2,5,1}, {LeftEye3,10,1}, {LeftEye4,10,1},   {LeftEye17,100,1}, {LeftEye4,10,1}, {LeftEye3,10,1}, {LeftEye2,5,1},
// Full wide again
{LeftEye1,3000,2},
// Frown
{LeftEye5,5,3},    {LeftEye6,5,4}, {LeftEye7,5,5},  {LeftEye8,frownTime,15}, {LeftEye7,5,5},    {LeftEye6,5,4},   {LeftEye5,5,0},
// Blink
{LeftEye1,stareTime,1}, {LeftEye2,5,1}, {LeftEye3,10,1}, {LeftEye4,10,1},   {LeftEye17,100,10}
EMARKER
};

typedef struct {
    int frameCountCurrent;   // This field must be zero 
    int frameDelayCurrent;   // This field must be zero  
    int movieLength;         // This field must be zero. Auto calculated at start up in 'loop()', requires 'EMARKER' to be last element in 'frameType' array
    int whichScreen;         // Which screen you wish to show this movie on.
    myframeType *movie;      // Pointer to movie of interest (ie. a collection of frames)
} frameAnimationDescriptor;


frameAnimationDescriptor nowShowing[] =
{
{0,0,0,SCREEN1, movie4}
};


void handleFrameAnimation(int whichScreen, int movielength, struct myframeType *movie, int *frameCountCurrent, int *frameDelayCurrent)
{
   for (int i = 0; i < 8; i++)
   {
     lc.setRow(whichScreen,     i, binaryArray[movie[*frameCountCurrent].animationIndex    ].array1[i]); 
     lc.setRow(whichScreen + 1, i, binaryArray[movie[*frameCountCurrent].animationIndex + 1].array1[i]); 
   }          
   lc.setIntensity(whichScreen,     movie[*frameCountCurrent].frameLuminance);       
   lc.setIntensity(whichScreen + 1, movie[*frameCountCurrent].frameLuminance);       

  *frameDelayCurrent = movie[*frameCountCurrent].frameDelay;
  
   if ((*frameCountCurrent) < (movielength-1))
     (*frameCountCurrent)++;
   //else
   //  *frameCountCurrent = 0;
}



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
SoftwareSerial secondarySerial(6, 7); // RX, TX
DFMiniMp3<SoftwareSerial, Mp3Notify> mp3(secondarySerial);



int dataReceived; // this must match dataToSend in the TX
int ackData[2] = {0, 0}; // the two values to be sent to the master
bool newData = false;
bool on_state = false;

int state = 0;
int trigger = 0;
int currSong=0;
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

   for (int x=0; x<numDevices; x++){
    lc.shutdown(x,false);       //The MAX72XX is in power-saving mode on startup
    lc.setIntensity(x,3);       // Set the brightness to default value
    lc.clearDisplay(x);         // and clear the display
}

    // Initialise
  int index1;
  for (int index = 0; index < (sizeof(nowShowing)/sizeof(frameAnimationDescriptor)); index++)
  {
      index1 = 0;
      while (nowShowing[index].movie[index1].animationIndex != MAX_INT) // calculate the number of frames in the movie and store is movieLength 
      {
          nowShowing[index].movieLength++;
          index1++;
      }
      nowShowing[index].frameCountCurrent = 0;
      handleFrameAnimation(nowShowing[index].whichScreen, nowShowing[index].movieLength, nowShowing[index].movie, &nowShowing[index].frameCountCurrent, &nowShowing[index].frameDelayCurrent);
  }  
  
}

//==========

void loop() {

  

  CurrentTime = micros();  

    ElapsedTime = CurrentTime - PastTime; // handles roll-over, if currentTime is small, and pastTime large, the result rolls over to a small positive value, the time that has passed
    if(ElapsedTime >= DELAY_US) PastTime = CurrentTime;
    while (ElapsedTime < DELAY_US)
    {
      CurrentTime = micros(); 
      ElapsedTime = CurrentTime - PastTime; // handles roll-over, if currentTime is small, and pastTime large, the result rolls over to a small positive value, the time that has passed
      if(ElapsedTime >= DELAY_US) PastTime = CurrentTime;
    }
    CurrentTime = micros(); 

    for (int index = 0; index < (sizeof(nowShowing)/sizeof(frameAnimationDescriptor)); index++)
    {
      nowShowing[index].frameDelayCurrent--;
      if (nowShowing[index].frameDelayCurrent == 0)
        handleFrameAnimation(nowShowing[index].whichScreen, nowShowing[index].movieLength, nowShowing[index].movie, &nowShowing[index].frameCountCurrent, &nowShowing[index].frameDelayCurrent);
    }
  
  
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
            //trigger = 0;
            delay(2000);
            for (int index = 0; index < (sizeof(nowShowing)/sizeof(frameAnimationDescriptor)); index++)
              nowShowing[index].frameCountCurrent = 0;

            //Serial.println("playing new song");
            mp3.setVolume(soundVol);
            delay(2000);
            mp3.playGlobalTrack(playList[0]);
            delay(200);
            currSong=1;
 
 
            state=0;
          }
        }
        else{
        Serial.println(trigger);
        Serial.println(currSong);
        if ((currSong > 0)&&(currSong<=6)){
          if (mp3Busy){
            Serial.println(currSong);
            mp3.playGlobalTrack(playList[currSong]);
                        currSong+=1;
            if (currSong>6)
              currSong=0;
              trigger=0;
          }
        }
        updateReplyData();
        newData = true;
    }}
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
    ackData[0] = trigger;
    ackData[1] = trigger;

    radio.writeAckPayload(1, &ackData, sizeof(ackData)); // load the payload for the next time
}
