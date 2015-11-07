#include <OctoWS2811.h>

const int ledsPerStrip = 50;

DMAMEM int displayMemory[ledsPerStrip*6];
int drawingMemory[ledsPerStrip*6];

const int config = WS2811_GRB | WS2811_800kHz;

OctoWS2811 leds(ledsPerStrip, displayMemory, drawingMemory, config);

#define RED     0xFF0000
#define GREEN   0x00FF00
#define BLUE    0x0000FF
#define YELLOW  0xFFFF00
#define PINK    0xFF1088
#define ORANGE  0xE05800
#define WHITE   0xFFFFFF

//#define THRESHOLD 50
#define SCAN_DELAY 200
#define SET_SENSITIVITY 70
#define SONAR_COUNT 6
#define SONAR_ENABLE_PIN 0

int lastReadings[SONAR_COUNT];

int sustainState = 0;
int sensitivity = 0;

struct Tread {
    byte pin;
    byte enablePin;
    int numNotes;
    int notes[4];
    int state;
    int ledStart;
    int color;
    //int currentReading;
};

//#define BASSKEY   43
//#define MELODYKEY 55

//Mary Had Lamb with Chords defined
struct Tread treads[][6] = {{ {A9, 23, 1, {52,56,59}, 0, 0, RED}, 
                              {A8, 22, 1, {53,57,60}, 0, (ledsPerStrip), GREEN}, 
                              {A5, 19, 1, {55,59,62}, 0, (ledsPerStrip * 2), BLUE},
                              {A4, 18, 1, {57,61,64}, 0, (ledsPerStrip * 3), YELLOW},
                              {A3, 18, 1, {59,63,66}, 0, (ledsPerStrip * 4), PINK},
                              {A10, 18, 1, {61,65,68}, 0, (ledsPerStrip * 5), ORANGE}
                            }};

//Beethonven's Fifth Chords defined
struct Tread fifthTreads[][6] =  {{ {A0, 2, 3, {47,51,54}, 0, 0, RED}, 
                                    {A1, 3, 3, {48,52,55}, 0, 50, GREEN}, 
                                    {A2, 4, 3, {50,54,57}, 0, 100, BLUE},
                                    {A3, 5, 3, {52,56,59}, 0, 150, YELLOW},
                                    {A3, 18, 1, {57,61,64}, 0, 200, PINK},
                                    {A10, 18, 1, {57,61,64}, 0, 250, ORANGE}
                                 }};

//Smoke on the Water
struct Tread smokeTreads[][6] = {{  {A9, 23, 3, {48,52,55}, 0, 0, RED}, 
                                    {A8, 22, 3, {51,55,58}, 0, 50, GREEN}, 
                                    {A5, 19, 3, {53,57,60}, 0, 100, BLUE},
                                    {A4, 18, 3, {54,58,61}, 0, 150, YELLOW},
                                    {A3, 18, 3, {57,61,64}, 0, 200, PINK},
                                    {A10, 18, 3, {61,65,68}, 0, 250, ORANGE}
                                  }};


//Mary Had Lamb with Chords defined
struct Tread maryTreads[][6] = {{ {A0, 2, 1, {52,56,59}, 0, 0, RED}, 
                                  {A1, 3, 1, {53,57,60}, 0, 50, GREEN}, 
                                  {A2, 4, 1, {55,59,62}, 0, 100, BLUE},
                                  {A3, 5, 1, {57,61,64}, 0, 150, YELLOW},
                                  {A3, 18, 1, {57,61,64}, 0, 200, PINK},
                                  {A10, 18, 1, {57,61,64}, 0, 250, ORANGE}
                                }};

//Louie Louie
struct Tread louieTreads[][6] =  {{ {A0, 2, 1, {55,59,62}, 0, 0, RED}, 
                                    {A1, 3, 1, {62,66,69}, 0, 50, GREEN}, 
                                    {A2, 4, 1, {64,68,71}, 0, 100, BLUE},
                                    {A3, 5, 1, {65,69,72}, 0, 150, YELLOW},
                                    {A3, 18, 1, {57,61,64}, 0, 200, PINK},
                                    {A10, 18, 1, {57,61,64}, 0, 250, ORANGE}
                                 }};
                              
// Carol of the Bells Melody NO CHORDS
struct Tread bellMelTreads[][6] =    {{ {A0, 2, 1, {67,0,0}, 0, 0, RED}, 
                                        {A1, 3, 1, {68,0,0}, 0, 50, GREEN}, 
                                        {A2, 4, 1, {69,0,0}, 0, 100, BLUE},
                                        {A3, 5, 1, {70,0,0}, 0, 150, YELLOW},
                                        {A3, 18, 1, {57,61,64}, 0, 200, PINK},
                                        {A10, 18, 1, {57,61,64}, 0, 250, ORANGE}
                                     }};

// Carol of the Bells Bass
struct Tread bellBassTreads[][6] =   {{ {A0, 2, 1, {50,54,57}, 0, 0, RED}, 
                                        {A1, 3, 1, {52,56,59}, 0, 50, GREEN}, 
                                        {A2, 4, 1, {53,57,60}, 0, 100, BLUE},
                                        {A3, 5, 1, {55,59,62}, 0, 150, YELLOW},
                                        {A3, 18, 1, {57,61,64}, 0, 200, PINK},
                                        {A10, 18, 1, {57,61,64}, 0, 250, ORANGE}
                                     }};


//int measure = 0;
int treadNumber = 0;
int lastSong = 0;


//Digital pin 7 for reading in the pulse width from the MaxSonar device.
//This variable is a constant because the pin will not change throughout execution of this code.
//const int pwPin = 7; 
//variables needed to store values
//long pulse, inches, cm;


//  pinMode(pwPin, INPUT);
    //Used to read in the pulse that is being sent by the MaxSonar device.
  //Pulse Width representation with a scale factor of 147 uS per Inch.

/*  pulse = pulseIn(pwPin, HIGH);
  //147uS per inch
  inches = pulse/147;
  //change inches to centimetres
  cm = inches * 2.54;
  Serial.print(inches);
  Serial.print("in, ");
  Serial.print(cm);
  Serial.print("cm");
  Serial.println();*/


void readTread(struct Tread * tread) {
  //digitalWrite(tread->enablePin, HIGH);
  //delay(25); // min 15
  int reading = analogRead(tread->pin);
  //long reading = pulseIn(tread->pin, HIGH);
  //if (treadNumber == 3) {
    //Serial.print(treadNumber);
    //Serial.print(" ");
    Serial.print(reading);
    Serial.print(" ");
    if (treadNumber == 5) {
      Serial.println(" ");
    }
  //}
  //digitalWrite(tread->enablePin, LOW);                    

    /*if(abs(reading - tread->lastReading) >= THRESHOLD) {
        pot->currentReading = (reading/2);
        //Serial.println(pot->currentReading);
        pot->lastReading = reading;
    }*/
  //if(abs(reading - lastReadings[treadNumber]) < 5) {  
  if(reading > sensitivity) {
    if(tread->state == 1) {
      if(digitalRead(9)) {
        usbMIDI.sendNoteOff(tread->notes[0], 0, (1));
        lightOff(tread->ledStart);
      } else {
        for(int i = 0; i < tread->numNotes; i++) {
          usbMIDI.sendNoteOff(tread->notes[i], 0, (1));
          lightOff(tread->ledStart);
        }
      }
      //digitalWrite(13,LOW);
      tread->state = 0;      
    }
  } else {
    if(tread->state == 0) {
      if(digitalRead(9)) {
        usbMIDI.sendNoteOn(tread->notes[0], 127, (1));
        lightTread(tread->ledStart, tread->color);
        Serial.println(treadNumber);
      } else {
        for(int i = 0; i < tread->numNotes; i++) {
          usbMIDI.sendNoteOn(tread->notes[i], 127, (1));
          lightTread(tread->ledStart, tread->color);
          Serial.println(treadNumber);
        }
      }
      //digitalWrite(13,HIGH);
      tread->state = 1;
    }
  }
  //}
  lastReadings[treadNumber] = reading;
}

void colorWipe(int color, int wait)
{
  for (int i=0; i < ledsPerStrip; i++) {
    leds.setPixel(i, color);
    //leds.show();
    //delayMicroseconds(wait);
  }
  leds.show();
}

void lightTread(int ledStart, int color)
{
  int ledEnd = ledStart + ledsPerStrip;
  for (int i=ledStart; i < ledEnd; i++) {
    leds.setPixel(i, color);
    //leds.show();
    //delayMicroseconds(wait);
  }
  leds.show();
}

void lightOff(int ledStart)
{
  int ledEnd = ledStart + ledsPerStrip;
  for (int i=ledStart; i < ledEnd; i++) {
    leds.setPixel(i, 0x000000);
  }
  leds.show();
}

void setup() {
    pinMode(A3,INPUT);
    pinMode(A4,INPUT);
    pinMode(A5,INPUT);
    pinMode(A8,INPUT);
    pinMode(A9,INPUT);
    pinMode(A14,INPUT);
    pinMode(0,OUTPUT);
    leds.begin();
    leds.show();
    digitalWrite(13,LOW);
    Serial.begin(9600);
    delay(1000);
    Serial.print(leds.numPixels());
    Serial.print(" pixels ===================================================");
    //digitalWrite(SONAR_ENABLE_PIN, HIGH);
    //delayMicroseconds(30);
    //digitalWrite(SONAR_ENABLE_PIN, LOW);
    //delay(50);
    memcpy(treads, smokeTreads, sizeof(treads));
}

elapsedMillis sinceLastReading = 0;

void loop() {
    if(sinceLastReading > SCAN_DELAY) {
      digitalWrite(SONAR_ENABLE_PIN, HIGH);
      delayMicroseconds(25);
      digitalWrite(SONAR_ENABLE_PIN, LOW);
      sinceLastReading = 0;
    }
    /*int song = analogRead(A4);
    if (abs(song - lastSong) > 50) {
      if (song <= 200) {
        // song one: Mary Had a Little Lamb
        Serial.println("Mary Had a Little Lamb");
      } else if ((song > 200) && (song <= 400)) {
        // song two: Louie Louie
        Serial.println("Louie Louie");
        memcpy(treads, louieTreads, sizeof(treads));
      } else if ((song > 400) && (song <= 600)) {
        // song three: Smoke on the Water
        Serial.println("Smoke on the Water");
        memcpy(treads, smokeTreads, sizeof(treads));
      } else if ((song > 600) && (song <= 800)) {
        // song four: Beethoven's Fifth Symphony
        Serial.println("Beethoven's Fifth Symphony");
        memcpy(treads, fifthTreads, sizeof(treads));
      } else if ((song > 800) && (song <= 900)) {
        // song five:
        Serial.println("Bells Melody");
        memcpy(treads, bellMelTreads, sizeof(treads));
      } else if ((song > 900) && (song <= 1024)) {
        // song six:
        Serial.println("Bells Bass");
        memcpy(treads, bellBassTreads, sizeof(treads));
      }
      lastSong = song;
    }
  */
  //sensitivity = analogRead(A5);
  sensitivity = SET_SENSITIVITY;
  //Serial.print("Sensitivity: ");
  //Serial.println(sensitivity);
  if (digitalRead(10)) {
    if (sustainState == 1) {
      usbMIDI.sendControlChange(64, 0, 1); // Sustain off if still pulled up
      sustainState = 0;
      digitalWrite(13,LOW);
    }
  } else {
    if (sustainState == 0) {
      usbMIDI.sendControlChange(64, 127, 1); // Sustain on if switch closed
      sustainState = 1;
      digitalWrite(13,HIGH);
    }
  }

  for (treadNumber = 0; treadNumber < SONAR_COUNT; treadNumber++) {
    readTread(&treads[0][treadNumber]);
  }
  //readTread(&treads[progression[measure]][treadNumber]);

  /*if (treadNumber == (numMeasures - 1)) {
    treadNumber = 0;
  } else {
    treadNumber++;
  }*/
}
