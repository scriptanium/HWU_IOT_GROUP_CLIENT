#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <espnow.h>

// https://randomnerdtutorials.com/esp-now-many-to-one-esp32/

#define BUT_PIN 5

#define BOARD_ID 0

u8 bridge_address[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

struct payload {
  u8 id;
};

struct payload data;

int state = 0;
bool change = false;

void check_data_send(u8 *mac_addr, u8 status) {
  if (status == 0) {
    Serial.println("Delivery success");
  } else {
    Serial.println("Delivery Failed");
  }
}

void setup() {
  pinMode(BUT_PIN, INPUT);

  Serial.begin(115200);
  Serial.setTimeout(2000);

  WiFi.mode(WIFI_STA);

  while (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    delay(500);
  }

  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  esp_now_register_send_cb(check_data_send);
  esp_now_add_peer(bridge_address, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
  data.id = BOARD_ID;
  delay(1000);
  Serial.println("Board is ready");
}

void loop() {
  int esp_status;
  state = digitalRead(BUT_PIN);
  if (state == HIGH && change == false) {
    change = true;
    for (u8 i = 0; i < 10; ++i) {
      esp_status = esp_now_send(bridge_address, (u8 *)&data, sizeof(data));
      if (esp_status == 0) {
        break;
      }
      delay(500);
    }
    if (esp_status != 0) {
      Serial.println("Failed to send the data to the bridge");
    }
    delayMicroseconds(50000);
  } else {
    ESP.deepSleep(5000000); // 5 SEC BEFORE next detection
  }
  change = (state == HIGH);
}
