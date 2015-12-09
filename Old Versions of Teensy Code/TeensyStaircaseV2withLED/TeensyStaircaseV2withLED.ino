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
struct Tread treads[][4] = {{ {A0, 2, 1, {52,56,59}, 0}, 
                              {A1, 3, 1, {53,57,60}, 0}, 
                              {A2, 4, 1, {55,59,62}, 0},
                              {A3, 5, 1, {57,61,64}, 0}  }};

//Beethonven's Fifth Chords defined
struct Tread fifthTreads[][4] =  {{ {A0, 2, 3, {47,51,54}, 0}, 
                                    {A1, 3, 3, {48,52,55}, 0}, 
                                    {A2, 4, 3, {50,54,57}, 0},
                                    {A3, 5, 3, {52,56,59}, 0}  }}; //


/*//Smoke on the Water
struct Tread smokeTreads[][4] = {{ {A0, 2, 3, {60,64,67}, 0}, 
                                    {A1, 3, 3, {63,67,70}, 0}, 
                                    {A2, 4, 3, {65,69,72}, 0},
                                    {A3, 5, 3, {66,70,73}, 0}  }};

*/
//Smoke on the Water
struct Tread smokeTreads[][4] = {{ {A0, 2, 3, {48,52,55}, 0}, 
                                    {A1, 3, 3, {51,55,58}, 0}, 
                                    {A2, 4, 3, {53,57,60}, 0},
                                    {A3, 5, 3, {54,58,61}, 0}  }};


//Mary Had Lamb with Chords defined
struct Tread maryTreads[][4] = {{ {A0, 2, 1, {52,56,59}, 0}, 
                                  {A1, 3, 1, {53,57,60}, 0}, 
                                  {A2, 4, 1, {55,59,62}, 0},
                                  {A3, 5, 1, {57,61,64}, 0}  }};


//Louie Louie
struct Tread louieTreads[][4] =  {{ {A0, 2, 1, {55,59,62}, 0}, 
                                    {A1, 3, 1, {62,66,69}, 0}, 
                                    {A2, 4, 1, {64,68,71}, 0},
                                    {A3, 5, 1, {65,69,72}, 0}  }};

// Carol of the Bells Melody NO CHORDS
struct Tread bellMelTreads[][4] =    {{ {A0, 2, 1, {67,0,0}, 0}, 
                                        {A1, 3, 1, {68,0,0}, 0}, 
                                        {A2, 4, 1, {69,0,0}, 0},
                                        {A3, 5, 1, {70,0,0}, 0}  }};

// Carol of the Bells Bass
struct Tread bellBassTreads[][4] =   {{ {A0, 2, 1, {50,54,57}, 0}, 
                                        {A1, 3, 1, {52,56,59}, 0}, 
                                        {A2, 4, 1, {53,57,60}, 0},
                                        {A3, 5, 1, {55,59,62}, 0}  }};

/*
struct Tread treads[][4] = {{ {A0, 2, 3, {55,61,64}, 0}, 
                              {A1, 3, 3, {62,61,64}, 0}, 
                              {A2, 4, 3, {64,66,69}, 0},
                              {A3, 5, 3, {65,67,71}, 0}  },
    
                            { {A0, 2, 3, {36,40,43}, 0}, 
                              {A1, 3, 3, {59,64,67}, 0}, 
                              {A2, 4, 3, {60,68,71}, 0},
                              {A3, 5, 3, {62,71,74}, 0}  },
    
                            { {A0, 2, 3, {38,42,45}, 0}, 
                              {A1, 3, 3, {60,66,69}, 0}, 
                              {A2, 4, 3, {62,70,73}, 0},
                              {A3, 5, 3, {65,73,76}, 0}  }};


// Higher step Chords not filled out bluesy riffer
struct Tread treads[][4] = {{ {A0, 2, 3, {BASSKEY,(BASSKEY+4),(BASSKEY+7)}, 0}, 
                          {A1, 3, 1, {MELODYKEY,(MELODYKEY+4),(MELODYKEY+7)}, 0}, 
                          {A2, 4, 1, {(MELODYKEY+4),(MELODYKEY+8),(MELODYKEY+11)}, 0},
                          {A3, 5, 1, {(MELODYKEY+7),(MELODYKEY+11),(MELODYKEY+14)}, 0}  },

                        { {A0, 2, 3, {(BASSKEY+5),(BASSKEY+9),(BASSKEY+12)}, 0}, 
                          {A1, 3, 1, {(MELODYKEY+4),(MELODYKEY+9),(MELODYKEY+12)}, 0}, // keep
                          {A2, 4, 1, {(MELODYKEY+5),(MELODYKEY+13),(MELODYKEY+16)}, 0},
                          {A3, 5, 1, {(MELODYKEY+7),(MELODYKEY+16),(MELODYKEY+19)}, 0}  },

                        { {A0, 2, 3, {(BASSKEY+7),(BASSKEY+11),(BASSKEY+14)}, 0}, 
                          {A1, 3, 1, {(MELODYKEY+5),(MELODYKEY+11),(MELODYKEY+14)}, 0}, 
                          {A2, 4, 1, {(MELODYKEY+7),(MELODYKEY+15),(MELODYKEY+18)}, 0},
                          {A3, 5, 1, {(MELODYKEY+10),(MELODYKEY+18),(MELODYKEY+21)}, 0}  }};

struct Tread treads[][4] = {{ {A0, 2, 3, {31,35,38}, 0}, 
                          {A1, 3, 1, {55,59,62}, 0}, 
                          {A2, 4, 1, {59,63,66}, 0},
                          {A3, 5, 1, {62,66,69}, 0}  },

                        { {A0, 2, 3, {36,40,43}, 0}, 
                          {A1, 3, 1, {59,64,67}, 0}, 
                          {A2, 4, 1, {60,68,71}, 0},
                          {A3, 5, 1, {62,71,74}, 0}  },

                        { {A0, 2, 3, {38,42,45}, 0}, 
                          {A1, 3, 1, {60,66,69}, 0}, 
                          {A2, 4, 1, {62,70,73}, 0},
                          {A3, 5, 1, {65,73,76}, 0}  }};



struct Tread treads[][4] = {{ {A0, 2, 3, {BASSKEY,(BASSKEY+4),(BASSKEY+7)}, 0}, 
                          {A1, 3, 3, {MELODYKEY,(MELODYKEY+4),(MELODYKEY+7)}, 0}, 
                          {A2, 4, 3, {(MELODYKEY+4),(MELODYKEY+8),(MELODYKEY+11)}, 0},
                          {A3, 5, 3, {(MELODYKEY+7),(MELODYKEY+11),(MELODYKEY+14)}, 0}  },

                        { {A0, 2, 3, {(BASSKEY+5),(BASSKEY+9),(BASSKEY+12)}, 0}, 
                          {A1, 3, 3, {(MELODYKEY+5),(MELODYKEY+9),(MELODYKEY+12)}, 0}, 
                          {A2, 4, 3, {(MELODYKEY+9),(MELODYKEY+13),(MELODYKEY+16)}, 0},
                          {A3, 5, 3, {(MELODYKEY+12),(MELODYKEY+16),(MELODYKEY+19)}, 0}  },

                        { {A0, 2, 3, {(BASSKEY+7),(BASSKEY+11),(BASSKEY+14)}, 0}, 
                          {A1, 3, 3, {(MELODYKEY+7),(MELODYKEY+11),(MELODYKEY+14)}, 0}, 
                          {A2, 4, 3, {(MELODYKEY+11),(MELODYKEY+15),(MELODYKEY+18)}, 0},
                          {A3, 5, 3, {(MELODYKEY+14),(MELODYKEY+18),(MELODYKEY+21)}, 0}  }};

struct Tread treads[][4] = {{ {A0, 2, 3, {31,35,38}, 0}, 
                          {A1, 3, 3, {55,59,62}, 0}, 
                          {A2, 4, 3, {59,63,66}, 0},
                          {A3, 5, 3, {62,66,69}, 0}  },

                        { {A0, 2, 3, {36,40,43}, 0}, 
                          {A1, 3, 3, {60,64,67}, 0}, 
                          {A2, 4, 3, {64,68,71}, 0},
                          {A3, 5, 3, {67,71,74}, 0}  },

                        { {A0, 2, 3, {38,42,45}, 0}, 
                          {A1, 3, 3, {62,66,69}, 0}, 
                          {A2, 4, 3, {66,70,73}, 0},
                          {A3, 5, 3, {69,73,76}, 0}  }};




struct Tread treads[] = { {A0, 2, 3, {55,59,62}, 0}, 
                          {A1, 3, 3, {56,60,63}, 0}, 
                          {A2, 4, 3, {57,61,64}, 0},
                          {A3, 5, 3, {58,62,65}, 0}  };

struct Tread treads[] = { {A0, 2, 4, {50,52,59,60}, 0}, 
                          {A1, 3, 4, {48,52,59,62}, 0}, 
                          {A2, 4, 2, {65,69,0,0}, 0},
                          {A3, 5, 4, {50,52,59,60}, 0} };

struct Tread treads[] = { {A0, 2, 3, {50,66,69,0}, 0}, 
                          {A1, 3, 3, {55,59,62,0}, 0}, 
                          {A2, 4, 3, {59,63,66,0}, 0},
                          {A3, 5, 3, {62,66,70,0}, 0} };
*/
int progression[4] = {0,0,0,0};
int numMeasures = 4;
//int progression[12] = {0,0,0,0,1,1,0,0,2,1,0,2}; // 12 bar blues
//int progression[4] = {0,0,2,1}; // 4 bar blues


int measure = 0;
int myindex = 0;
int lastSong = 0;

void readTread(struct Tread * tread) {
  digitalWrite(tread->enablePin, HIGH);
  delay(25); // min 15
  int reading = analogRead(tread->pin);
  //if (myindex == 3) {
    Serial.print(myindex);
    Serial.print(" ");
    Serial.print(reading);
    if (myindex == 3) {
      Serial.println(" ");
    }
  //}
  digitalWrite(tread->enablePin, LOW);                    

    /*if(abs(reading - tread->lastReading) >= THRESHOLD) {
        pot->currentReading = (reading/2);
        //Serial.println(pot->currentReading);
        pot->lastReading = reading;
    }*/
    
  if(reading < sensitivity) {
    if(tread->state == 1) {
      if(digitalRead(9)) {
        usbMIDI.sendNoteOff(tread->notes[0], 0, (1));
      } else {
        for(int i = 0; i < tread->numNotes; i++) {
          usbMIDI.sendNoteOff(tread->notes[i], 0, (1));
        }
      }
      //digitalWrite(13,LOW);
      tread->state = 0;
      
      if (myindex == 0) {
        if (measure == 3) {
          measure = 0;
        } else {
          measure++;
        }
      }

      
    }
  } else {
    if(tread->state == 0) {
      if(digitalRead(9)) {
        usbMIDI.sendNoteOn(tread->notes[0], 127, (1));
        Serial.println(myindex);
      } else {
        for(int i = 0; i < tread->numNotes; i++) {
          usbMIDI.sendNoteOn(tread->notes[i], 127, (1));
          Serial.println(myindex);
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
    delayMicroseconds(wait);
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
    Serial.print(" pixels");
}

void loop() {
    int song = analogRead(A4);
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
  
  sensitivity = analogRead(A5);
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
  
  readTread(&treads[progression[measure]][myindex]);

  if (myindex == (numMeasures - 1)) {
    myindex = 0;
  } else {
    myindex++;
  }
}
