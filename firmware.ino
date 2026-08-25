// 1. Force the ESP32 platform architecture macro required by ESP32-A2DP
#ifndef ARDUINO_ARCH_ESP32
#define ARDUINO_ARCH_ESP32
#endif

#ifndef CONFIG_IDF_TARGET_ESP32S3
#define CONFIG_IDF_TARGET_ESP32S3
#endif

// 2. Include core Espressif & Arduino headers FIRST
#include <Arduino.h>
#include <sdkconfig.h>

// 3. Include Audio & A2DP Libraries
#include <BluetoothA2DPSink.h>
#include <AudioTools.h>

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

// Debounce Timing Constants (Renamed to avoid conflict with AudioTools)
const unsigned long BUTTON_DEBOUNCE_MS = 50; 

// Button State Tracking Struct
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
  i2s_pin_config_t my_pin_config = {
      .bck_io_num = I2S_BCK_PIN,
      .ws_io_num = I2S_LRCK_PIN,
      .data_out_num = I2S_DATA_PIN,
      .data_in_num = I2S_PIN_NO_CHANGE
  };
  
  a2dp_sink.set_pin_config(my_pin_config);

  // Start Bluetooth A2DP Audio Receiver Service
  a2dp_sink.start("Hidden Headphones");

  Serial.println("Bluetooth A2DP Sink ready. Connect via Bluetooth!");
}

// ==========================================
// MAIN LOOP
// ==========================================
void loop() {
  handleButton(btnVolUp, onVolumeUp);
  handleButton(btnVolDown, onVolumeDown);
  delay(10); // Yield to prevent watchdog triggers
}

// ==========================================
// BUTTON HANDLING & CALLBACKS
// ==========================================
void handleButton(Button &btn, void (*onPress)()) {
  bool reading = digitalRead(btn.pin);

  if (reading != btn.lastState) {
    btn.lastDebounceTime = millis();
  }

  if ((millis() - btn.lastDebounceTime) > BUTTON_DEBOUNCE_MS) {
    if (reading != btn.currentState) {
      btn.currentState = reading;

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