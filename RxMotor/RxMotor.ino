

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

int numRot1 = 3;
int numRot2 = 6;
int numRot = 0;
float hall_thresh = 100.0;
float hall_count = 0.0;

const byte thisSlaveAddress[5] = {'R','x','A','A','A'};

RF24 radio(CE_PIN, CSN_PIN);

int dataReceived; // this must match dataToSend in the TX
int ackData[2] = {0, 0}; // the two values to be sent to the master
bool newData = false;
bool on_state = false;

int state = 0;
int trigger = 0;
int toggle=0;

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

//==========

void loop() {
  int potValue = analogRead(A0); // Read potentiometer value
  int pwmOutput = map(potValue, 0, 1023, 0 , 255); // Map the potentiometer value from 0 to 255
  bool hall = digitalRead(hallIn);

  if (hall==0){
      if (on_state==false){
        on_state = true;
        hall_count+=1.0;
      }
    } else{
      on_state = false;
    }


      
    if (hall_count>numRot){
      hall_count=0;
      state+=1;
      if (state>3) state=0;

      if (toggle==0){
        numRot=numRot2;
        toggle=1;}
      else if (toggle==1){
        numRot=numRot1;
        toggle=0;}
    }

    if (state==0){
        analogWrite(enA, 0); // Send PWM signal to L298N Enable pin
    }
    else if (state==1){
      digitalWrite(in1, LOW);
      digitalWrite(in2, HIGH);
      analogWrite(enA, pwmOutput); // Send PWM signal to L298N Enable pin
    }

        else if (state==2){
      analogWrite(enA, 0); // Send PWM signal to L298N Enable pin
      delay(3000);
      state=3;
    }

    else if (state==3){
      digitalWrite(in1, HIGH);
      digitalWrite(in2, LOW);
      analogWrite(enA, pwmOutput); // Send PWM signal to L298N Enable pin
    }
  
    getData();
    updateReplyData();
    showData();
    delay(1);
    Serial.println(hall_count);
    Serial.print(", ");
    Serial.print(state);
}

//============

void getData() {
    if ( radio.available() ) {
        radio.read( &dataReceived, sizeof(dataReceived) );
        trigger=dataReceived;
        if (trigger == 1){
          if (state ==0){
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
