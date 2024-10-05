#include <TimeLib.h>

#include <Vector.h>

// LOLIN(WEMOS) D1 R2 & mini / reinstall CH340 driver
#include <ESP8266WiFi.h>

#define LED D8
#define PIR D6
#define PIR_LEFT D5

#ifndef STASSID
#define STASSID "SMre_EXT"
#define STAPSK "17718972428229327098"
#endif

Vector<String> setupInfo;

namespace Wifi
{
  const char* ssid = STASSID;
  const char* password = STAPSK;

  const char* host = "djxmmx.net";
  const uint16_t port = 17;

  struct Wifi
  {
    void setup()
    {
      // We start by connecting to a WiFi network

      setupInfo.push_back("Connecting to ");
      setupInfo.push_back(ssid);

      /* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
        would try to act as both a client and an access-point and could cause
        network-issues with your other WiFi-devices on your WiFi-network. */
      WiFi.mode(WIFI_STA);
      WiFi.begin(ssid, password);

      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
      }

      setupInfo.push_back("");
      setupInfo.push_back("WiFi connected");
      setupInfo.push_back("IP address: ");
      setupInfo.push_back(WiFi.localIP().toString());
    }

    bool connect()
    {
      //static bool wait = false;

      Serial.print("connecting to ");
      Serial.print(host);
      Serial.print(':');
      Serial.println(port);

      // Use WiFiClient class to create TCP connections
      WiFiClient client;
      if (!client.connect(host, port)) {
        Serial.println("connection failed");
        delay(5000);
        return false;
      }

      // This will send a string to the server
      Serial.println("sending data to server");
      if (client.connected()) { client.println("hello from ESP8266"); }

      // wait for data to be available
      unsigned long timeout = millis();
      while (client.available() == 0) {
        if (millis() - timeout > 5000) {
          Serial.println(">>> Client Timeout !");
          client.stop();
          delay(60000);
          return false;
        }
      }

      // Read all the lines of the reply from server and print them to Serial
      Serial.println("receiving from remote server");
      // not testing 'client.connected()' since we do not need to send data here
      while (client.available()) {
        char ch = static_cast<char>(client.read());
        Serial.print(ch);
      }

      // Close the connection
      Serial.println();
      Serial.println("closing connection");
      client.stop();

      return true;
    }
  };
}

Wifi::Wifi m_wifi;
time_t startTP;

void setup()
{
  Serial.begin(115200);

  // pinMode(PIR, INPUT);
  // pinMode(PIR_LEFT, INPUT);
  // pinMode(LED, OUTPUT);

  // // m_wifi.setup();

  // // startTP = now();
  // // setupInfo.push_back("Setup done");
}

bool checkPIR()
{
    return digitalRead(PIR);
}

bool checkPIRLeft()
{
    Serial.println("PIRLeft.analog = "+String(analogRead(PIR_LEFT)));
    return digitalRead(PIR_LEFT);
}

void printSetupInfo()
{
  Serial.println("printSetupInfo");
  Serial.println("setupInfo "+String(setupInfo.size()));

  while(setupInfo.size())
    Serial.println(setupInfo.at(0));

  setupInfo.clear();
}

void flicker()
{
  Serial.println("flickering red eyes for 1 second");

  for(int i=0;i<20;i++)
  {
    digitalWrite(LED, HIGH);
    delay(50);
    digitalWrite(LED, LOW);
    delay(50);
  }
}

float smoothness_pts = 1000;//larger=slower change in brightness

void flickerPWM()
{
  // // for (int ii=0;ii<smoothness_pts;ii++){
  // //   Serial.println("PIRLeft.analog = "+String(analogRead(PIR_LEFT)));
  // //   float pwm_val = 255.0*(1.0 -  abs((2.0*(ii/smoothness_pts))-1.0));
  // //   analogWrite(LED,int(pwm_val));
  // //   delay(5);
  // // }

  // float fadeUpTime = smoothness_pts/2;

  // for (int ii=0;ii<fadeUpTime;ii++){
  //   float pwm_val = 255.0*(1.0 -  abs((2.0*(ii/smoothness_pts))-1.0));
  //   analogWrite(LED,int(pwm_val));
  //   delay(2 - (ii/fadeUpTime));
  // }

  // for (int ii=fadeUpTime;ii<smoothness_pts;ii++){
  //   float pwm_val = 255.0*(1.0 -  abs((2.0*(ii/smoothness_pts))-1.0));
  //   analogWrite(LED,int(pwm_val));
  //   delay(5 + ((ii-fadeUpTime)/fadeUpTime) * 10);
  // }

  int steps = 25;
  int currentSteps = steps;

  for (int ii=0;ii<smoothness_pts;ii++){
    if(currentSteps)
    {
      if(ii == (int(smoothness_pts/(float)currentSteps)))
        ii = (int)((float)ii/2.0);
      currentSteps--;
    }
    float pwm_val = 255.0*(ii/smoothness_pts);
    analogWrite(LED,int(pwm_val));
    delay(2);
  }

  for (int ii=0;ii<smoothness_pts;ii++){
    float pwm_val = 255.0 - 255.0*(ii/smoothness_pts);
    analogWrite(LED,int(pwm_val));
    delay(5);
  }
}

void flickerLeft()
{
  Serial.println("flickering red eyes for 2 second");

  for(int i=0;i<1;i++)
  {
    digitalWrite(LED, HIGH);
    delay(200);
    digitalWrite(LED, LOW);
    delay(200);
  }
}

void loop()
{
  printSetupInfo();

  // bool PIRLeft = checkPIRLeft();

  // // if(PIRLeft)
  // //   flickerLeft();

  bool PIR = checkPIR();
  
  //if(PIRLeft || PIR)
  if(PIR)
  {
    //Serial.println("PIRLeft = "+String(PIRLeft));
    Serial.println("PIR = "+String(PIR));
    flickerPWM();
    delay(2000);
  }

  // // if(!m_wifi.connect())
  // //   return;

  delay(200);
}