#include <Arduino.h>
#include "AudioTools.h"
#include "BluetoothA2DPSink.h"

// ==========================================
// PIN DEFINITIONS
// ==========================================

// I2S Audio Pins for PCM5102A DAC
#define I2S_LRCK_PIN   21    // Word Select / LRCK
#define I2S_BCK_PIN    26    // Bit Clock / BCK
#define I2S_DATA_PIN   1     // Serial Data Out / DIN

// Button GPIO Pins (Active LOW - Connected to GND when pressed)
#define BTN_VOL_UP     0     // SW1: Volume Up
#define BTN_VOL_DOWN   1     // SW2: Volume Down / Multi-function

// Debounce Timing Constants
const unsigned long DEBOUNCE_DELAY = 50; // milliseconds

// Button State Tracking
struct Button {
  uint8_t pin;
  bool lastState;
  bool currentState;
  unsigned long lastDebounceTime;
};

Button btnVolUp   = {BTN_VOL_UP, HIGH, HIGH, 0};
Button btnVolDown = {BTN_VOL_DOWN, HIGH, HIGH, 0};

// A2DP Sink Object
BluetoothA2DPSink a2dp_sink;

// ==========================================
// FUNCTION DECLARATIONS
// ==========================================
void handleButton(Button &btn, void (*onPress)());
void onVolumeUp();
void onVolumeDown();

// ==========================================
// SETUP & INITIALIZATION
// ==========================================
void setup() {
  Serial.begin(115200);
  Serial.println("Starting Hidden Bone Conduction Headphone Firmware...");

  // Initialize Button GPIOs with Internal Pull-Ups
  pinMode(btnVolUp.pin, INPUT_PULLUP);
  pinMode(btnVolDown.pin, INPUT_PULLUP);

  // Configure I2S Hardware Pins for PCM5102A DAC
  i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_BCK_PIN,
    .ws_io_num = I2S_LRCK_PIN,
    .data_out_num = I2S_DATA_PIN,
    .data_in_num = I2S_PIN_NO_CHANGE
  };
  a2dp_sink.set_pin_config(pin_config);

  // Start Bluetooth A2DP Audio Receiver Service
  // Device name visible in Bluetooth settings:
  a2dp_sink.start("Hidden Headphones");

  Serial.println("Bluetooth A2DP Sink ready. Connect via Bluetooth!");
}

// ==========================================
// MAIN LOOP
// ==========================================
void loop() {
  // Poll & Debounce Controls
  handleButton(btnVolUp, onVolumeUp);
  handleButton(btnVolDown, onVolumeDown);

  delay(10); // Yield to prevent watchdog triggers
}

// ==========================================
// BUTTON HANDLING & CALLBACKS
// ==========================================
void handleButton(Button &btn, void (*onPress)()) {
  bool reading = digitalRead(btn.pin);

  // Reset debounce timer if state changed due to noise
  if (reading != btn.lastState) {
    btn.lastDebounceTime = millis();
  }

  if ((millis() - btn.lastDebounceTime) > DEBOUNCE_DELAY) {
    // If the button state has stably changed
    if (reading != btn.currentState) {
      btn.currentState = reading;

      // Trigger callback on press down (LOW because of INPUT_PULLUP)
      if (btn.currentState == LOW) {
        onPress();
      }
    }
  }

  btn.lastState = reading;
}

void onVolumeUp() {
  int currentVol = a2dp_sink.get_volume();
  int newVol = min(127, currentVol + 10);
  a2dp_sink.set_volume(newVol);
  Serial.printf("Volume UP: %d\n", newVol);
}

void onVolumeDown() {
  int currentVol = a2dp_sink.get_volume();
  int newVol = max(0, currentVol - 10);
  a2dp_sink.set_volume(newVol);
  Serial.printf("Volume DOWN: %d\n", newVol);
}