#include <Wire.h>
#include <M5Stack.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

char cont[1024];
const char* ca = \
R"(-----BEGIN CERTIFICATE-----
MIIDSjCCAjKgAwIBAgIQRK+wgNajJ7qJMDmGLvhAazANBgkqhkiG9w0BAQUFADA/
MSQwIgYDVQQKExtEaWdpdGFsIFNpZ25hdHVyZSBUcnVzdCBDby4xFzAVBgNVBAMT
DkRTVCBSb290IENBIFgzMB4XDTAwMDkzMDIxMTIxOVoXDTIxMDkzMDE0MDExNVow
PzEkMCIGA1UEChMbRGlnaXRhbCBTaWduYXR1cmUgVHJ1c3QgQ28uMRcwFQYDVQQD
Ew5EU1QgUm9vdCBDQSBYMzCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEB
AN+v6ZdQCINXtMxiZfaQguzH0yxrMMpb7NnDfcdAwRgUi+DoM3ZJKuM/IUmTrE4O
rz5Iy2Xu/NMhD2XSKtkyj4zl93ewEnu1lcCJo6m67XMuegwGMoOifooUMM0RoOEq
OLl5CjH9UL2AZd+3UWODyOKIYepLYYHsUmu5ouJLGiifSKOeDNoJjj4XLh7dIN9b
xiqKqy69cK3FCxolkHRyxXtqqzTWMIn/5WgTe1QLyNau7Fqckh49ZLOMxt+/yUFw
7BZy1SbsOFU5Q9D8/RhcQPGX69Wam40dutolucbY38EVAjqr2m7xPi71XAicPNaD
aeQQmxkqtilX4+U9m5/wAl0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNV
HQ8BAf8EBAMCAQYwHQYDVR0OBBYEFMSnsaR7LHH62+FLkHX/xBVghYkQMA0GCSqG
SIb3DQEBBQUAA4IBAQCjGiybFwBcqR7uKGY3Or+Dxz9LwwmglSBd49lZRNI+DT69
ikugdB/OEIKcdBodfpga3csTS7MgROSR6cz8faXbauX+5v3gTt23ADq1cEmv8uXr
AvHRAosZy5Q6XkjEGB5YGV8eAlrwDPGxrancWYaLbumR9YbK+rlmM6pZW87ipxZz
R8srzJmwN0jP41ZL9c8PDHIyh8bwRLtTcm1D9SZImlJnt1ir/md2cXjbDaJWFBM5
JDGFoqgCWjBH4d1QB7wCCZAA62RjYJsWvIjJEubSfZGL+T0yjWW06XyxV3bqxbYo
Ob8VZRzI9neWagqNdwvYkQsEjgfbKbYK7p2CNTUQ
-----END CERTIFICATE-----)";
Adafruit_BME280 bme;
WiFiClientSecure client;

void setup(){
  WiFi.begin("GUEST2", "00000000");
  Wire.begin();
  M5.begin();
  Serial.begin(9600);

  while (WiFi.status() != WL_CONNECTED) {
    M5.Lcd.print(".");
    delay(500);
  }
  M5.Lcd.println("wifi connected");

  bme.begin(0x76);

  // 接続するクライアントのルート証明書がないとそもそもハンドシェイクできない
  client.setCACert(ca);
}

void loop() {
  // 一度接続すれば何度もデータ送信できるが途中で切断されるときがあるためその場合再接続する
  if (!client.connected()) {
    M5.Lcd.fillScreen(TFT_RED);
    client.connect("api.daco.dev", 443);
  } else {
    M5.Lcd.fillScreen(TFT_GREEN);
    sprintf(cont, "{\"temp\":\"%f\",\"humid\":\"%f\",\"pressure\":\"%f\"}", bme.readTemperature(), bme.readHumidity(), bme.readPressure());
    client.printf(
      "POST /t/write.php HTTP/1.1\n"
      "Host: api.daco.dev\n"
      "Cache-Control: no-cache\n"
      "Content-Type: application/JSON\n"
      "Content-Length: %d\n"
      "\n"
    , strlen(cont));
    client.print(cont);
    // ヘッダーの受信をまたずに再送信をすると途中で送信できなくなるためヘッダーが受信できるのを待つ
    while (client.connected()) {
      String line = client.readStringUntil('\n');
      if (line == "\r") {
        Serial.println("headers received");
        // 上記までの処理で約50ms程度かかるため950ms待って1秒周期でデータを送る
        delay(950);
        break;
      }
    }
  }
}
