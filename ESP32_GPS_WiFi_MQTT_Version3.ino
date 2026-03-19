#include <WiFi.h>
#include <PubSubClient.h>
#include <TinyGPSPlus.h>
#include <HardwareSerial.h>

// ================= CONFIG =================
static const int RXPin = 16, TXPin = 17;
static const uint32_t GPSBaud = 9600;

const char* ssid     = "TOPNET_C228";
const char* password = "MCHLIN9HAL7NKK";

const char* mqtt_server = "192.168.1.13";
const int   mqtt_port   = 1883;

const char* esp_id = "Na7la";

// ================= OBJECTS =================
TinyGPSPlus gps;
HardwareSerial gpsSerial(1);

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lastPublishTime = 0;
const unsigned long publishInterval = 5000; // Publish every 5 seconds

// ================= MQTT CALLBACK ==========
void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.print("Topic: ");
  Serial.println(topic);
  Serial.print("Message: ");
  Serial.println(message);
}

// ================= WIFI ===================
void setup_wifi() {
  WiFi.begin(ssid, password);
  Serial.print("Connexion WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ WiFi connecté");
  Serial.println("IP: " + WiFi.localIP().toString());
}

// ================= MQTT ===================
void reconnect() {
  while (!client.connected()) {
    Serial.print("Connexion MQTT...");
    if (client.connect(esp_id)) {
      Serial.println("✅ connecté");
    } else {
      Serial.print("❌ échec rc=");
      Serial.println(client.state());
      delay(3000);
    }
  }
}

// ================= SETUP ==================
void setup()
{
  Serial.begin(115200);
  delay(100);
  
  Serial.println("\n\n========== GPS TRACKER WITH MQTT ==========");
  
  // Initialize GPS on UART1 with custom pins
  gpsSerial.begin(GPSBaud, SERIAL_8N1, RXPin, TXPin);
  Serial.println("GPS initialisé sur UART1");

  Serial.print("TinyGPSPlus Library v. "); 
  Serial.println(TinyGPSPlus::libraryVersion());
  Serial.println();

  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  reconnect();
  
  Serial.println("=========================================\n");
}

// ================= LOOP ===================
void loop()
{
  // Feed GPS parser
  while (gpsSerial.available())
    gps.encode(gpsSerial.read());

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long now = millis();
  if (now - lastPublishTime >= publishInterval) {
    lastPublishTime = now;

    static const double LONDON_LAT = 51.508131, LONDON_LON = -0.128002;

    // Only display and publish if we have a valid fix
    if (gps.location.isValid()) {
      Serial.print("Sats: ");
      printInt(gps.satellites.value(), gps.satellites.isValid(), 5);
      
      Serial.print("HDOP: ");
      printFloat(gps.hdop.hdop(), gps.hdop.isValid(), 6, 1);
      
      Serial.print("LAT: ");
      printFloat(gps.location.lat(), gps.location.isValid(), 11, 6);
      
      Serial.print("LNG: ");
      printFloat(gps.location.lng(), gps.location.isValid(), 12, 6);
      
      Serial.print("Age: ");
      printInt(gps.location.age(), gps.location.isValid(), 5);
      
      Serial.print("Date/Time: ");
      printDateTime(gps.date, gps.time);
      
      Serial.print("Alt: ");
      printFloat(gps.altitude.meters(), gps.altitude.isValid(), 7, 2);
      
      Serial.print("Course: ");
      printFloat(gps.course.deg(), gps.course.isValid(), 7, 2);
      
      Serial.print("Speed: ");
      printFloat(gps.speed.kmph(), gps.speed.isValid(), 6, 2);
      
      Serial.print("Card: ");
      printStr(gps.course.isValid() ? TinyGPSPlus::cardinal(gps.course.deg()) : "*** ", 6);

      unsigned long distanceKmToLondon =
        (unsigned long)TinyGPSPlus::distanceBetween(
          gps.location.lat(),
          gps.location.lng(),
          LONDON_LAT, 
          LONDON_LON) / 1000;
      
      Serial.print("Dist to London: ");
      printInt(distanceKmToLondon, gps.location.isValid(), 9);

      double courseToLondon =
        TinyGPSPlus::courseTo(
          gps.location.lat(),
          gps.location.lng(),
          LONDON_LAT, 
          LONDON_LON);

      Serial.print("Course to London: ");
      printFloat(courseToLondon, gps.location.isValid(), 7, 2);

      const char *cardinalToLondon = TinyGPSPlus::cardinal(courseToLondon);
      Serial.print("Cardinal: ");
      printStr(gps.location.isValid() ? cardinalToLondon : "*** ", 6);

      Serial.print("Chars: ");
      printInt(gps.charsProcessed(), true, 6);
      
      Serial.print("Sentences: ");
      printInt(gps.sentencesWithFix(), true, 10);
      
      Serial.print("Checksum: ");
      printInt(gps.failedChecksum(), true, 9);
      
      Serial.println();

      // ========== PUBLISH TO MQTT ==========
      String latTopic = String(esp_id) + "/gps/latitude";
      client.publish(latTopic.c_str(), String(gps.location.lat(), 6).c_str());

      String lngTopic = String(esp_id) + "/gps/longitude";
      client.publish(lngTopic.c_str(), String(gps.location.lng(), 6).c_str());

      String altTopic = String(esp_id) + "/gps/altitude";
      client.publish(altTopic.c_str(), String(gps.altitude.meters(), 2).c_str());

      String satTopic = String(esp_id) + "/gps/satellites";
      client.publish(satTopic.c_str(), String(gps.satellites.value()).c_str());

      String hdopTopic = String(esp_id) + "/gps/hdop";
      client.publish(hdopTopic.c_str(), String(gps.hdop.hdop(), 1).c_str());

      String speedTopic = String(esp_id) + "/gps/speed";
      client.publish(speedTopic.c_str(), String(gps.speed.kmph(), 2).c_str());

      String courseTopic = String(esp_id) + "/gps/course";
      client.publish(courseTopic.c_str(), String(gps.course.deg(), 2).c_str());

    } else {
      Serial.print("⏳ Waiting for GPS fix... Sats: ");
      Serial.println(gps.satellites.value());
    }
  }
}

// ================= HELPER FUNCTIONS ==========
static void printFloat(float val, bool valid, int len, int prec)
{
  if (!valid)
  {
    while (len-- > 1)
      Serial.print('*');
    Serial.print(' ');
  }
  else
  {
    Serial.print(val, prec);
    int vi = abs((int)val);
    int flen = prec + (val < 0.0 ? 2 : 1);
    flen += vi >= 1000 ? 4 : vi >= 100 ? 3 : vi >= 10 ? 2 : 1;
    for (int i=flen; i<len; ++i)
      Serial.print(' ');
  }
}

static void printInt(unsigned long val, bool valid, int len)
{
  char sz[32] = "*****************";
  if (valid)
    sprintf(sz, "%ld", val);
  sz[len] = 0;
  for (int i=strlen(sz); i<len; ++i)
    sz[i] = ' ';
  if (len > 0) 
    sz[len-1] = ' ';
  Serial.print(sz);
}

static void printDateTime(TinyGPSDate &d, TinyGPSTime &t)
{
  if (!d.isValid())
  {
    Serial.print("********** ");
  }
  else
  {
    char sz[32];
    sprintf(sz, "%02d/%02d/%02d ", d.month(), d.day(), d.year());
    Serial.print(sz);
  }
  
  if (!t.isValid())
  {
    Serial.print("******** ");
  }
  else
  {
    char sz[32];
    sprintf(sz, "%02d:%02d:%02d ", t.hour(), t.minute(), t.second());
    Serial.print(sz);
  }

  printInt(d.age(), d.isValid(), 5);
}

static void printStr(const char *str, int len)
{
  int slen = strlen(str);
  for (int i=0; i<len; ++i)
    Serial.print(i<slen ? str[i] : ' ');
}