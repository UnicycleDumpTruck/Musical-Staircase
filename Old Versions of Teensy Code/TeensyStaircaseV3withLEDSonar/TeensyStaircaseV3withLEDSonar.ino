//#include <MIDI.h>

//MIDI_CREATE_DEFAULT_INSTANCE();

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

#define THRESHOLD 50
#define SET_SENSITIVITY 50
#define SONAR_COUNT 6
#define SONAR_ENABLE_PIN A14

int sustainState = 0;
int sensitivity = 0;

struct Tread {
    byte pin;
    byte enablePin;
    int numNotes;
    int notes[4];
    int state;
    //int currentReading;
};

#define BASSKEY   43
#define MELODYKEY 55

//Mary Had Lamb with Chords defined
struct Tread treads[][6] = {{ {A9, 23, 1, {52,56,59}, 0}, 
                              {A8, 22, 1, {53,57,60}, 0}, 
                              {A5, 19, 1, {55,59,62}, 0},
                              {A4, 18, 1, {57,61,64}, 0},
                              {A3, 18, 1, {57,61,64}, 0},
                              {A10, 18, 1, {57,61,64}, 0}
                            }};

//Beethonven's Fifth Chords defined
struct Tread fifthTreads[][6] =  {{ {A0, 2, 3, {47,51,54}, 0}, 
                                    {A1, 3, 3, {48,52,55}, 0}, 
                                    {A2, 4, 3, {50,54,57}, 0},
                                    {A3, 5, 3, {52,56,59}, 0},
                                    {A3, 18, 1, {57,61,64}, 0},
                                    {A10, 18, 1, {57,61,64}, 0}
                                 }};

//Smoke on the Water
struct Tread smokeTreads[][6] = {{ {A0, 2, 3, {48,52,55}, 0}, 
                                    {A1, 3, 3, {51,55,58}, 0}, 
                                    {A2, 4, 3, {53,57,60}, 0},
                                    {A3, 5, 3, {54,58,61}, 0},
                                    {A3, 18, 1, {57,61,64}, 0},
                                    {A10, 18, 1, {57,61,64}, 0}
                                  }};


//Mary Had Lamb with Chords defined
struct Tread maryTreads[][6] = {{ {A0, 2, 1, {52,56,59}, 0}, 
                                  {A1, 3, 1, {53,57,60}, 0}, 
                                  {A2, 4, 1, {55,59,62}, 0},
                                  {A3, 5, 1, {57,61,64}, 0},
                                  {A3, 18, 1, {57,61,64}, 0},
                                  {A10, 18, 1, {57,61,64}, 0}
                                }};

//Louie Louie
struct Tread louieTreads[][6] =  {{ {A0, 2, 1, {55,59,62}, 0}, 
                                    {A1, 3, 1, {62,66,69}, 0}, 
                                    {A2, 4, 1, {64,68,71}, 0},
                                    {A3, 5, 1, {65,69,72}, 0},
                                    {A3, 18, 1, {57,61,64}, 0},
                                    {A10, 18, 1, {57,61,64}, 0}
                                 }};
                              
// Carol of the Bells Melody NO CHORDS
struct Tread bellMelTreads[][6] =    {{ {A0, 2, 1, {67,0,0}, 0}, 
                                        {A1, 3, 1, {68,0,0}, 0}, 
                                        {A2, 4, 1, {69,0,0}, 0},
                                        {A3, 5, 1, {70,0,0}, 0},
                                        {A3, 18, 1, {57,61,64}, 0},
                                        {A10, 18, 1, {57,61,64}, 0}
                                     }};

// Carol of the Bells Bass
struct Tread bellBassTreads[][6] =   {{ {A0, 2, 1, {50,54,57}, 0}, 
                                        {A1, 3, 1, {52,56,59}, 0}, 
                                        {A2, 4, 1, {53,57,60}, 0},
                                        {A3, 5, 1, {55,59,62}, 0},
                                        {A3, 18, 1, {57,61,64}, 0},
                                        {A10, 18, 1, {57,61,64}, 0}
                                     }};


//int progression[4] = {0,0,0,0};
//int numMeasures = 6;
//int progression[12] = {0,0,0,0,1,1,0,0,2,1,0,2}; // 12 bar blues
//int progression[4] = {0,0,2,1}; // 4 bar blues


//int measure = 0;
int treadNumber = 0;
int lastSong = 0;

void readTread(struct Tread * tread) {
  //digitalWrite(tread->enablePin, HIGH);
  //delay(25); // min 15
  int reading = analogRead(tread->pin);
  //if (treadNumber == 3) {
    //Serial.print(treadNumber);
    //Serial.print(" ");
    Serial.print(reading);
    Serial.print(" ");
    if (treadNumber == 5) {
      Serial.println(" ");
    }
  //}
  digitalWrite(tread->enablePin, LOW);                    

    /*if(abs(reading - tread->lastReading) >= THRESHOLD) {
        pot->currentReading = (reading/2);
        //Serial.println(pot->currentReading);
        pot->lastReading = reading;
    }*/
    
  if(reading > sensitivity) {
    if(tread->state == 1) {
      if(digitalRead(9)) {
        usbMIDI.sendNoteOff(tread->notes[0], 0, (1));
        //colorWipe(0x000000,0);
      } else {
        for(int i = 0; i < tread->numNotes; i++) {
          usbMIDI.sendNoteOff(tread->notes[i], 0, (1));
          //colorWipe(0x000000,0);
        }
      }
      //digitalWrite(13,LOW);
      tread->state = 0;
      
/*      if (treadNumber == 0) {
        if (measure == 3) {
          measure = 0;
        } else {
          measure++;
        }
      }
*/
      
    }
  } else {
    if(tread->state == 0) {
      if(digitalRead(9)) {
        usbMIDI.sendNoteOn(tread->notes[0], 127, (1));
        //colorWipe(RED,0);
        Serial.println(treadNumber);
      } else {
        for(int i = 0; i < tread->numNotes; i++) {
          usbMIDI.sendNoteOn(tread->notes[i], 127, (1));
          //colorWipe(RED,0);
          Serial.println(treadNumber);
        }
      }
      //digitalWrite(13,HIGH);
      tread->state = 1;
    }
  }
}

void colorWipe(int color, int wait)
{
  for (int i=0; i < leds.numPixels(); i++) {
    leds.setPixel(i, color);
    leds.show();
    //delayMicroseconds(wait);
    //delay(2000);
  }
}


void setup() {
//    usbMIDI.begin(MIDI_CHANNEL_OMNI);
    pinMode(A0,INPUT);
    pinMode(A1,INPUT);
    pinMode(A2,INPUT);
    pinMode(A3,INPUT);
    pinMode(A4,INPUT);
    pinMode(A5,INPUT);
    pinMode(2,OUTPUT);
    pinMode(3,OUTPUT);
    pinMode(4,OUTPUT);
    pinMode(5,OUTPUT);
    pinMode(6,OUTPUT);
    pinMode(7,OUTPUT);
    pinMode(9,INPUT_PULLUP);
    pinMode(10,INPUT_PULLUP);
    pinMode(13,OUTPUT);
    leds.begin();
    leds.show();
    digitalWrite(13,LOW);
    Serial.begin(9600);
    delay(200);
    Serial.print(leds.numPixels());
    Serial.print(" pixels ===================================================");
}

void loop() {
    digitalWrite(SONAR_ENABLE_PIN, HIGH);
    //delayMicroseconds(20);
    delay(50);
    digitalWrite(SONAR_ENABLE_PIN, LOW);
    //delay(50);
    int song = analogRead(A4);
    /*if (abs(song - lastSong) > 50) {
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
