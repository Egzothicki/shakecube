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
const int NUM_ANIMATIONS = 6;

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
// ANIMATIONS
// ============================================

// ANIMATION 1: Spiral Effect - Lights spiral outward from center
void animationSpiral(unsigned long duration) {
  unsigned long startTime = millis();
  
  // Spiral path coordinates (center to edges)
  int coords[][2] = {
    {3,3}, {3,4}, {4,4}, {4,3}, {4,2}, {3,2}, {2,2}, {2,3}, {2,4}, {2,5},
    {3,5}, {4,5}, {5,5}, {5,4}, {5,3}, {5,2}, {5,1}, {4,1}, {3,1}, {2,1},
    {1,1}, {1,2}, {1,3}, {1,4}, {1,5}, {1,6}, {2,6}, {3,6}, {4,6}, {5,6},
    {6,6}, {6,5}, {6,4}, {6,3}, {6,2}, {6,1}, {6,0}, {5,0}, {4,0}, {3,0},
    {2,0}, {1,0}, {0,0}, {0,1}, {0,2}, {0,3}, {0,4}, {0,5}, {0,6}, {0,7},
    {1,7}, {2,7}, {3,7}, {4,7}, {5,7}, {6,7}, {7,7}, {7,6}, {7,5}, {7,4},
    {7,3}, {7,2}, {7,1}, {7,0}
  };

  while (millis() - startTime < duration) {
    for (int i = 0; i < 64; i++) {
      lc.clearDisplay(0);
      // Draw tail of the spiral
      for (int j = 0; j <= i && j < 64; j++) {
        if (i - j < 8) {  // Only show last 8 positions
          setLed(coords[j][0], coords[j][1], true);
        }
      }
      delay(30);
      ArduinoOTA.handle();
    }
  }
  lc.clearDisplay(0);
}

// ANIMATION 2: Rain Effect - Random drops falling down
void animationRain(unsigned long duration) {
  unsigned long startTime = millis();
  byte screen[8] = {0};

  while (millis() - startTime < duration) {
    // Shift all rows down
    for (int row = 7; row > 0; row--) {
      screen[row] = screen[row - 1];
    }

    // Add random drops at top
    screen[0] = 0;
    for (int col = 0; col < 8; col++) {
      if (random(0, 3) == 0) {
        screen[0] |= (1 << col);
      }
    }

    // Update display
    for (int row = 0; row < 8; row++) {
      setRow(row, screen[row]);
    }

    delay(80);
    ArduinoOTA.handle();
  }
  lc.clearDisplay(0);
}

// ANIMATION 3: Wave Effect - Sine wave moving across display
void animationWave(unsigned long duration) {
  unsigned long startTime = millis();

  while (millis() - startTime < duration) {
    for (int offset = 0; offset < 8; offset++) {
      lc.clearDisplay(0);
      for (int col = 0; col < 8; col++) {
        int height = 4 + 3 * sin((col + offset) * 0.8);
        if (height >= 0 && height < 8) {
          for (int row = 0; row <= height; row++) {
            setLed(7 - row, col, true);
          }
        }
      }
      delay(100);
      ArduinoOTA.handle();
    }
  }
  lc.clearDisplay(0);
}

// ANIMATION 4: Bouncing Ball - Ball bounces around the display
void animationBouncingBall(unsigned long duration) {
  unsigned long startTime = millis();
  float x = 4, y = 4;
  float vx = 0.8, vy = 0.6;

  while (millis() - startTime < duration) {
    lc.clearDisplay(0);

    // Update position
    x += vx;
    y += vy;

    // Bounce off walls
    if (x < 0 || x > 7) {
      vx = -vx;
      x = constrain(x, 0, 7);
    }
    if (y < 0 || y > 7) {
      vy = -vy;
      y = constrain(y, 0, 7);
    }

    // Draw ball with trailing effect
    setLed((int)x, (int)y, true);
    setLed((int)(x - vx), (int)(y - vy), true);

    delay(50);
    ArduinoOTA.handle();
  }
  lc.clearDisplay(0);
}

// ANIMATION 5: Rotating Pattern - Blocks rotating
void animationRotate(unsigned long duration) {
  unsigned long startTime = millis();

  byte patterns[4][8] = {
    {B11110000, B11110000, B00000000, B00000000, B00000000, B00000000, B00001111, B00001111},
    {B11000000, B11100000, B01110000, B00111000, B00011100, B00001110, B00000111, B00000011},
    {B00001111, B00001111, B00000000, B00000000, B00000000, B00000000, B11110000, B11110000},
    {B00000011, B00000111, B00001110, B00011100, B00111000, B01110000, B11100000, B11000000}
  };

  int patternIndex = 0;
  while (millis() - startTime < duration) {
    for (int row = 0; row < 8; row++) {
      setRow(row, patterns[patternIndex][row]);
    }
    patternIndex = (patternIndex + 1) % 4;
    delay(150);
    ArduinoOTA.handle();
  }
  lc.clearDisplay(0);
}

// ANIMATION 6: Matrix Rain - Matrix-style falling code
void animationMatrix(unsigned long duration) {
  unsigned long startTime = millis();
  byte screen[8] = {0};

  while (millis() - startTime < duration) {
    // Shift all rows down
    for (int row = 7; row > 0; row--) {
      screen[row] = screen[row - 1];
    }

    // Add sparse random drops at top
    screen[0] = 0;
    for (int col = 0; col < 8; col++) {
      if (random(0, 10) == 0) {
        screen[0] |= (1 << col);
      }
    }

    // Update display
    for (int row = 0; row < 8; row++) {
      setRow(row, screen[row]);
    }

    delay(60);
    ArduinoOTA.handle();
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
    case 0: animationSpiral(duration); break;
    case 1: animationRain(duration); break;
    case 2: animationWave(duration); break;
    case 3: animationBouncingBall(duration); break;
    case 4: animationRotate(duration); break;
    case 5: animationMatrix(duration); break;
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