#include <ESP8266WiFi.h>

void setup()
{
  pinMode(2, OUTPUT);
  Serial.begin(115200);
  Serial.println();

  WiFi.begin("Dima", "dima0519");

  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    //Serial.print(".");
    digitalWrite(2, HIGH);
    delay(500);
    digitalWrite(2, LOW);
    
  }
  Serial.println();

  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());

  WiFiClient client;
  while(!client.connect("192.168.0.108", 4554)){
    delay(500);
    Serial.print("."); 
  }

  client.println("/hst esp8266/topic basic/type register/nof");
  client.stop();
}

void loop() {
  WiFiClient client;
  while(!client.connect("192.168.0.108", 4554)){
    delay(500);
    Serial.print("."); 
  }

  client.println("/body 5 moles ethyl detected/topic basic/hst esp8266/type broadcast/nof");
  client.stop();
  delay(2000);
}
