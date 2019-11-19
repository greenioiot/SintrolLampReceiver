
#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

#include <esp_now.h>
#include <WiFi.h>
#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;
#define CHANNEL 1
int r1 = 12; // ควบคุม Relay ตัวที่ 1 ต่อขา7
int r2 = 13; // ควบคุม Relay ตัวที่ 2 ต่อขา6
int r3 = 15; // ควบคุม Relay ตัวที่ 3 ต่อขา5
int r4 = 2; // ควบคุม Relay ตัวที่ 4 ต่อขา4
// Init ESP Now with fallback
void InitESPNow() {
  WiFi.disconnect();
  if (esp_now_init() == ESP_OK) {
    Serial.println("ESPNow Init Success");
  }
  else {
    Serial.println("ESPNow Init Failed");
    // Retry InitESPNow, add a counte and then restart?
    // InitESPNow();
    // or Simply Restart
    ESP.restart();
  }
}

// config AP SSID
void configDeviceAP() {
  const char *SSID = "GreenIO";
  bool result = WiFi.softAP(SSID, "Slave_1_Password", CHANNEL, 0);
  if (!result) {
    Serial.println("AP Config failed.");
  } else {
    Serial.println("AP Config Success. Broadcasting with AP: " + String(SSID));
  }
}

void setup() {
  Serial.begin(115200);
  SerialBT.begin("SintrolLamp"); //Bluetooth device name

  Serial.println("Sintrol/Lamp/Slave");
  //Set device in AP mode to begin with
  WiFi.mode(WIFI_AP);
  // configure device AP mode
  configDeviceAP();
  // This is the mac address of the Slave in AP Mode
  Serial.print("AP MAC: "); Serial.println(WiFi.softAPmacAddress());
  // Init ESPNow with a fallback logic
  InitESPNow();
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info.
  esp_now_register_recv_cb(OnDataRecv);

  pinMode(r1, OUTPUT);
  pinMode(r2, OUTPUT);
  pinMode(r3, OUTPUT);
  pinMode(r4, OUTPUT);
  digitalWrite(r1, 0);
  digitalWrite(r2, 0);
  digitalWrite(r3, 0);
  digitalWrite(r4, 0);
  greenOn();
}

void checkData(unsigned char level) {
  if (level == 1 ) {
    orangeOff();
    redOff();
    greenOn();
  } else if (level == 2 ) {
    greenOff();
    redOff();
    orangeOn();
  } else if (level == 3) {
    greenOff();
    orangeOff();
    soundOff();
    redOn();
  }
}
// callback when data is recv from Master
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print("Last Packet Recv from: "); Serial.println(macStr);
  Serial.print("Last Packet Recv Data: "); Serial.println(*data);
  int level = *data;
  checkData(level);
  Serial.println("");
}

void greenOff() {
  Serial.println("Green OFF");
  digitalWrite(r1, 1);
}

void greenOn() {
  Serial.println("Green ON");
  digitalWrite(r1, 0);
}

void orangeOn() {
  Serial.println("Orange ON");
  digitalWrite(r2, 1);
}
void orangeOff() {
  Serial.println("Orange OFF");
  digitalWrite(r2, 0);
}

void redOn() {
  Serial.println("Red ON");
  digitalWrite(r3, 1);
}
void redOff() {
  Serial.println("Red OFF");
  digitalWrite(r3, 0);
}

void soundOn() {
  Serial.println("Sound ON");
  digitalWrite(r4, 1);
}
void soundOff() {
  Serial.println("Sound OFF");
  digitalWrite(r4, 0);
}

void loop() {
  // Chill
}
