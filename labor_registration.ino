#include <SPI.h>
#include <MFRC522.h>
#include <SoftwareSerial.h>

#define buttonPin 6
#define ledPin 7
#define ledPinR 4
#define ledPinB 5
#define SS_PIN 10
#define RST_PIN 9
#define txPin 2
#define rxPin 3
MFRC522 rfid(SS_PIN, RST_PIN);
SoftwareSerial mySerial(txPin, rxPin);
MFRC522::MIFARE_Key key;
byte nuidPICC[4];
int state = LOW;
int reading;
int previous = LOW;
long time = 0;
long debounce = 100;

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
  Serial.println(F("This code scan the MIFARE Classsic NUID."));
  Serial.print(F("Using the following key:"));
  printHex(key.keyByte, MFRC522::MF_KEY_SIZE);
  mySerial.println(F("registration start!"));
  Serial.println();
  pinMode(ledPin, OUTPUT);
  pinMode(ledPinR, OUTPUT);
  pinMode(ledPinB, OUTPUT);
  pinMode(buttonPin, INPUT);
}

void loop() {
  if (mySerial.available())
    Serial.write(mySerial.read());
  if (Serial.available())
    mySerial.write(Serial.read());
  else {
    reading = digitalRead(buttonPin);
  if(reading == HIGH && previous == LOW && millis()-time > debounce) {
    Serial.println(F("Harmful insect is founded in this line."));
    mySerial.println(F("Harmful insect is founded in this line."));
    state = HIGH;
    time = millis();
  }
  else {
    state = LOW;
  }
  digitalWrite(ledPin, state);
  digitalWrite(ledPinB, LOW);
  digitalWrite(ledPinR, LOW);
  previous = reading;
  if ( ! rfid.PICC_IsNewCardPresent())
    return;
  if ( ! rfid.PICC_ReadCardSerial())
    return;

  Serial.print(F("PICC type: "));
  //mySerial.print(F("PICC type: "));
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  Serial.println(rfid.PICC_GetTypeName(piccType));
  //mySerial.println(rfid.PICC_GetTypeName(piccType));

  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&  
    piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
    piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    Serial.println(F("Your tag is not of type MIFARE Classic."));
    mySerial.println(F("Your tag is not of type MIFARE Classic."));
    return;
  }

  if (rfid.uid.uidByte[0] != nuidPICC[0] || 
    rfid.uid.uidByte[1] != nuidPICC[1] || 
    rfid.uid.uidByte[2] != nuidPICC[2] || 
    rfid.uid.uidByte[3] != nuidPICC[3]) {
    Serial.println(F("Worker enters line."));
    mySerial.println(F("Worker enters line."));

    for (byte i = 0; i < 4; i++) {
      nuidPICC[i] = rfid.uid.uidByte[i];
    }
   
    Serial.print(F("The line's ID is:"));
    mySerial.print(F("The line's ID is:"));
    printHex(rfid.uid.uidByte, rfid.uid.size);
    myprintHex(rfid.uid.uidByte, rfid.uid.size);
    Serial.println();
    mySerial.println();
    digitalWrite(ledPinB, HIGH);
    digitalWrite(ledPinR, LOW);
  }
  else {
    Serial.println(F("Worker exits line."));
    mySerial.println(F("Worker exits line."));
    //Serial.print(F("The line's ID was:"));
    //mySerial.print(F("The line's ID was:"));
    //printHex(rfid.uid.uidByte, rfid.uid.size);
    //myprintHex(rfid.uid.uidByte, rfid.uid.size);
    //Serial.println();
    //mySerial.println();
    digitalWrite(ledPinB, LOW);
    digitalWrite(ledPinR, HIGH);
    for (byte i = 0; i < 4; i++) {
          nuidPICC[i] = 0;
    }
  }

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
  }
}

void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}
void myprintHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    mySerial.print(buffer[i] < 0x10 ? " 0" : " ");
    mySerial.print(buffer[i], HEX);
  }
}

void printDec(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], DEC);
  }
}
void myprintDec(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    mySerial.print(buffer[i] < 0x10 ? " 0" : " ");
    mySerial.print(buffer[i], DEC);
  }
}