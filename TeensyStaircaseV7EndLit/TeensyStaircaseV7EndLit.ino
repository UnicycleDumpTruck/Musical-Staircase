#include <OctoWS2811.h> // Library for LED driving shield on Teensy

const int ledsPerStrip = 8;   // Neopixel strips currently cut into 8 LED sections, each section wired as a separate strip on the controller.
const int numberOfTreads = 6; // Number of Treads currently implemented in hardware

int nextNoteInSong = 0;             // Keeps track of next note to play in chosen song array
int numNotesInSongs[] = {47, 120};  // Since notes array must be padded to largest song, store the length here

int notesInSongs[][120] = {   { 67, 72, 72, 72, 74, 76, 76, 76, 74, 72, 74, 76, 72, 76, 76, 77, 79, 79, 77, 76,
                                77, 79, 76, 72, 72, 74, 76, 76, 74, 72, 74, 76, 72, 67, 67, 72, 72, 72, 74, 
                                76, 76, 76, 74, 72, 74, 76, 72,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                0,0,0,0,0,0,0,0,0,0,0,0,0 },  // Itsy Bitsy Spider
                                    
                              { 83, 88, 88, 88, 83, 83, 83, 86, 86, 88, 
                                91, 91, 91, 91, 83,  83, 86, 86, 88, 
                                76, 76, 88, 88, 88, 90,
                                86, 86, 86, 86, 88, 86, 86, 86,
                                86, 86, 85, 83, 81, 79, 81, 79, 83, 79, 76, 
                                83, 83, 86, 88, 88, 88, 88, 88, 88, 88, 88, 88, 86,
                                88, 88, 88, 86, 88, 83, 83, 86, 88, 88, 88, 88, 88, 90,
                                86, 86, 83, 83, 88, 86, 88, 88, 86, 88, 83, 83, 86, 
                                88, 88, 95, 93, 93, 91, 88, 91, 88, 88, 86, 88, 88, 88, 88, 86, 88,
                                83, 83, 86, 88, 88, 88, 88, 88, 90, 86, 86, 83, 83,
                                88, 88, 86, 88, 86, 88 } // Happy 
              };
int treadNoteAssignments[numberOfTreads]; // Used to assign a note to a tread as it is played so it can be turned off when foot leaves
int whichSong = 1;                        // Index of chosen song in notesInSongs 2D array

// Setup for LEDs
DMAMEM int displayMemory[ledsPerStrip*numberOfTreads];
int drawingMemory[ledsPerStrip*numberOfTreads];
const int config = WS2811_GRB | WS2811_800kHz;
OctoWS2811 leds(ledsPerStrip, displayMemory, drawingMemory, config);

// Colors for LEDs
#define RED       0xFF0000
#define GREEN     0x00FF00
#define BLUE      0x0000FF
#define YELLOW    0xFFFF00
#define PINK      0xFF1088
#define ORANGE    0xE05800
#define WHITELED  0xFFFFFF
#define LEDOFF    0x000000

#define DEBUG 0                   // Enables various Serial.print commands
#define PLAYINGSONG 0             // When set to 1, whatever tread is activated will play the next not in the chosen song array
#define PLAYCHORDS 0              // Plays notes assigned in Tread struct
#define VELOCITY 115              // MIDI velocity for notes played. Range is 0-127.
#define LASER_SENSITIVITY 700     // Values less than this will indicate the laser is blocked and the input has been pulled low.

int needsLighting[numberOfTreads] = {0, 0, 0, 0, 0, 0};     // 0 = off; 1 = lighting; 2 = unlighting;
elapsedMillis sinceLastLEDUpdate = 0;                       // Refresh rate for LED animations
int treadNumber = 0;                                        // Used to scan through treads and check for activation/obstruction of laser

int sustainState = 0; // Sustain Pedal to make notes last longer
int inputPins[] = {A9, A8, A5, A4, A11, A3}; // Connections to laser sensors. External 100k pullups. Lasers are NPN, pulling down upon interruption/activation.
int states[] = {0, 0, 0, 0, 0, 0}; // Store current states of treads/notes. 0 means no foot on tread. 1 means foot on tread.
int ledStarts[] = {0, ledsPerStrip, (ledsPerStrip * 2), (ledsPerStrip * 3), (ledsPerStrip * 4), (ledsPerStrip * 5)}; // Starting pixel of tread strip
int colors[] = {RED, GREEN, BLUE, YELLOW, PINK, ORANGE, WHITELED}; // Colors to light each tread, starting at bottom of stairs
int nextLit[] = {0, ledsPerStrip, (ledsPerStrip * 2), (ledsPerStrip * 3), (ledsPerStrip * 4), (ledsPerStrip * 5)}; // Next LED to light in animation

struct Tread {
    int numNotes; // 1 if no chord would work with this song, 3 or more for songs with chords
    int notes[4]; // MIDI notes
};

/* MIDI Notes
 * Octave
 *         C  C#   D  D#   E   F  F#   G  G#   A  A#   B
 * 0       0   1   2   3   4   5   6   7   8   9  10  11
 * 1      12  13  14  15  16  17  18  19  20  21  22  23
 * 2      24  25  26  27  28  29  30  31  32  33  34  35
 * 3      36  37  38  39  40  41  42  43  44  45  46  47
 * 4      48  49  50  51  52  53  54  55  56  57  58  59
 * 5      60  61  62  63  64  65  66  67  68  69  70  71
 * 6      72  73  74  75  76  77  78  79  80  81  82  83
 * 7      84  85  86  87  88  89  90  91  92  93  94  95
 * 8      96  97  98  99 100 101 102 103 104 105 106 107
 * 9     108 109 110 111 112 113 114 115 116 117 118 119
 * 10    120 121 122 123 124 125 126 127
 */

//Iron Man with Chords defined. Top tread not used in riff
struct Tread ironTreads[][6] = {{ {3, {22,29,34}}, 
                                  {3, {25,32,37}}, 
                                  {3, {27,34,39}},
                                  {3, {29,36,41}},
                                  {3, {30,37,42}},
                                  {3, {32,39,44}}
                                }};

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
    #ifdef DEBUG
      //Serial.print(treadNumber);
      //Serial.print(" ");
      /*Serial.print(reading);
      Serial.print(" ");
      if (treadNumber == 5) {
        Serial.println(" ");
      }*/
    #endif

  if(reading > LASER_SENSITIVITY) { // Tread clear of feet, turn off notes if playing

    if(states[treadNumber] == 1) {  // If note was playing
        if(!(PLAYCHORDS)) {         // without chords
          if (PLAYINGSONG) {        // with song but no chords
            usbMIDI.sendNoteOff(treadNoteAssignments[treadNumber], 0, 1); // Variables are note, velocity, channel
            needsLighting[treadNumber] = 2;
          } else {                  // no song, no chords
            usbMIDI.sendNoteOff(tread->notes[0], 0, (1)); // Variables are note, velocity, channel
            needsLighting[treadNumber] = 2;
          }
        } else {                    // with chords, no song
          for(int i = 0; i < tread->numNotes; i++) {
            usbMIDI.sendNoteOff(tread->notes[i], 0, (1)); // Variables are note, velocity, channel
            needsLighting[treadNumber] = 2;
          }
        }
      states[treadNumber] = 0;      // set state to off so we don't keep sending NoteOff commands
    }
  } else {
    if(states[treadNumber] == 0) {
      if(!(PLAYCHORDS)) {           // without chords:
        if (PLAYINGSONG) {          // with song but no chords
          treadNoteAssignments[treadNumber] = notesInSongs[whichSong][nextNoteInSong];  // assign next note in song to this tread so it can be turned off later
          nextNoteInSong++;                                                             // increment nextNoteInSong index so next tread activated can take it
          if (nextNoteInSong == numNotesInSongs[whichSong]) {                           // if we're at the end of the song
            nextNoteInSong = 0;                                                         // go back to the beginning
          }
          usbMIDI.sendNoteOn(treadNoteAssignments[treadNumber], VELOCITY, 1); // Variables are note, velocity, channel
          #ifdef DEBUG
            //Serial.print(treadNoteAssignments[treadNumber]);
            //Serial.print(treadNumber);
            //Serial.print(" sent On");
          #endif
          needsLighting[treadNumber] = 1;
        } else {                   // No song:
          usbMIDI.sendNoteOn(tread->notes[0], VELOCITY, (1)); // Variables are note, velocity, channel
          #ifdef DEBUG
            //Serial.print(treadNumber);
            //Serial.print(" sent On");
          #endif
          needsLighting[treadNumber] = 1;
        }
      } else {                    // No chords and no song:
        for(int i = 0; i < tread->numNotes; i++) {
          usbMIDI.sendNoteOn(tread->notes[i], VELOCITY, (1)); // Variables are note, velocity, channel
          needsLighting[treadNumber] = 1;
          #ifdef DEBUG
            //Serial.print(treadNumber);
            //Serial.print(" sent On");
          #endif
        }
      }
      states[treadNumber] = 1; // Set tread to on so multiple NoteOn commands don't get sent
    }
  }
}

void lightTread(int ledStart, int color) // Turns on one tread's section of LEDs
{
  int ledEnd = ledStart + ledsPerStrip;
  for (int i=ledStart; i < ledEnd; i++) {
    leds.setPixel(i, color);
  }
  leds.show();
}

void lightOff(int ledStart) // Turns off one tread's section of LEDs
{
  int ledEnd = ledStart + ledsPerStrip;
  for (int i=ledStart; i < ledEnd; i++) {
    leds.setPixel(i, LEDOFF);
  }
  leds.show();
}

void setup() {
    leds.begin();
    leds.show();
    Serial.begin(9600);
    memcpy(treads, smokeTreads, sizeof(treads)); // Copy notes for Smoke on the Water into treads
}

void loop() {
  // LED Animation:
  if (sinceLastLEDUpdate > 64) { // Refresh all LEDs every so many milliseconds
    for(int i=0; i<numberOfTreads; i++) {         // for each tread
        if (needsLighting[i] == 1) {              // if needs lighing
          leds.setPixel(nextLit[i], colors[i]);   // light the next LED to be lit
          nextLit[i]++;                           // increment the counter so the next refresh will get the next LED up
          if (nextLit[i]  >  (ledStarts[i] + (ledsPerStrip-1))) { // If we incremented past the end
            needsLighting[i] = 0;                 // this section needs no further lighting
          }
        } else if (needsLighting[i] == 2) {       // if needs unlighting (has been fully or partially lit, but foot has left tread and LEDs should go off
          leds.setPixel(nextLit[i], LEDOFF);      // turn of this LED
          nextLit[i]--;                           // count back down the strip, towards the bottom of the staircase
          if (nextLit[i]  <  (ledStarts[i])) {    // if we 
            needsLighting[i] = 0;                 // strip is fully unlit, set status to off. @@@@@@@@@@@@@@@@@@BUG?@@@@@@@@@@@@@@@@ was "2", changed to "0"
            nextLit[i] = (ledStarts[i]);          // set nextLit back to start of strip
          }
        }
      }
      leds.show();                                // Update all LEDs with above changes to buffer
      sinceLastLEDUpdate = 0;                     // Reset ellapsed millis since last refresh
  }

    
 // Tread scanning:   
  for (treadNumber = 0; treadNumber < numberOfTreads; treadNumber++) {
    readTread();
  }

  // Read pot to select which notes we want to play with for testing prototype. Not currently implemented in hardware.
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

/*
  // Read pin 10 to see if we are testing Sustain mode. Only for prototyping purposes, not currently implemented in hardware.
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
*/

} // Loop End
