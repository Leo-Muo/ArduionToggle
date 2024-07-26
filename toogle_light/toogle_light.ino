#include <WiFi.h>
#include <WiFiClient.h>
#include <HTTPClient.h>

#define GREEN_LED 21
#define RED_LED 19
#define BLUE_LED 18
#define BUTTON_PIN  14

unsigned long last_debounce_time = 0;
unsigned long debounce_delay = 50; // Debounce delay in milliseconds
bool led_state = LOW;
int last_state = HIGH;
int current_state;
bool status = false;


String HOST_NAME   = "http://172.20.10.3:5001"; // CHANGE IT
String POST_NAME   = "/set_light"; // CHANGE IT
String GET_PATH    = "/status";

const char* ssid = "LordvirG";
const char* password = "leoHacking";

void scanNetwork(const char* ssid, bool& isFound) {
    Serial.println("Scan start");
    digitalWrite(RED_LED, HIGH); // turn the LED on
    // WiFi.scanNetworks will return the number of networks found.
    int n = WiFi.scanNetworks();
    Serial.println("Scan done");

    if (n == 0) {
        Serial.println("no networks found");
    } else {
        Serial.print(n);
        Serial.println(" networks found");
        Serial.println("Nr | SSID                             | RSSI | CH | Encryption");
        for (int i = 0; i < n; ++i) {
            // Print SSID and RSSI for each network found
            Serial.printf("%2d", i + 1);
            Serial.print(" | ");
            Serial.printf("%-32.32s", WiFi.SSID(i).c_str());
            Serial.print(" | ");
            Serial.printf("%4d", WiFi.RSSI(i));
            Serial.print(" | ");
            Serial.printf("%2d", WiFi.channel(i));
            Serial.print(" | ");

            switch (WiFi.encryptionType(i)) {
                case WIFI_AUTH_OPEN:
                    Serial.print("open");
                    break;
                case WIFI_AUTH_WEP:
                    Serial.print("WEP");
                    break;
                case WIFI_AUTH_WPA_PSK:
                    Serial.print("WPA");
                    break;
                case WIFI_AUTH_WPA2_PSK:
                    Serial.print("WPA2");
                    break;
                case WIFI_AUTH_WPA_WPA2_PSK:
                    Serial.print("WPA+WPA2");
                    break;
                case WIFI_AUTH_WPA2_ENTERPRISE:
                    Serial.print("WPA2-EAP");
                    break;
                case WIFI_AUTH_WPA3_PSK:
                    Serial.print("WPA3");
                    break;
                case WIFI_AUTH_WPA2_WPA3_PSK:
                    Serial.print("WPA2+WPA3");
                    break;
                case WIFI_AUTH_WAPI_PSK:
                    Serial.print("WAPI");
                    break;
                default:
                    Serial.print("unknown");
            }
            Serial.println();
            delay(10);

            if (strcmp(ssid, WiFi.SSID(i).c_str()) == 0) {
                Serial.println("The network found");
                isFound = true;
            }
        }
    }

    if (!isFound) {
        Serial.println("The network not found");
    }
    Serial.println("");
}

void initWiFi() {
    bool isFound = false;  // Move isFound declaration inside the function

    WiFi.mode(WIFI_STA);

    while (!isFound) {
        scanNetwork(ssid, isFound);
    }
    digitalWrite(RED_LED, LOW); // turn the LED on
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi ..");

    while (WiFi.status() != WL_CONNECTED) {
        Serial.print('.');
        digitalWrite(RED_LED, HIGH); // turn the LED on
        delay(300); 
        digitalWrite(RED_LED, LOW);  
        delay(300);
    }
    digitalWrite(GREEN_LED, HIGH); // turn the LED on
    Serial.println(WiFi.localIP());
}

void checkConnection() {
    if (WiFi.status() != WL_CONNECTED) {
        digitalWrite(GREEN_LED, LOW); // turn the LED off
        WiFi.disconnect();
        initWiFi();
    }
}



void setup() {
    // put your setup code here, to run once:
    Serial.begin(115200);
    Serial.println("Hello, ESP32!");

    pinMode(GREEN_LED, OUTPUT);
    pinMode(RED_LED, OUTPUT);
    pinMode(BLUE_LED, OUTPUT);
    pinMode(BUTTON_PIN, INPUT_PULLUP);

    initWiFi();
    Serial.print("RSSI: ");
    Serial.println(WiFi.RSSI());  

}


// void checkStatus(){
//   WiFiClient client; 
//   HTTPClient http;

//   http.begin(client, HOST_NAME + GET_PATH);  //HTTP
//   int httpCode = http.GET();

//   // httpCode will be negative on error
//   if (httpCode > 0) {
//     // file found at server
//     if (httpCode == HTTP_CODE_OK) {
//       String payload = http.getString();
//       Serial.println(payload);
//     } else {
//       // HTTP header has been sent and Server response header has been handled
//       Serial.printf("[HTTP] GET... code: %d\n", httpCode);
//     }
//   } else {
//     Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
//   }

//   http.end();
// }



void loop() {
    checkConnection();
    // checkStatus();
      
    int reading = digitalRead(BUTTON_PIN);

    // Check if the button state has changed
    if (reading != last_state) {
        // Reset the debounce timer
        last_debounce_time = millis();
    }

    // Check if the debounce delay has passed
    if ((millis() - last_debounce_time) > debounce_delay) {
        // If the button state has changed
        if (reading != current_state) {
            current_state = reading;

            // Only toggle the LED if the button is pressed (LOW state)
            if (current_state == LOW) {
                led_state = !led_state;
                digitalWrite(BLUE_LED, led_state);

                if (led_state == HIGH) {
                    Serial.println("The LED is ON");
                    status = true;
                } else {
                    Serial.println("The LED is OFF");
                    status = false;
                }

                HTTPClient http;
                http.begin(HOST_NAME + POST_NAME); //HTTP

                http.addHeader("Content-Type", "application/json");

                String statusString = (status) ? "True" : "False";


                Serial.println(String(statusString));

                String jsonPayload = "{\"status\":\"" + statusString + "\"}";


                int httpCode = http.POST(jsonPayload);
                String payload = http.getString();
                // httpCode will be negative on error
                if(httpCode > 0) {
                  // file found at server
                  if(httpCode == HTTP_CODE_OK) {
                    
                    Serial.println(payload);
                  } else {
                    // HTTP header has been send and Server response header has been handled
                    Serial.printf("[HTTP] POST... code: %d\n", httpCode);
                  }
                } else {
                  Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
                  Serial.println(jsonPayload);
                }

                http.end();
            }
        }
    }

    // Save the reading. Next time through the loop, it'll be the last_state
    last_state = reading;
}
