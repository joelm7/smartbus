// both thinkspeak and firebase used
#include <SoftwareSerial.h>     
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <FirebaseArduino.h>
#define FIREBASE_HOST "sbus-ede13.firebaseio.com"
#define FIREBASE_AUTH "D5FT7VmeqQdXU8QsMeYpt09T5DMKov3gnVPUg6Ee"
SoftwareSerial NodeMCU(D1, D2);
WiFiClient client;
  int a;
  int b;
  int c;
  int i;
  int j;
  int k;
  int t;
  int total;
String thingSpeakAddress = "http://api.thingspeak.com/update?";
String writeAPIKey;
String tsfield1Name;
String request_string;

HTTPClient http;

void setup()
{
  Serial.begin(9600);
  NodeMCU.begin(9600); //for serial communication with arduino 
  // pins for serial comm
  pinMode(D1, INPUT);
  pinMode(D2, OUTPUT);
  pinMode(D0, OUTPUT); // connection status led
  WiFi.disconnect();
  WiFi.begin("AndroidAP", "12345678"); //username and password for wifi
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  digitalWrite (D0,HIGH);
  Serial.println(WiFi.localIP());
  
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH); //connecting to firebase
}

void loop()
{

  String content = "";
  char character;
  while (NodeMCU.available()) {
    character = NodeMCU.read();
    content.concat(character);
  }
//variables for storing value of student 1, student 2, student 3, total
  a= content.charAt(1);
  b= content.charAt(3);
  c= content.charAt(5);
  t= content.charAt(7);
  
if(a!=0){
  Serial.println(content);    
i=a-'0'; // to convert asci to int
j=b-'0';
k=c-'0';
total=t-'0';
  Serial.print("STUDENT 1 =");
  Serial.println(i);
  Serial.print("STUDENT 2 =");
  Serial.println(j);
  Serial.print("STUDENT 3 =");
  Serial.println(k);
  Serial.print("TOTAL =");
  Serial.println(total);
}
  // updating values to thinkspeak API 
  if (client.connect("api.thingspeak.com", 80)) {
    request_string = thingSpeakAddress;
    request_string += "key=";
    request_string += "BEL3O00039TNV1N7";
    request_string += "&";
    request_string += "field1";
    request_string += "=";
    request_string += i;
    request_string += "&";
    request_string += "field2";
    request_string += "=";
    request_string += j;
    request_string += "&";
    request_string += "field3";
    request_string += "=";
    request_string += k;
    request_string += "&";
    request_string += "field4";
    request_string += "=";
    request_string += total;
  //  if(a!=0){
    Serial.println(request_string);
    http.begin(request_string);
    http.GET();
    http.end();
     // set values in firebase fields
  Firebase.setInt("student1", i);
  Firebase.setInt("student2", j);
  Firebase.setInt("student3", k);
  Firebase.setInt("total", total);
  // handle error
  if (Firebase.failed()) {
      Serial.print("setting /number failed:");
      Serial.println(Firebase.error());  
    }
  }
  delay(1000);
}
