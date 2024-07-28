#include <Wire.h>
#include <MKRGSM.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>

// Your GPRS credentials (leave empty, if your SIM card has no PIN)
const char PINNUMBER[] = "";
const char GPRS_APN[] = "your_apn";  // replace with your GPRS APN
const char GPRS_LOGIN[] = "";
const char GPRS_PASSWORD[] = "";

// initialize the library instance
GSMClient client;
GPRS gprs;
GSM gsmAccess;

const char server[] = "api.thingspeak.com";
String apiKey = "your_API_KEY";  // replace with your ThingSpeak API key

float temperature = 0.0;

// For the display
#define TFT_CS        10
#define TFT_RST       9
#define TFT_DC        8
#define TFT_BLK       6  // PWM pin for backlight control
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

void setup() {
    Serial.begin(9600);
    while (!Serial) {
        ;  // wait for serial port to connect. Needed for native USB port only
    }

    // connection state
    boolean notConnected = true;

    // After starting the modem with GSM.begin()
    // attach to the GPRS network with the APN, login and password
    while (notConnected) {
        if ((gsmAccess.begin(PINNUMBER) == GSM_READY) &&
            (gprs.attachGPRS(GPRS_APN, GPRS_LOGIN, GPRS_PASSWORD) == GPRS_READY)) {
            notConnected = false;
        } else {
            delay(1000);
            Serial.println("Connecting to GSM network...");
        }
    }

    Serial.println("Connected to GSM network");

    // Initialize the display
    tft.initR(INITR_BLACKTAB);  // Initialize ST7735S display
    tft.setRotation(1);
    tft.fillScreen(ST7735_BLACK);
    tft.setTextColor(ST7735_WHITE);
    tft.setTextSize(1);

    // Set up backlight control
    pinMode(TFT_BLK, OUTPUT);
    analogWrite(TFT_BLK, 255);  // Set backlight to maximum brightness

    Wire.begin(8);  // Join I2C bus with address #8
    Wire.onReceive(receiveEvent);  // Register event
}

void loop() {
    if (temperature != 0.0) {
        sendDataToThingSpeak(temperature);
        displayTemperature(temperature);  // Display the temperature on the screen
    }
    delay(20000);  // 20 seconds delay between updates
}

void receiveEvent(int howMany) {
    if (howMany >= sizeof(temperature)) {
        Wire.readBytes((char*)&temperature, sizeof(temperature));
    }
}

void sendDataToThingSpeak(float temp) {
    if (client.connect(server, 80)) {
        String postStr = apiKey;
        postStr += "&field1=";
        postStr += String(temp);
        postStr += "\r\n\r\n";

        client.print("POST /update HTTP/1.1\n");
        client.print("Host: ");
        client.print(server);
        client.print("\n");
        client.print("Connection: close\n");
        client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
        client.print("Content-Type: application/x-www-form-urlencoded\n");
        client.print("Content-Length: ");
        client.print(postStr.length());
        client.print("\n\n");
        client.print(postStr);

        Serial.print("Temperature: ");
        Serial.print(temp);
        Serial.println(" C, Data sent to ThingSpeak");

        client.stop();
    } else {
        Serial.println("Connection to ThingSpeak failed");
    }
}

void displayTemperature(float temp) {
    tft.fillScreen(ST7735_BLACK);  // Clear the screen
    tft.setCursor(0, 0);
    tft.setTextSize(2);
    tft.setTextColor(ST7735_WHITE);
    tft.print("Temp: ");
    tft.print(temp);
    tft.println(" C");
}
