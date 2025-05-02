/*
 * Happy Birthday Song with:
 * - Faster 7-segment letter changes (300ms)
 * - Buzzer (pin 6)
 * - 4 LEDs (pins 2-5)
 * - 7-segment display (pins 7-13)
 */

#define BUZZER_PIN 6
#define LED1_PIN 2
#define LED2_PIN 3
#define LED3_PIN 4
#define LED4_PIN 5

// 7-segment pins (a-g)
const int segmentPins[] = {7, 8, 9, 10, 11, 12, 13};

// Patterns for 7-segment letters (a-g segments, 1=on, 0=off)
const byte letterPatterns[26][7] = {
  {1,1,1,0,1,1,1}, // A
  {0,0,1,1,1,1,1}, // B
  {1,0,0,1,1,1,0}, // C
  {0,1,1,1,1,0,1}, // D
  {1,0,0,1,1,1,1}, // E
  {1,0,0,0,1,1,1}, // F
  {1,1,1,1,0,1,1}, // G
  {0,1,1,0,1,1,1}, // H
  {0,1,1,0,0,0,0}, // I
  {0,1,1,1,1,0,0}, // J
  {0,0,0,0,1,1,1}, // K
  {0,0,0,1,1,1,0}, // L
  {1,0,1,0,1,0,0}, // M
  {0,0,1,0,1,0,1}, // N
  {1,1,1,1,1,1,0}, // O
  {1,1,0,0,1,1,1}, // P
  {1,1,1,0,0,1,1}, // Q
  {1,1,0,0,1,1,0}, // R
  {1,0,1,1,0,1,1}, // S
  {0,0,0,1,1,1,1}, // T
  {0,1,1,1,1,1,0}, // U
  {0,1,0,1,0,1,0}, // V
  {0,1,0,1,1,1,0}, // W
  {0,1,1,0,1,1,1}, // X
  {0,1,1,1,0,1,1}, // Y
  {1,1,0,1,1,0,1}  // Z
};

// Notes definitions
#define NOTE_C4  262
#define NOTE_D4  294
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_G4  392
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define REST     0

// Happy Birthday melody
int melody[] = {
  NOTE_C4, NOTE_C4, NOTE_D4, NOTE_C4, NOTE_F4, NOTE_E4,
  NOTE_C4, NOTE_C4, NOTE_D4, NOTE_C4, NOTE_G4, NOTE_F4,
  NOTE_C4, NOTE_C4, NOTE_C5, NOTE_A4, NOTE_F4, NOTE_E4, NOTE_D4,
  NOTE_AS4, NOTE_AS4, NOTE_A4, NOTE_F4, NOTE_G4, NOTE_F4
};

int noteDurations[] = {
  8, 8, 4, 4, 4, 2,
  8, 8, 4, 4, 4, 2,
  8, 8, 4, 4, 4, 4, 4,
  8, 8, 4, 4, 4, 2
};

// Letters to display
const char displayLetters[] = {'H', 'A', 'P', 'P', 'Y', 'B', 'D', 'A', 'Y'};
int currentLetter = 0;
unsigned long lastLetterChange = 0;
const int letterDisplayTime = 1; // FASTER display change (300ms)

void setup() {
  // Initialize buzzer
  pinMode(BUZZER_PIN, OUTPUT);
  
  // Initialize LEDs
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  pinMode(LED3_PIN, OUTPUT);
  pinMode(LED4_PIN, OUTPUT);
  
  // Initialize 7-segment display
  for (int i = 0; i < 7; i++) {
    pinMode(segmentPins[i], OUTPUT);
    digitalWrite(segmentPins[i], LOW);
  }
  
  // Turn all LEDs off initially
  digitalWrite(LED1_PIN, LOW);
  digitalWrite(LED2_PIN, LOW);
  digitalWrite(LED3_PIN, LOW);
  digitalWrite(LED4_PIN, LOW);
}

void displayLetter(char letter) {
  // First turn all segments off
  for (int i = 0; i < 7; i++) {
    digitalWrite(segmentPins[i], LOW);
  }
  
  // Then turn on the segments for the current letter
  if (letter >= 'A' && letter <= 'Z') {
    int index = letter - 'A';
    for (int i = 0; i < 7; i++) {
      digitalWrite(segmentPins[i], letterPatterns[index][i]);
    }
  }
}

void loop() {
  unsigned long currentTime = millis();
  
  // Change displayed letter faster (every 300ms)
  if (currentTime - lastLetterChange > letterDisplayTime) {
    lastLetterChange = currentTime;
    currentLetter = (currentLetter + 1) % 9;
    displayLetter(displayLetters[currentLetter]);
  }

  // Play music and blink LEDs
  for (int thisNote = 0; thisNote < 25; thisNote++) {
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(BUZZER_PIN, melody[thisNote], noteDuration);
    
    // Light up LEDs in sequence
    int activeLED = (thisNote % 4) + 1;
    digitalWrite(LED1_PIN, (activeLED == 1) ? HIGH : LOW);
    digitalWrite(LED2_PIN, (activeLED == 2) ? HIGH : LOW);
    digitalWrite(LED3_PIN, (activeLED == 3) ? HIGH : LOW);
    digitalWrite(LED4_PIN, (activeLED == 4) ? HIGH : LOW);
    
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    noTone(BUZZER_PIN);
  }
  
  // Turn all LEDs off between songs
  digitalWrite(LED1_PIN, LOW);
  digitalWrite(LED2_PIN, LOW);
  digitalWrite(LED3_PIN, LOW);
  digitalWrite(LED4_PIN, LOW);
  delay(2000);
}
