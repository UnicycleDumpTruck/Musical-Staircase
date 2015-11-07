#include <OctoWS2811.h>
const int ledsPerStrip = 8;
const int numberOfTreads = 6;


int nextNoteInSong = 0;
int numNotesInSongs[] = {47, 120};
int notesInSongs[][120] = {   { 67, 72, 72, 72, 74, 76, 76, 76, 74, 72, 74, 76, 72, 76, 76, 77, 79, 79, 77, 76, 77, 79, 76, 72, 72, 74, 76, 76, 74, 72, 74, 76, 72, 67, 67, 72, 72, 72, 74, 76, 76, 76, 74, 72, 74, 76, 72,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },  // Itsy Bitsy Spider
                {83, 88, 88, 88, 83, 83, 83, 86, 86, 88, 
                91, 91, 91, 91, 83,  83, 86, 86, 88, 
                76, 76, 88, 88, 88, 90,
                86, 86, 86, 86, 88, 86, 86, 86,
                86, 86, 85, 83, 81, 79, 81, 79, 83, 79, 76, 
                83, 83, 86, 88, 88, 88, 88, 88, 88, 88, 88, 88, 86,
                88, 88, 88, 86, 88, 83, 83, 86, 88, 88, 88, 88, 88, 90,
                86, 86, 83, 83, 88, 86, 88, 88, 86, 88, 83, 83, 86, 
                88, 88, 95, 93, 93, 91, 88, 91, 88, 88, 86, 88, 88, 88, 88, 86, 88,
                83, 83, 86, 88, 88, 88, 88, 88, 90, 86, 86, 83, 83,
                88, 88, 86, 88, 86, 88} // Happy 
              };
int treadNoteAssignments[numberOfTreads];
int whichSong = 0;

DMAMEM int displayMemory[ledsPerStrip*numberOfTreads];
int drawingMemory[ledsPerStrip*numberOfTreads];

const int config = WS2811_GRB | WS2811_800kHz;

OctoWS2811 leds(ledsPerStrip, displayMemory, drawingMemory, config);

#define RED     0xFF0000
#define GREEN   0x00FF00
#define BLUE    0x0000FF
#define YELLOW  0xFFFF00
#define PINK    0xFF1088
#define ORANGE  0xE05800
#define WHITELED   0xFFFFFF
#define LEDOFF  0x000000

#define PLAYINGSONG 1
#define PLAYCHORDS 0
//#define THRESHOLD 50
#define SCAN_DELAY 200
#define SET_SENSITIVITY 700
#define SONAR_SENSITIVITY 70
#define LASER_SENSITIVITY 700
#define SONAR_ENABLE_PIN 0

int needsLighting[numberOfTreads] = {0, 0, 0, 0, 0, 0}; // 0 = off; 1 = lighting; 2 = unlighting;
elapsedMillis sinceLastLedScan = 0;
int treadNumber = 0;
int lastSong = 0;

int lastReadings[numberOfTreads];
int sustainState = 0;
int sensitivity = 0;
int sensitivities[] = {LASER_SENSITIVITY, LASER_SENSITIVITY, LASER_SENSITIVITY, LASER_SENSITIVITY, LASER_SENSITIVITY, LASER_SENSITIVITY};
int inputPins[] = {A9, A8, A5, A4, A11, A3};
int states[] = {0, 0, 0, 0, 0, 0};
int ledStarts[] = {0, ledsPerStrip, (ledsPerStrip * 2), (ledsPerStrip * 3), (ledsPerStrip * 4), (ledsPerStrip * 5)};
int colors[] = {RED, GREEN, BLUE, YELLOW, PINK, ORANGE, WHITELED};
int nextLefts[] = {(ledsPerStrip/2 - 1), (ledsPerStrip + (ledsPerStrip/2 - 1)), ((ledsPerStrip * 2) + (ledsPerStrip/2 - 1)), ((ledsPerStrip * 3) + (ledsPerStrip/2 - 1)), ((ledsPerStrip * 4) + (ledsPerStrip/2 - 1)), ((ledsPerStrip * 5) + (ledsPerStrip/2 - 1))};
int nextRights[] = {(ledsPerStrip/2), (ledsPerStrip + (ledsPerStrip/2)), ((ledsPerStrip * 2) + (ledsPerStrip/2)), ((ledsPerStrip * 3) + (ledsPerStrip/2)), ((ledsPerStrip * 4) + (ledsPerStrip/2)), ((ledsPerStrip * 5) + (ledsPerStrip/2))};

struct Tread {
    int numNotes;
    int notes[4];
};

//Mary Had Lamb with Chords defined
struct Tread treads[][6] = {{ {1, {48,56,59}}, 
                              {1, {50,57,60}}, 
                              {1, {52,59,62}},
                              {1, {53,61,64}},
                              {1, {55,63,66}},
                              {1, {57,65,68}}
                            }};

//Beethonven's Fifth Chords defined
struct Tread fifthTreads[][6] =  {{ {3, {47,51,54}}, 
                                    {3, {48,52,55}}, 
                                    {3, {50,54,57}},
                                    {3, {52,56,59}},
                                    {3, {57,61,64}},
                                    {3, {57,61,64}}
                                 }};

//Smoke on the Water
struct Tread smokeTreads[][6] = {{  {3, {48,52,55}}, 
                                    {3, {51,55,58}}, 
                                    {3, {53,57,60}},
                                    {3, {54,58,61}},
                                    {3, {57,61,64}},
                                    {3, {61,65,68}}
                                  }};


//Mary Had Lamb with Chords defined
struct Tread maryTreads[][6] = {{ {1, {52,56,59}}, 
                                  {1, {53,57,60}}, 
                                  {1, {55,59,62}},
                                  {1, {57,61,64}},
                                  {1, {57,61,64}},
                                  {1, {57,61,64}}
                                }};

//Louie Louie
struct Tread louieTreads[][6] =  {{ {1, {55,59,62}}, 
                                    {1, {62,66,69}}, 
                                    {1, {64,68,71}},
                                    {1, {65,69,72}},
                                    {1, {57,61,64}},
                                    {1, {57,61,64}}
                                 }};
                              
// Carol of the Bells Melody NO CHORDS
struct Tread bellMelTreads[][6] =    {{ {1, {67,0,0}}, 
                                        {1, {68,0,0}}, 
                                        {1, {69,0,0}},
                                        {1, {70,0,0}},
                                        {1, {57,61,64}},
                                        {1, {57,61,64}}
                                     }};

// Carol of the Bells Bass
struct Tread bellBassTreads[][6] =   {{ {1, {50,54,57}}, 
                                        {1, {52,56,59}}, 
                                        {1, {53,57,60}},
                                        {1, {55,59,62}},
                                        {1, {57,61,64}},
                                        {1, {57,61,64}}
                                     }};

void readTread(void) {
  struct Tread * tread = &treads[0][treadNumber];
  int reading = analogRead(inputPins[treadNumber]);
    //Serial.print(treadNumber);
    //Serial.print(" ");
    /*Serial.print(reading);
    Serial.print(" ");
    if (treadNumber == 5) {
      Serial.println(" ");
    }*/

  //if(abs(reading - lastReadings[treadNumber]) < 5) {  
  if(reading > sensitivities[treadNumber]) {
    if(states[treadNumber] == 1) {
        if(!(PLAYCHORDS)) {
 
        if (PLAYINGSONG) { 
          //treadNoteAssignments[treadNumber] = notesInSongs[nextNoteInSong][whichSong];
          //nextNoteInSong++;
          //if (nextNoteInSong == numNotesInSongs[whichSong]) {
          //  nextNoteInSong = 0;
          //}
          usbMIDI.sendNoteOff(treadNoteAssignments[treadNumber], 0, 1);
          needsLighting[treadNumber] = 2;
        } else {      
          usbMIDI.sendNoteOff(tread->notes[0], 0, (1));
          needsLighting[treadNumber] = 2;
        }
      } else {
        for(int i = 0; i < tread->numNotes; i++) {
          usbMIDI.sendNoteOff(tread->notes[i], 0, (1));
          needsLighting[treadNumber] = 2;
        }
      }
      states[treadNumber] = 0;      
    }
  } else {
    if(states[treadNumber] == 0) {
      if(!(PLAYCHORDS)) {
 
        if (PLAYINGSONG) { 
          treadNoteAssignments[treadNumber] = notesInSongs[whichSong][nextNoteInSong];
          nextNoteInSong++;
          if (nextNoteInSong == numNotesInSongs[whichSong]) {
            nextNoteInSong = 0;
          }
          usbMIDI.sendNoteOn(treadNoteAssignments[treadNumber], 127, 1);
          Serial.println(treadNoteAssignments[treadNumber]);
          needsLighting[treadNumber] = 1;
          Serial.println(treadNumber);
        } else {
          usbMIDI.sendNoteOn(tread->notes[0], 127, (1));
          //lightTread(tread->ledStart, tread->color);
          needsLighting[treadNumber] = 1;
          Serial.println(treadNumber);
        }
      } else {
        for(int i = 0; i < tread->numNotes; i++) {
          usbMIDI.sendNoteOn(tread->notes[i], 127, (1));
          //lightTread(tread->ledStart, tread->color);
          needsLighting[treadNumber] = 1;
          Serial.println(treadNumber);
        }
      }
      //digitalWrite(13,HIGH);
      states[treadNumber] = 1;
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
    //for (int i=0; i<numberOfTreads; i++) {
      pinMode(A3,INPUT);
      pinMode(A4,INPUT);
      pinMode(A5,INPUT);
      pinMode(A8,INPUT);
      pinMode(A9,INPUT);
      pinMode(A11,INPUT_PULLUP);
      pinMode(A14,INPUT_PULLUP);
      pinMode(0,OUTPUT);
    //}
    leds.begin();
    leds.show();
    digitalWrite(13,LOW);
    Serial.begin(9600);
    Serial.print(leds.numPixels());
    Serial.print(" pixels ===================================================");
    //bmemcpy(treads, smokeTreads, sizeof(treads));
}

elapsedMillis sinceLastReading = 0;

void loop() {
  // LED Animation:
  if (sinceLastLedScan > 64) {
    for(int i=0; i<numberOfTreads; i++) {
        if (needsLighting[i] == 1) {
          leds.setPixel(nextLefts[i], colors[i]);
          leds.setPixel(nextRights[i], colors[i]);
          nextLefts[i]--;
          nextRights[i]++;
          if (nextLefts[i]  <  ledStarts[i]) {
            needsLighting[i] = 0;
          }
        } else if (needsLighting[i] == 2) {
          leds.setPixel(nextLefts[i], LEDOFF);
          leds.setPixel(nextRights[i], LEDOFF);
          nextLefts[i]++;
          nextRights[i]--;
          if (nextLefts[i]  >  (ledStarts[i] + (ledsPerStrip/2 -1))) {
            needsLighting[i] = 2;
            nextLefts[i] = (ledStarts[i] + (ledsPerStrip/2 -1));
            nextRights[i] = (ledStarts[i] + (ledsPerStrip/2));
          }
        }
      }
      leds.show();
      sinceLastLedScan = 0;
  }

    
 // Tread scanning:   
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

  for (treadNumber = 0; treadNumber < numberOfTreads; treadNumber++) {
    readTread();
  }

} // Loop End
