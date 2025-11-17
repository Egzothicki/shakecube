/*
 * SHAKE CUBE - Interactive LED Animation Device
 * 
 * Hardware:
 * - ESP8266 (NodeMCU v2)
 * - 8x8 LED Matrix (MAX7219)
 * - Vibration Sensor (SW-420)
 * 
 * Features:
 * - Detects shaking and plays random animations
 * - WiFi access point for wireless updates
 * - 6 different animations + boot animation
 * 
 * Update via phone: Connect to "ShakeCube-Config" WiFi
 * Then visit http://192.168.4.1/update
 */

#include "LedControl.h"
#include <ArduinoOTA.h>
#include <ESP8266HTTPUpdateServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>

// ============================================
// CONFIGURATION
// ============================================

// Pin Definitions
const int MATRIX_DIN_PIN = 13;  // D7
const int MATRIX_CLK_PIN = 14;  // D5
const int MATRIX_CS_PIN = 15;   // D8
const int SENSOR_PIN = D1;      // GPIO 5 (Vibration sensor)

// Settings
const int LED_BRIGHTNESS = 8;    // 0-15 (higher = brighter)
const int SHAKE_DEBOUNCE = 1000; // Milliseconds between animations
const int NUM_ANIMATIONS = 5;    // Number of shake animations

// WiFi Access Point
const char* AP_NAME = "ShakeCube-Config";
const char* AP_PASSWORD = "shakecube123";

// ============================================
// HARDWARE SETUP
// ============================================

LedControl lc = LedControl(MATRIX_DIN_PIN, MATRIX_CLK_PIN, MATRIX_CS_PIN, 1);
ESP8266WebServer httpServer(80);
ESP8266HTTPUpdateServer httpUpdater;

unsigned long lastShakeTime = 0;

// ============================================
// DISPLAY HELPER FUNCTIONS
// ============================================
// Note: Display is mounted upside-down, so coordinates are flipped

void setLed(int row, int col, bool state) {
  lc.setLed(0, 7 - row, 7 - col, state);
}

void setRow(int row, byte value) {
  // Reverse the bits for upside-down display
  byte reversed = 0;
  for (int i = 0; i < 8; i++) {
    if (value & (1 << i)) {
      reversed |= (1 << (7 - i));
    }
  }
  lc.setRow(0, 7 - row, reversed);
}

// ============================================
// FONT DATA - 5x8 font for scrolling text
// ============================================
// Based on popular LED matrix fonts from Arduino community
const byte font5x8[][5] PROGMEM = {
  {0x00, 0x00, 0x00, 0x00, 0x00}, // 0: Space
  {0x00, 0x00, 0x5F, 0x00, 0x00}, // 1: !
  {0x7C, 0x12, 0x11, 0x12, 0x7C}, // 2: A
  {0x7F, 0x49, 0x49, 0x49, 0x36}, // 3: B
  {0x3E, 0x41, 0x41, 0x41, 0x22}, // 4: C
  {0x7F, 0x41, 0x41, 0x22, 0x1C}, // 5: D
  {0x7F, 0x49, 0x49, 0x49, 0x41}, // 6: E
  {0x7F, 0x09, 0x09, 0x09, 0x01}, // 7: F
  {0x3E, 0x41, 0x49, 0x49, 0x7A}, // 8: G
  {0x7F, 0x08, 0x08, 0x08, 0x7F}, // 9: H
  {0x00, 0x41, 0x7F, 0x41, 0x00}, // 10: I
  {0x20, 0x40, 0x41, 0x3F, 0x01}, // 11: J
  {0x7F, 0x08, 0x14, 0x22, 0x41}, // 12: K
  {0x7F, 0x40, 0x40, 0x40, 0x40}, // 13: L
  {0x7F, 0x02, 0x0C, 0x02, 0x7F}, // 14: M
  {0x7F, 0x04, 0x08, 0x10, 0x7F}, // 15: N
  {0x3E, 0x41, 0x41, 0x41, 0x3E}, // 16: O
  {0x7F, 0x09, 0x09, 0x09, 0x06}, // 17: P
  {0x3E, 0x41, 0x51, 0x21, 0x5E}, // 18: Q
  {0x7F, 0x09, 0x19, 0x29, 0x46}, // 19: R
  {0x46, 0x49, 0x49, 0x49, 0x31}, // 20: S
  {0x01, 0x01, 0x7F, 0x01, 0x01}, // 21: T
  {0x3F, 0x40, 0x40, 0x40, 0x3F}, // 22: U
  {0x1F, 0x20, 0x40, 0x20, 0x1F}, // 23: V
  {0x3F, 0x40, 0x38, 0x40, 0x3F}, // 24: W
  {0x63, 0x14, 0x08, 0x14, 0x63}, // 25: X
  {0x07, 0x08, 0x70, 0x08, 0x07}, // 26: Y
  {0x61, 0x51, 0x49, 0x45, 0x43}, // 27: Z
};

// ============================================
// EPIC ANIMATIONS FROM INTERNET/MAKER COMMUNITY
// ============================================

// ANIMATION 1: Pac-Man - Classic arcade animation with trail dimming
void animationPacMan(unsigned long duration) {
  unsigned long startTime = millis();
  
  // Pac-Man sprites (open/closed mouth)
  byte pacOpen[8] = {B00111100, B01111000, B11110000, B11100000, B11110000, B01111000, B00111100, B00000000};
  byte pacClosed[8] = {B00111100, B01111110, B11111110, B11111110, B11111110, B01111110, B00111100, B00000000};
  
  // Dots
  byte dots[8] = {B00000000, B00000000, B00000000, B01010101, B00000000, B00000000, B00000000, B00000000};
  
  // Trail memory for dimming effect
  bool trail[8][8] = {false};
  int trailAge[8][8] = {0};
  
  while (millis() - startTime < duration) {
    for (int pos = -8; pos < 16; pos++) {
      lc.clearDisplay(0);
      
      // Age the trail
      for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
          if (trailAge[row][col] > 0) trailAge[row][col]++;
        }
      }
      
      // Draw dots being eaten
      for (int row = 0; row < 8; row++) {
        byte dotPattern = dots[row];
        if (pos >= 0 && pos < 8) {
          dotPattern &= ~(0xFF << pos);  // Clear eaten dots
        }
        setRow(row, dotPattern);
      }
      
      // Draw Pac-Man and mark trail
      if (pos >= 0 && pos < 8) {
        byte* sprite = (pos % 2 == 0) ? pacOpen : pacClosed;
        for (int row = 0; row < 8; row++) {
          byte combined = dots[row] | (sprite[row] >> pos);
          setRow(row, combined);
          
          // Mark pac-man position in trail
          if (sprite[row] & (0x80 >> pos)) {
            trail[row][pos] = true;
            trailAge[row][pos] = 1;
          }
        }
      }
      
      // Draw dimming trail behind pac-man (show older positions with decreasing probability)
      for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
          if (trail[row][col] && trailAge[row][col] > 1 && trailAge[row][col] < 6) {
            // Show with decreasing probability based on age
            if (random(0, trailAge[row][col]) == 0) {
              setLed(row, col, true);
            }
          }
          // Clear very old trail
          if (trailAge[row][col] > 5) {
            trail[row][col] = false;
            trailAge[row][col] = 0;
          }
        }
      }
      
      delay(150);
      ArduinoOTA.handle();
    }
  }
  lc.clearDisplay(0);
}

// ANIMATION 2: Heart Beat - Popular pulsing heart with smooth dimming
void animationHeartBeat(unsigned long duration) {
  unsigned long startTime = millis();
  
  // Heart patterns (small to large) from LED matrix tutorials
  byte heartSmall[8] = {B00000000, B01100110, B11111111, B11111111, B01111110, B00111100, B00011000, B00000000};
  byte heartLarge[8] = {B01100110, B11111111, B11111111, B11111111, B11111111, B01111110, B00111100, B00011000};
  
  while (millis() - startTime < duration) {
    // Beat 1 - with fade in/out using intensity
    for (int row = 0; row < 8; row++) setRow(row, heartSmall[row]);
    delay(100);
    
    // Fade up
    for (int brightness = 4; brightness <= LED_BRIGHTNESS; brightness += 2) {
      lc.setIntensity(0, brightness);
      delay(20);
    }
    for (int row = 0; row < 8; row++) setRow(row, heartLarge[row]);
    delay(100);
    
    // Fade down
    for (int brightness = LED_BRIGHTNESS; brightness >= 4; brightness -= 2) {
      lc.setIntensity(0, brightness);
      delay(20);
    }
    lc.setIntensity(0, LED_BRIGHTNESS); // Reset
    for (int row = 0; row < 8; row++) setRow(row, heartSmall[row]);
    delay(100);
    
    // Beat 2
    for (int brightness = 4; brightness <= LED_BRIGHTNESS; brightness += 2) {
      lc.setIntensity(0, brightness);
      delay(20);
    }
    for (int row = 0; row < 8; row++) setRow(row, heartLarge[row]);
    delay(100);
    
    for (int brightness = LED_BRIGHTNESS; brightness >= 4; brightness -= 2) {
      lc.setIntensity(0, brightness);
      delay(20);
    }
    lc.setIntensity(0, LED_BRIGHTNESS); // Reset
    for (int row = 0; row < 8; row++) setRow(row, heartSmall[row]);
    delay(300);
    
    ArduinoOTA.handle();
  }
  lc.setIntensity(0, LED_BRIGHTNESS); // Ensure reset
  lc.clearDisplay(0);
}

// ANIMATION 3: Fireworks - Exploding particles with dimming fade
void animationFireworks(unsigned long duration) {
  unsigned long startTime = millis();
  
  while (millis() - startTime < duration) {
    int centerX = random(2, 6);
    int centerY = random(2, 6);
    
    // Launch
    for (int i = 7; i > centerY; i--) {
      lc.clearDisplay(0);
      setLed(i, centerX, true);
      delay(50);
      ArduinoOTA.handle();
    }
    
    // Store explosion pattern for dimming effect
    bool explosion[8][8] = {false};
    
    // Explode outward - build up pattern
    for (int radius = 0; radius < 5; radius++) {
      for (int angle = 0; angle < 16; angle++) {
        float rad = angle * 3.14159 / 8.0;
        int x = centerX + (int)(radius * cos(rad));
        int y = centerY + (int)(radius * sin(rad));
        if (x >= 0 && x < 8 && y >= 0 && y < 8) {
          explosion[y][x] = true;
        }
      }
      
      // Draw current explosion state
      lc.clearDisplay(0);
      for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
          if (explosion[row][col]) setLed(row, col, true);
        }
      }
      delay(80);
      ArduinoOTA.handle();
    }
    
    // Dimming effect - fade out by showing progressively fewer LEDs
    for (int fade = 0; fade < 4; fade++) {
      lc.clearDisplay(0);
      for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
          // Only show LED based on fade level and random chance
          if (explosion[row][col] && random(0, fade + 1) == 0) {
            setLed(row, col, true);
          }
        }
      }
      delay(100);
      ArduinoOTA.handle();
    }
    
    delay(200);
  }
  lc.clearDisplay(0);
}

// ANIMATION 4: Snake/Worm - Slithering across display with fading tail
void animationSnake(unsigned long duration) {
  unsigned long startTime = millis();
  
  int snakeX[32], snakeY[32];
  int snakeLength = 10;
  int headX = 0, headY = 3;
  int dirX = 1, dirY = 0;
  
  // Initialize snake
  for (int i = 0; i < snakeLength; i++) {
    snakeX[i] = headX - i;
    snakeY[i] = headY;
  }
  
  while (millis() - startTime < duration) {
    // Move head
    headX += dirX;
    headY += dirY;
    
    // Bounce off walls
    if (headX < 0 || headX >= 8) { dirX = -dirX; headX += dirX * 2; }
    if (headY < 0 || headY >= 8) { dirY = -dirY; headY += dirY * 2; }
    
    // Random direction changes
    if (random(0, 10) == 0) {
      if (dirX != 0) { dirX = 0; dirY = random(0, 2) * 2 - 1; }
      else { dirY = 0; dirX = random(0, 2) * 2 - 1; }
    }
    
    // Update body
    for (int i = snakeLength - 1; i > 0; i--) {
      snakeX[i] = snakeX[i-1];
      snakeY[i] = snakeY[i-1];
    }
    snakeX[0] = headX;
    snakeY[0] = headY;
    
    // Draw snake with fading tail (simulate dimming by skipping tail segments)
    lc.clearDisplay(0);
    for (int i = 0; i < snakeLength; i++) {
      if (snakeX[i] >= 0 && snakeX[i] < 8 && snakeY[i] >= 0 && snakeY[i] < 8) {
        // Tail segments fade out - show with decreasing probability
        int fadeChance = i * 2; // Tail gets dimmer
        if (i < 3 || random(0, fadeChance) < 5) {  // Head always shows, tail fades
          setLed(snakeY[i], snakeX[i], true);
        }
      }
    }
    
    delay(120);
    ArduinoOTA.handle();
  }
  lc.clearDisplay(0);
}

// ANIMATION 5: Scrolling Text - "SHAKEN!" 
void animationScrollText(unsigned long duration) {
  unsigned long startTime = millis();
  const char* text = "SHAKEN!";
  int textLen = strlen(text);
  
  while (millis() - startTime < duration) {
    for (int scroll = 0; scroll < textLen * 6 + 8; scroll++) {
      lc.clearDisplay(0);
      
      for (int charIndex = 0; charIndex < textLen; charIndex++) {
        int charPos = charIndex * 6 - scroll + 8;
        if (charPos >= -5 && charPos < 8) {
          char c = text[charIndex];
          int fontIndex = 0;
          
          // Map characters to font
          if (c == ' ') fontIndex = 0;
          else if (c == '!') fontIndex = 1;
          else if (c >= 'A' && c <= 'Z') fontIndex = c - 'A' + 2;
          else continue;
          
          // Draw character
          for (int col = 0; col < 5; col++) {
            if (charPos + col >= 0 && charPos + col < 8) {
              byte columnData = pgm_read_byte(&font5x8[fontIndex][col]);
              for (int row = 0; row < 8; row++) {
                if (columnData & (1 << row)) {
                  setLed(row, charPos + col, true);
                }
              }
            }
          }
        }
      }
      
      delay(80);
      ArduinoOTA.handle();
    }
  }
  lc.clearDisplay(0);
}

// BOOT ANIMATION: Hyperspace Jump - Stars flying toward you
void accelerationAnimation() {
  lc.clearDisplay(0);

  struct Star {
    float row, col, speed;
  };
  Star stars[12];

  // Initialize stars near center
  for (int i = 0; i < 12; i++) {
    stars[i].row = 3.5 + random(-10, 10) / 10.0;
    stars[i].col = 3.5 + random(-10, 10) / 10.0;
    stars[i].speed = 0.1 + (i * 0.05);
  }

  // Animate stars flying outward (approaching viewer)
  for (int frame = 0; frame < 50; frame++) {
    lc.clearDisplay(0);

    for (int i = 0; i < 12; i++) {
      // Calculate direction from center
      float dirRow = stars[i].row - 3.5;
      float dirCol = stars[i].col - 3.5;
      float dist = sqrt(dirRow * dirRow + dirCol * dirCol);
      
      if (dist > 0.1) {
        dirRow /= dist;
        dirCol /= dist;
      }

      // Move star outward with acceleration
      float accel = 1.0 + (frame * 0.08);
      stars[i].row += dirRow * stars[i].speed * accel;
      stars[i].col += dirCol * stars[i].speed * accel;

      // Draw star if in bounds
      int r = (int)stars[i].row;
      int c = (int)stars[i].col;
      if (r >= 0 && r < 8 && c >= 0 && c < 8) {
        setLed(r, c, true);

        // Draw trail for faster stars
        if (frame > 20 && stars[i].speed > 0.3) {
          int prevR = (int)(stars[i].row - dirRow * stars[i].speed * accel * 0.5);
          int prevC = (int)(stars[i].col - dirCol * stars[i].speed * accel * 0.5);
          if (prevR >= 0 && prevR < 8 && prevC >= 0 && prevC < 8) {
            setLed(prevR, prevC, true);
          }
        }
      }

      // Reset star if off-screen
      if (r < 0 || r >= 8 || c < 0 || c >= 8) {
        stars[i].row = 3.5 + random(-5, 5) / 10.0;
        stars[i].col = 3.5 + random(-5, 5) / 10.0;
      }
    }

    delay(max(5, 40 - frame));  // Speed up over time
  }

  // Final hyperspace flash
  for (int row = 0; row < 8; row++) {
    setRow(row, B11111111);
  }
  delay(150);
  lc.clearDisplay(0);
}

// ============================================
// ANIMATION CONTROLLER
// ============================================

void playRandomAnimation() {
  int choice = random(0, NUM_ANIMATIONS);
  unsigned long duration = random(5000, 10001);  // 5-10 seconds

  Serial.print("Animation #");
  Serial.print(choice);
  Serial.print(" for ");
  Serial.print(duration / 1000.0);
  Serial.println("s");

  switch (choice) {
    case 0: animationPacMan(duration); break;
    case 1: animationHeartBeat(duration); break;
    case 2: animationFireworks(duration); break;
    case 3: animationSnake(duration); break;
    case 4: animationScrollText(duration); break;
  }
}

// ============================================
// SETUP
// ============================================

void setup() {
  Serial.begin(115200);
  Serial.println("\n=== SHAKE CUBE BOOTING ===");

  // Initialize LED Matrix
  lc.shutdown(0, false);
  lc.setIntensity(0, LED_BRIGHTNESS);
  lc.clearDisplay(0);

  // Create WiFi Access Point
  Serial.println("Starting WiFi AP...");
  WiFi.mode(WIFI_AP);
  WiFi.softAP(AP_NAME, AP_PASSWORD);
  Serial.print("Connect to: ");
  Serial.print(AP_NAME);
  Serial.print(" (password: ");
  Serial.print(AP_PASSWORD);
  Serial.println(")");
  Serial.print("Update URL: http://");
  Serial.print(WiFi.softAPIP());
  Serial.println("/update");

  // Setup OTA updates
  ArduinoOTA.setHostname("ShakeCube");
  ArduinoOTA.begin();
  httpUpdater.setup(&httpServer, "/update");
  httpServer.begin();

  // Play boot animation
  accelerationAnimation();

  // Display "OK" pattern to confirm update
  delay(200);
  setRow(1, B01111110);
  setRow(2, B10000001);
  setRow(3, B10000001);
  setRow(4, B10000001);
  setRow(5, B10000001);
  setRow(6, B01111110);
  delay(1500);
  lc.clearDisplay(0);

  // Setup vibration sensor
  pinMode(SENSOR_PIN, INPUT);
  randomSeed(analogRead(A0));

  Serial.println("=== READY TO SHAKE! ===\n");
}

// ============================================
// MAIN LOOP
// ============================================

void loop() {
  // Handle OTA and web server
  ArduinoOTA.handle();
  httpServer.handleClient();

  // Read vibration sensor (LOW = shake detected)
  if (digitalRead(SENSOR_PIN) == LOW) {
    // Debounce: Only trigger if enough time has passed
    if (millis() - lastShakeTime > SHAKE_DEBOUNCE) {
      Serial.println("\n🎲 SHAKE DETECTED!");
      
      lastShakeTime = millis();
      playRandomAnimation();
      
      lc.clearDisplay(0);
      Serial.println("✓ Ready for next shake!\n");
    }
  }

  delay(10);
}