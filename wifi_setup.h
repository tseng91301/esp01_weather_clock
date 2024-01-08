ESP8266WebServer server(80);

String ssid;
String password;
int connect_suc=0;

//连接WIFI
void setup_wifi();
void wifi_try();
void doWiFiTick();
void startSTA();

//TCP初始化连接
void doTCPClientTick();
void startTCPClient();

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
            wifi_try();
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
  html += "</body></html>";
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
    String connectingtxt="Connecting to WiFi";
    if(timeout>=50){
      setup_wifi();
      failedtoconnect=1;
      break;
    }
    for(int a=0;a<timeout%4;a++){
      connectingtxt+=".";
    }
    oled1.txt(connectingtxt);
    oled1.show();
    delay(400);
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
    oled1.txt("Connected! Rebooting...");
    oled1.show();
    delay(1000);
    ESP.restart();
  }
  server.send(200, "text/plain", "Failed!");
  setup_wifi();
}

void handleip(){
    String html="<meta charset='utf-8'><html>";
    String ip=WiFi.localIP().toString();
    html +=ip;
    html+="</html>";
    server.send(200, "text/html", html);
}

void setup_wifi(){
    oled1.txt("Please setup WiFi!");
    oled1.show();
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

void wifi_try(){
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
            String connectingtxt="Connecting to WiFi";
            if(timeout>=25){
                setup_wifi();
                failedtoconnect=1;
                break;
            }
            for(int a=0;a<timeout%4;a++){
              connectingtxt+=".";
            }
            oled1.txt(connectingtxt);
            oled1.show();
            delay(400);
            timeout++;
        }
        if(!failedtoconnect){
            oled1.txt("WiFi connected!\nIP: "+WiFi.localIP().toString());
            oled1.show();
            connect_suc=1;
        }
        
    } else {
        setup_wifi();
    }
}
