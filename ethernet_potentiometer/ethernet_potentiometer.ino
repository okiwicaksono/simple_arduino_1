/*
 Ethernet read analog port
 Reads an analog input on pin A0 and print the result as a web page that
 can be accessed on web browser.
 
 created 12 January 2015
 by Oki Wicaksono
 */

#include <Ethernet.h>
#include <SPI.h>

// assign a MAC address for the ethernet controller.
// fill in your address here:
byte mac[] = { 
  0x90, 0xA2, 0xDA, 0x0D, 0x22, 0x2B};
// assign an IP address for the controller:
IPAddress ip(192,168,0,100);
IPAddress gateway(192,168,0,1);	
IPAddress subnet(255, 255, 255, 0);

int led = 7; // LED port

// Initialize the Ethernet server library
// with the IP address and port you want to use 
// (port 80 is default for HTTP):
EthernetServer server(80);

String request;

long temperature = 0;

void setup() {
  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();

  Serial.begin(9600);
  
  pinMode(led, OUTPUT);

  // give Ethernet shield time to set up:
  delay(1000);
}

void loop() { 
  getData();

  // listen for incoming Ethernet connections:
  listenForEthernetClients();
  
  delay(1000);
}


void getData() {
  Serial.println("\nGetting reading");
  
  // getting temperature data (degrees)
  float data = analogRead(A0);
  
  if (digitalRead(led) == HIGH) {
    Serial.println("ON");
  } else {
    Serial.println("OFF");
  }

  // convert the temperature to celsius and display it:
  temperature = data;

  Serial.print("Temperature: ");
  Serial.print(temperature);
}

void listenForEthernetClients() {
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    Serial.println("\nGot a client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        
        //read char by char HTTP request
        //store characters to string
        request += c;
        
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          if (request.indexOf("?button1on") > 0) {
            digitalWrite(led, HIGH);
            Serial.println("ON BRO!");
          }
          else if (request.indexOf("?button1off") > 0) {
            digitalWrite(led, LOW);
            Serial.println("OFF BRO!");
          }
          
          //clearing string for next read
          request="";

          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println();
          // print the current readings, in HTML format:
          client.print("Temperature: ");
          client.print(temperature);
          client.print(" degrees F");
          client.println("<br />");
          client.println("<a href=\"/?button1on\"\">Turn On LED</a><br />");
          client.println("<a href=\"/?button1off\"\">Turn Off LED</a><br />");   
          client.println("<br />");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } 
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
  }
}
