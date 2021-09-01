#include "DHT.h"

//FirebaseESP8266.h must be included before ESP8266WiFi.h
#include "FirebaseESP8266.h"
#include <ESP8266WiFi.h>

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define FIREBASE_HOST "esp8266-699c6.firebaseio.com/" //Without http:// or https:// schemes
#define FIREBASE_AUTH "68DPC4k8GYe4Pm1MwcdqwZQiqZsVJ83wz6ssjXXF"
#define WIFI_SSID "TNK@COVER"
#define WIFI_PASSWORD "Intanin263"

#define btn1 0
#define btn2 2
#define btn3 14
#define relay1 12
//pin D7 - 13 is Temperature sensor

FirebaseData firebaseData;
FirebaseJson json;
void printResult(FirebaseData &data);

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x3F, 20, 4);

DHT dht;

void setup()
{
  // initialize the LCD
  lcd.begin();
  // Turn on the blacklight and print a message.
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Connecting WiFi...");

  pinMode(btn1, INPUT);
  pinMode(btn2, INPUT);
  pinMode(btn3, INPUT);
  pinMode(relay1, OUTPUT);

  Serial.begin(9600);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

  //Set the size of WiFi rx/tx buffers in the case where we want to work with large data.
  firebaseData.setBSSLBufferSize(1024, 1024);

  //Set the size of HTTP response buffers in the case where we want to work with large data.
  firebaseData.setResponseSize(1024);

  //Set database read timeout to 1 minute (max 15 minutes)
  Firebase.setReadTimeout(firebaseData, 1000 * 60);
  //tiny, small, medium, large and unlimited.
  //Size and its write timeout e.g. tiny (1s), small (10s), medium (30s) and large (60s).
  Firebase.setwriteSizeLimit(firebaseData, "tiny");

  Serial.println();
  Serial.println("Status\tHumidity (%)\tTemperature (C)\t(F)");

  dht.setup(13);

  lcd.clear();
}

void printResult(FirebaseData &data)
{

  if (data.dataType() == "int")
    Serial.println(data.intData());
  else if (data.dataType() == "float")
    Serial.println(data.floatData(), 5);
  else if (data.dataType() == "double")
    printf("%.9lf\n", data.doubleData());
  else if (data.dataType() == "boolean")
    Serial.println(data.boolData() == 1 ? "true" : "false");
  else if (data.dataType() == "string")
    Serial.println(data.stringData());
  else if (data.dataType() == "json")
  {
    Serial.println();
    FirebaseJson &json = data.jsonObject();
    //Print all object data
    Serial.println("Pretty printed JSON data:");
    String jsonStr;
    json.toString(jsonStr, true);
    Serial.println(jsonStr);
    Serial.println();
    Serial.println("Iterate JSON data:");
    Serial.println();
    size_t len = json.iteratorBegin();
    String key, value = "";
    int type = 0;
    for (size_t i = 0; i < len; i++)
    {
      json.iteratorGet(i, type, key, value);
      Serial.print(i);
      Serial.print(", ");
      Serial.print("Type: ");
      Serial.print(type == FirebaseJson::JSON_OBJECT ? "object" : "array");
      if (type == FirebaseJson::JSON_OBJECT)
      {
        Serial.print(", Key: ");
        Serial.print(key);
      }
      Serial.print(", Value: ");
      Serial.println(value);
    }
    json.iteratorEnd();
  }
  else if (data.dataType() == "array")
  {
    Serial.println();
    //get array data from FirebaseData using FirebaseJsonArray object
    FirebaseJsonArray &arr = data.jsonArray();
    //Print all array values
    Serial.println("Pretty printed Array:");
    String arrStr;
    arr.toString(arrStr, true);
    Serial.println(arrStr);
    Serial.println();
    Serial.println("Iterate array values:");
    Serial.println();
    for (size_t i = 0; i < arr.size(); i++)
    {
      Serial.print(i);
      Serial.print(", Value: ");

      FirebaseJsonData &jsonData = data.jsonData();
      //Get the result data from FirebaseJsonArray object
      arr.get(jsonData, i);
      if (jsonData.typeNum == FirebaseJson::JSON_BOOL)
        Serial.println(jsonData.boolValue ? "true" : "false");
      else if (jsonData.typeNum == FirebaseJson::JSON_INT)
        Serial.println(jsonData.intValue);
      else if (jsonData.typeNum == FirebaseJson::JSON_FLOAT)
        Serial.println(jsonData.floatValue);
      else if (jsonData.typeNum == FirebaseJson::JSON_DOUBLE)
        printf("%.9lf\n", jsonData.doubleValue);
      else if (jsonData.typeNum == FirebaseJson::JSON_STRING ||
               jsonData.typeNum == FirebaseJson::JSON_NULL ||
               jsonData.typeNum == FirebaseJson::JSON_OBJECT ||
               jsonData.typeNum == FirebaseJson::JSON_ARRAY)
        Serial.println(jsonData.stringValue);
    }
  }
  else
  {
    Serial.println(data.payload());
  }
}

int flag;
int temp_set;
int temp_set_mode;
int pump;

void loop()
{
  delay(dht.getMinimumSamplingPeriod());

  float humidity = dht.getHumidity();
  float temperature = dht.getTemperature();

  Serial.print(dht.getStatusString());
  Serial.print("\t");
  Serial.print(humidity, 1);
  Serial.print("\t\t");
  Serial.print(temperature, 1);
  Serial.print("\t\t");
  Serial.println(dht.toFahrenheit(temperature), 1);

  get_mode();




  if ( flag == 0 ) {
    lcd.setCursor(0, 0);
    lcd.print("WiFi Connected");
    flag++;
    Firebase.setInt(firebaseData, "/mode", flag);
    delay(2000);
  }
  if ( flag == 1) {
    //Get temp setting
    get_temp_set();

    //Set temp now
    Firebase.setInt(firebaseData, "/temp_now", temperature);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("(Mode1) Temperature");
    lcd.setCursor(0, 2);
    lcd.print("NOW : ");
    lcd.print(temperature);
    lcd.print("c");
    lcd.setCursor(0, 3);
    lcd.print("SET : ");
    lcd.print(temp_set);
    lcd.print("c");

    if ( temperature >= temp_set ) digitalWrite(relay1, LOW);
    else digitalWrite(relay1, HIGH);

    // button check
    if (digitalRead(btn1) == 0) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Next mode...");
      delay(2000);
      lcd.clear();
      flag = 3;
      Firebase.setInt(firebaseData, "/mode", flag);
    }
    if (digitalRead(btn2) == 0) {
      get_temp_set_mode();

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Setting mode1");
      delay(2000);
      lcd.clear();
      flag = 2;
    }
  }
  if ( flag == 2 ) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Temperature setting");
    lcd.setCursor(0, 2);
    lcd.print("Temp >= ");
    lcd.print(temp_set_mode);

    // button check
    if (digitalRead(btn1) == 0) {
      temp_set_mode++;
    }
    if (digitalRead(btn2) == 0 ) {
      temp_set_mode--;
    }
    if (digitalRead(btn3) == 0 ) {
      Firebase.setInt(firebaseData, "/temp_set", temp_set_mode);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Setting complete");
      flag = 1;
    }
  }
  if (flag == 3) {
    //Set hum now
    Firebase.setInt(firebaseData, "/hum_now", humidity);
    get_pump();


    if (pump == 0) {
      digitalWrite(relay1, LOW);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("(Mode2) Humidity");
      lcd.setCursor(0, 1);
      lcd.print("NOW : ");
      lcd.print(humidity);
      lcd.print("%");
      lcd.setCursor(0, 3);
      lcd.print("Pump : Open");
    }
    else if (pump == 1) {
      digitalWrite(relay1, HIGH);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("(Mode2) Humidity");
      lcd.setCursor(0, 1);
      lcd.print("NOW : ");
      lcd.print(humidity);
      lcd.print("%");
      lcd.setCursor(0, 3);
      lcd.print("Pump : Close");
    }

    // Button check
    if (digitalRead(btn1) == 0) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Next mode...");
      delay(2000);
      lcd.clear();
      flag = 4;
      Firebase.setInt(firebaseData, "/mode", flag);
    }
    if (digitalRead(btn2) == 0) {
      digitalWrite(relay1, LOW);
      lcd.setCursor(0, 3);
      lcd.print("Pump : Open ");
    }
    if (digitalRead(btn3) == 0) {
      digitalWrite(relay1, HIGH);
      lcd.setCursor(0, 3);
      lcd.print("Pump : Close");
    }
  }
  if (flag == 4) {



    get_pump();

    if (pump == 0) {
      lcd.clear();
      digitalWrite(relay1, LOW);
      lcd.setCursor(0, 0);
      lcd.print("(Mode3) Manual");
      lcd.setCursor(0, 1);
      lcd.print("Pump : Open");
      lcd.setCursor(0, 2);
      lcd.print("SW2 : Open pump");
      lcd.setCursor(0, 3);
      lcd.print("SW3 : Close pump");
    }
    else if (pump == 1) {
      lcd.clear();
      digitalWrite(relay1, HIGH);
      lcd.setCursor(0, 0);
      lcd.print("(Mode3) Manual");
      lcd.setCursor(0, 1);
      lcd.print("Pump : Close");
      lcd.setCursor(0, 2);
      lcd.print("SW2 : Open pump");
      lcd.setCursor(0, 3);
      lcd.print("SW3 : Close pump");
    }


    // Button check
    if (digitalRead(btn1) == 0) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Next mode...");
      delay(2000);
      lcd.clear();
      flag = 1;
      Firebase.setInt(firebaseData, "/mode", flag);
    }
    if (digitalRead(btn2) == 0) {
      digitalWrite(relay1, LOW);
      lcd.setCursor(0, 1);
      lcd.print("Pump : Open ");
    }
    if (digitalRead(btn3) == 0) {
      digitalWrite(relay1, HIGH);
      lcd.setCursor(0, 1);
      lcd.print("Pump : Close");
    }
  }


}


void get_mode() {
  //  Firebase Settings
  if (Firebase.getInt(firebaseData, "/mode")) {
    if (firebaseData.dataType() == "int") {
      flag = firebaseData.intData();
    }
  } else {
    Serial.println(firebaseData.errorReason());
  }
}

void get_pump() {
  //  Firebase Settings
  if (Firebase.getInt(firebaseData, "/pump")) {
    if (firebaseData.dataType() == "int") {
      pump = firebaseData.intData();
    }
  } else {
    Serial.println(firebaseData.errorReason());
  }
}

void get_temp_set() {
  //  Firebase Settings
  if (Firebase.getInt(firebaseData, "/temp_set")) {
    if (firebaseData.dataType() == "int") {
      temp_set = firebaseData.intData();
    }
  } else {
    Serial.println(firebaseData.errorReason());
  }
}

void get_temp_set_mode () {
  //  Firebase Settings
  if (Firebase.getInt(firebaseData, "/temp_set")) {
    if (firebaseData.dataType() == "int") {
      temp_set_mode = firebaseData.intData();
    }
  } else {
    Serial.println(firebaseData.errorReason());
  }
}
