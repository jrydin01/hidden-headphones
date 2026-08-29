/*
 * Hidden Headphones - ESP32 Bluetooth A2DP firmware
 *
 * This sketch targets an original ESP32/WROOM-32 board. Unlike the ESP32-S3,
 * the original ESP32 supports Bluetooth Classic A2DP for phone audio.
 */

#include <Arduino.h>
#include <BluetoothA2DPSink.h>
#include <driver/i2s.h>

// I2S connection to the PCM5102A.
constexpr gpio_num_t I2S_BCK_PIN = GPIO_NUM_26;
constexpr gpio_num_t I2S_LRCK_PIN = GPIO_NUM_25;
constexpr gpio_num_t I2S_DATA_PIN = GPIO_NUM_22;

// Active-low controls from the diagram. Keep these separate from I2S pins.
constexpr gpio_num_t BTN_VOL_UP = GPIO_NUM_16;
constexpr gpio_num_t BTN_VOL_DOWN = GPIO_NUM_17;
constexpr gpio_num_t BTN_PLAY_PAUSE = GPIO_NUM_18;
constexpr gpio_num_t LED_STATUS = GPIO_NUM_19;

constexpr uint32_t BUTTON_DEBOUNCE_MS = 50;
constexpr int16_t VOLUME_STEP = 10;
constexpr int16_t MAX_VOLUME = 127;

BluetoothA2DPSink a2dpSink;

struct Button {
  gpio_num_t pin;
  bool lastReading;
  bool stableState;
  uint32_t lastChangeMs;
  void (*onPress)();
};

int16_t volume = 64;
bool playing = true;

void onVolumeUp();
void onVolumeDown();
void onPlayPause();

Button buttons[] = {
    {BTN_VOL_UP, HIGH, HIGH, 0, onVolumeUp},
    {BTN_VOL_DOWN, HIGH, HIGH, 0, onVolumeDown},
    {BTN_PLAY_PAUSE, HIGH, HIGH, 0, onPlayPause},
};

void configureI2S() {
  i2s_pin_config_t pins = {};
  pins.bck_io_num = I2S_BCK_PIN;
  pins.ws_io_num = I2S_LRCK_PIN;
  pins.data_out_num = I2S_DATA_PIN;
  pins.data_in_num = I2S_PIN_NO_CHANGE;
  a2dpSink.set_pin_config(pins);
}

void handleButton(Button &button) {
  const bool reading = digitalRead(button.pin);
  const uint32_t now = millis();

  if (reading != button.lastReading) {
    button.lastChangeMs = now;
    button.lastReading = reading;
  }

  if (now - button.lastChangeMs >= BUTTON_DEBOUNCE_MS && reading != button.stableState) {
    button.stableState = reading;
    if (button.stableState == LOW) {
      button.onPress();
    }
  }
}

void onVolumeUp() {
  volume = min<int16_t>(MAX_VOLUME, a2dpSink.get_volume() + VOLUME_STEP);
  a2dpSink.set_volume(volume);
  Serial.printf("Volume UP: %d/127\n", volume);
}

void onVolumeDown() {
  volume = max<int16_t>(0, a2dpSink.get_volume() - VOLUME_STEP);
  a2dpSink.set_volume(volume);
  Serial.printf("Volume DOWN: %d/127\n", volume);
}

void onPlayPause() {
  playing = !playing;
  if (playing) {
    a2dpSink.play();
  } else {
    a2dpSink.pause();
  }
  digitalWrite(LED_STATUS, playing ? HIGH : LOW);
  Serial.printf("Playback %s\n", playing ? "started" : "paused");
}

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println("Starting Hidden Headphones ESP32 Bluetooth A2DP firmware");

  pinMode(LED_STATUS, OUTPUT);
  digitalWrite(LED_STATUS, HIGH);
  for (Button &button : buttons) {
    pinMode(button.pin, INPUT_PULLUP);
  }

  configureI2S();
  a2dpSink.start("Hidden Headphones");
  a2dpSink.set_volume(volume);
  Serial.println("Bluetooth A2DP sink and PCM5102A I2S output ready");
}

void loop() {
  for (Button &button : buttons) {
    handleButton(button);
  }
}