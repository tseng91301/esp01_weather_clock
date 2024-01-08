#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h>

String ssid;
String password;

#define ledin LED_BUILTIN
#define rstread 3

const int resetDuration = 3; // 按下RST鍵3秒後重設WiFi
unsigned long resetTime = 0;
bool resetFlag = false;
bool firstsetup = false;

int connect_suc=0;
ESP8266WebServer server(80);

//连接WIFI
void doWiFiTick();
void startSTA();


/*
   初始化wifi连接
*/
void startSTA() {
  WiFi.disconnect();
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
}

void doWiFiTick() {
  static bool startSTAFlag = true;
  static bool taskStarted = false;
  static uint32_t lastWiFiCheckTick = 0;

  if (!startSTAFlag) {
    startSTAFlag = true;
    startSTA();
  }

  //未连接1s重连
  if ( WiFi.status() != WL_CONNECTED ) {
    if (millis() - lastWiFiCheckTick > 1000) {
      lastWiFiCheckTick = millis();
    }
  }
  //连接成功建立
  else {
    if (taskStarted == false) {
      taskStarted = true;
    }
  }
}

void handleRoot() {
  String html = "<meta charset='utf-8'><html><body>";
  html += "<h1>WiFi設定</h1>";
  html += "<form method='POST' action='/save'>";
  html += "WiFi名稱： <input type='text' name='ssid'><br>";
  html += "WiFi密碼： <input type='password' name='password'><br>";
  html += "<input type='submit' value='Save'>";
  html += "</form>";
  server.send(200, "text/html", html);
}

void handleSave() {
  String ssid = server.arg("ssid");
  String password = server.arg("password");
  
  File file = SPIFFS.open("/wifi_ssid.txt", "w");
  if (file) {
    file.print(ssid);
    file.close();
  }
  File file2 = SPIFFS.open("/wifi_pass.txt", "w");
  if (file2) {
    file2.print(password);
    file2.close();
  }
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(ssid, password);
  int timeout=0;
  int failedtoconnect=0;
  while (WiFi.status() != WL_CONNECTED) {
    if(timeout>=50){
      setup_wifi();
      failedtoconnect=1;
      break;
    }
    led_blink(200);
    timeout++;
  }
  if(!failedtoconnect){
    String html="<meta charset='utf-8'><html>設定成功！<br/>IP位址： <a href='http://";
    html+=WiFi.localIP().toString();
    html+="' target='_blank'>";
    html+=WiFi.localIP().toString();
    html+="</a>";
    html+="<br/>請截圖妥善保存，以免找不到IP位址";
    server.send(200, "text/html", html);
    delay(1000);
    ESP.restart();
  }
  server.send(200, "text/plain", "Failed!");
  ESP.restart();
}

void handleip(){
  String html="<meta charset='utf-8'><html>";
  String ip=WiFi.localIP().toString();
  html +=ip;
  html+="</html>";
  server.send(200, "text/html", html);
}

void led_blink(int freq){
  digitalWrite(ledin,HIGH);
  delay(freq/2);
  digitalWrite(ledin,LOW);
  delay(freq/2);
}

void setup_wifi(){
  WiFi.mode(WIFI_AP);
  WiFi.softAP("Esp01 weather clock 001", "");
  server.begin();
  server.on("/", handleRoot);
  server.on("/save", handleSave);
  server.on("/ip",handleip);
}

void resetWifiCredentials() {
  SPIFFS.remove("/wifi_ssid.txt");
  SPIFFS.remove("/wifi_pass.txt");
  ESP.restart();
}

void setup() {
  SPIFFS.begin();
  pinMode(rstread,INPUT_PULLUP);
  pinMode(ledin,INPUT_PULLUP);
  File file = SPIFFS.open("/wifi_ssid.txt", "r");
  if (file) {
    WiFi.mode(WIFI_STA);
    
    ssid = file.readStringUntil('\n');
    file.close();
    File file2 = SPIFFS.open("/wifi_pass.txt", "r");
    if(file2){
      password = file2.readStringUntil('\n');
      file2.close();
    }

    WiFi.begin(ssid, password);
    int timeout=0;
    int failedtoconnect=0;
    while (WiFi.status() != WL_CONNECTED) {
      if(timeout>=50){
        setup_wifi();
        failedtoconnect=1;
        break;
      }
      led_blink(200);
      timeout++;
    }
    if(!failedtoconnect){
      digitalWrite(ledin,LOW);
      connect_suc=1;
    }
    
  } else {
    setup_wifi();
  }

  
}

void loop() {
  
  if(!connect_suc){
    server.handleClient();
    led_blink(1000);
  }else if(!preTCPConnected){
    led_blink(200);
    led_blink(1000);
  }

  unsigned long startResetTime = millis();
  while (digitalRead(rstread) == LOW) {
    if (millis() - startResetTime >= (resetDuration * 1000)) {
      digitalWrite(ledin,HIGH);
      resetFlag = true;
      break;
    }
    delay(10);
  }

  // If resetFlag is true, then reset WiFi credentials
  if (resetFlag) {
    resetWifiCredentials();
  }
  // Your relay control logic can be implemented here.

  if(connect_suc){
    doWiFiTick();
    doTCPClientTick();
    delay(1000);
  }

  
}
