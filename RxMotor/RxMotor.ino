

// SimpleRxAckPayload- the slave or the receiver

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define CE_PIN   9
#define CSN_PIN 10
// #define DEBUG
#define enA 5
#define in1 4
#define in2 8
#define hallIn 3

unsigned long CurrentTime  = 0;     // this variable will be overwritten by micros() each iteration of loop
unsigned long startTime     = 0;     // A record of the time before this read
unsigned long ElapsedTime  = 0;     // Elapsed time in uS

int numRot1 = 12; // number of motor rotations flying down.
unsigned long restDelay = 6000; // time before flyback (ms)
unsigned long startDelay=15000;
int extraFlybackRevolution = 10; //compensation for slip.

float hall_thresh = 100.0;
unsigned long hall_count = 1;

const byte thisSlaveAddress[5] = {'R','x','A','A','A'};

RF24 radio(CE_PIN, CSN_PIN);

int dataReceived; // this must match dataToSend in the TX
int ackData[2] = {0, 0}; // the two values to be sent to the master
bool newData = false;
bool on_state = false;

int state = 0;
int trigger = 0;
int numRot = 0; // stores actual motor rotations


//==============

void setup() {

    Serial.begin(9600);
    pinMode(enA, OUTPUT);
    pinMode(hallIn, INPUT);
        // Set initial rotation direction
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);

    Serial.println("SimpleRxAckPayload Starting");
    radio.begin();
    radio.setDataRate( RF24_250KBPS );
    radio.openReadingPipe(1, thisSlaveAddress);

    radio.enableAckPayload();
    
    radio.startListening();

    radio.writeAckPayload(1, &ackData, sizeof(ackData)); // pre-load data

    numRot=numRot1;
}

void updateHall(){
  bool hall = digitalRead(hallIn);

  if (hall==0){
      if (on_state==false){
        on_state = true;
        hall_count++;
      }
    } else{
      on_state = false;
    }
}

//==========

void loop() {
  int potValue = analogRead(A0); // Read potentiometer value
  int pwmOutput = map(potValue, 0, 1023, 0 , 255); // Map the potentiometer value from 0 to 255
  

    updateHall();
      
    if (hall_count>numRot && state!= 2){
      //hall_count=0;
      state+=1;
      if (state>3) {
        state=0;
        delay(5000);
      }
    }

    if (state==0){
        analogWrite(enA, 0); // Send stop signal to L298N Enable pin
        numRot=numRot1;
        hall_count=0;
    }
    else if (state==1){
      digitalWrite(in1, LOW);
      digitalWrite(in2, HIGH);
      analogWrite(enA, pwmOutput); // Send PWM signal to L298N Enable pin
    }

        else if (state==2){
      analogWrite(enA, 0); // Send stop signal to L298N Enable pin

      startTime = millis();

    ElapsedTime = 0;
    while (ElapsedTime < restDelay)
    {
      CurrentTime = millis();
      updateHall(); // keep counting revolutions in state 2. Ghost continues to fly even when motor is off.
          
    
    Serial.print(state);
    Serial.print(", waiting for delay,  ");
    Serial.print(ElapsedTime);
    Serial.print(",  ");
        Serial.print(restDelay);
    Serial.print(",  ");
    Serial.println(hall_count);
    
      ElapsedTime = CurrentTime - startTime;
    }
    
      numRot=hall_count + extraFlybackRevolution; // one more revolution on flyback to compensate for slip.
      hall_count=0;
      state=3;
    }

    else if (state==3){
      digitalWrite(in1, HIGH);
      digitalWrite(in2, LOW);
      analogWrite(enA, pwmOutput); // Send PWM signal to L298N Enable pin, reverse direction
    }
  
    getData();
    updateReplyData();
    showData();
    delay(1);
    //Serial.print(hall_count);
    //Serial.print(", ");
    //Serial.println(state);
}

//============

void getData() {
    if ( radio.available() ) {
        radio.read( &dataReceived, sizeof(dataReceived) );
        trigger=dataReceived;
        if (trigger == 1){
          if (state ==0){
            delay(startDelay);
            state=1;
            trigger = 0;
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
    ackData[1] = hall_count;

    radio.writeAckPayload(1, &ackData, sizeof(ackData)); // load the payload for the next time
}
