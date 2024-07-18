#define BLYNK_TEMPLATE_ID "TMPL3GWFkcE5K"
#define BLYNK_TEMPLATE_NAME "Gaz sensor"
#define BLYNK_AUTH_TOKEN "wrMaLLf06u6tBvM9wJr7J7sJxb70bFoa"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <LiquidCrystal_I2C.h>
#include <HTTPClient.h>

char auth[] = BLYNK_AUTH_TOKEN;
int lcdColumns = 16;
int lcdRows = 2;
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);
char ssid[] = "MANI";
char pass[] = "1234567890";
BlynkTimer timer;
const int smokeA0 = 34;
int sensorThres = 50;
int smokeValue = 0;
bool buzzerEnabled = true; // Initially set to true

const char* serverURL = "http://192.168.172.9/mq135/test_data.php";

// Blynk virtual pin for turning off the buzzer
#define BUZZER_CONTROL_VPIN V1

void sendSensor() {
  Blynk.virtualWrite(V0, smokeValue);
  
  lcd.clear(); // Clear the LCD screen
  
  lcd.setCursor(0, 0);
  lcd.print("Smoke Value: ");
  lcd.print(smokeValue); // Print the smokeValue
  
  if (smokeValue > 100) {
    Blynk.logEvent("gaz_detected", "Gas detected");
    lcd.setCursor(0, 1);
    lcd.print("Gas Detected:");
    if (buzzerEnabled) {
      digitalWrite(23, LOW); // Turn on buzzer
    }
    digitalWrite(18, HIGH); // Turn on LED
  } else {
    lcd.setCursor(0, 1);
    lcd.print("No danger:");
    digitalWrite(23, HIGH); // Turn off buzzer
    digitalWrite(18, LOW);  // Turn off LED
  }
}

// This function will be called every time the value of V1 changes
BLYNK_WRITE(BUZZER_CONTROL_VPIN) {
  String value = param.asStr();
  if (value == "OFF") {
    buzzerEnabled = false;
    digitalWrite(23, HIGH); // Ensure the buzzer is turned off
    Serial.println("Buzzer turned OFF");
  } else if (value == "ON") {
    buzzerEnabled = true;
    Serial.println("Buzzer enabled");
  }
}

void setup() {
  lcd.init();
  lcd.backlight();
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);
  pinMode(smokeA0, INPUT);
  pinMode(23, OUTPUT); // Buzzer
  pinMode(18, OUTPUT); // LED
  timer.setInterval(2500L, sendSensor);
  
  connectWiFi();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }

  smokeValue = analogRead(smokeA0);
  sendDataToServer(smokeValue);

  Blynk.run();
  timer.run();
}

void sendDataToServer(int value) {
  String postData = "smokeValue=" + String(value);
  
  HTTPClient http;
  http.begin(serverURL);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  
  int httpCode = http.POST(postData);
  String payload = http.getString();

  Serial.print("URL : "); Serial.println(serverURL); 
  Serial.print("Data: "); Serial.println(postData);
  Serial.print("httpCode: "); Serial.println(httpCode);
  Serial.print("payload : "); Serial.println(payload);
  Serial.println("--------------------------------------------------");
}

void connectWiFi() {
  WiFi.mode(WIFI_OFF);
  delay(1000);
  WiFi.mode(WIFI_STA);
  
  WiFi.begin(ssid, pass);
  Serial.println("Connecting to WiFi");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
    
  Serial.print("Connected to: "); Serial.println(ssid);
  Serial.print("IP address: "); Serial.println(WiFi.localIP());
}