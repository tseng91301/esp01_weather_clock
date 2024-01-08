#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char* ssid = "2F_BACK";
const char* password = "0932576037";
const char* host = "192.168.0.52"; // 目标主机的域名或IP地址
const int port = 5625; // 您想要使用的端口
const char* parameter1 = "09";

void setup() {
  Serial.begin(115200);

  // 连接到Wi-Fi网络
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;

    // 构建带参数的URL
    String url = "http://" + String(host) + ":" + String(port) + "/dealweatherserver.php";
    url += "?hr=" + String(parameter1);
    
    // 开始GET请求
    http.begin(client,url);

    // 发起请求并获取响应
    int httpCode = http.GET();
    if (httpCode > 0) {
      Serial.printf("HTTP code: %d\n", httpCode);
      if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        Serial.println("Response:");
        Serial.println(payload);
      }
    } else {
      Serial.println("HTTP request failed");
    }

    // 结束请求
    http.end();

    // 等待一段时间后重试
    delay(5000);
  }
}
