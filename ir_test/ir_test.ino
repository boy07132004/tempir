#include <Arduino.h>
#include <WiFi.h>

#include "PinDefinitionsAndMore.h" //Define macros for input and output pin etc.
#include <IRremote.hpp>

#define DECODE_NEC          // Includes Apple and Onkyo

const char* ssid = "asus";       // 雙引號內，修改為你要 ESP32 連上的 WiFi 網路名稱 SSID
const char* password = "0989212698";   // 雙引號內，鍵入此網路的密碼

WiFiServer server(80);  //設定網路伺服器 port number 為 80

// Variable to store the HTTP request
String header;

String retrainState = "on";  //設定字串變數 output26State，顯示 GPIO26 狀態。如果您要增加可控制的 GPIO 數目，請在這裡增加
String irState = "on";

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

void setup() {
    Serial.begin(115200);

    pinMode(LED_BUILTIN, OUTPUT);
    /*pinMode(output26, OUTPUT);
    pinMode(output27, OUTPUT);
    // Set outputs to LOW
    digitalWrite(output26, LOW);
    digitalWrite(output27, LOW);*/
    
    Serial.print("Connecting to ");  // 連上你所指定的Wi-Fi，並在序列埠螢幕中，印出 ESP32 web server 的 IP address
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    // Print local IP address and start web server
    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    server.begin();
    
    // Just to know which program is running on my Arduino
    Serial.println(F("START " __FILE__ " from " __DATE__ "\r\nUsing library version " VERSION_IRREMOTE));

    /*
     * The IR library setup. That's all!
     */
    IrSender.begin(4, ENABLE_LED_FEEDBACK); // Start with IR_SEND_PIN as send pin and if NO_LED_FEEDBACK_CODE is NOT defined, enable feedback LED at default feedback LED pin
    IrReceiver.begin(17);
    Serial.print(F("Ready to send IR signals at pin "));
    Serial.println(IR_SEND_PIN);
    Serial.print(F("Ready to receive IR signals of protocols: "));
    printActiveIRProtocols(&Serial);
    Serial.println(F("at pin 17"));
}

/*
 * Set up the data to be sent.
 * For most protocols, the data is build up with a constant 8 (or 16 byte) address
 * and a variable 8 bit command.
 * There are exceptions like Sony and Denon, which have 5 bit address.
 */
uint16_t sAddress1;
uint8_t sCommand1;
uint16_t sAddress2;
uint8_t sCommand2;
uint16_t sAddress3;
uint8_t sCommand3;
uint8_t sRepeats = 0;

void loop() {
    /*
     * Print current send values
     */
    WiFiClient client = server.available();
    
    if (client) {                             // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  // loop while the client's connected
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // 接收 client  的指令，點亮或關閉 連上 GPIO 的 LED。瀏覽器上，不同的按鈕，會發出不同的指令 (URLs requests)
            if (header.indexOf("GET /button1/train") >= 0) {
              Serial.println("Retrain on");
              IrReceiver.start();
              int i = 1;
              while(i=1){
                if (IrReceiver.decode()) {
                  Serial.print(F("Decoded protocol: "));
                  Serial.print(getProtocolString(IrReceiver.decodedIRData.protocol));
                  Serial.print(F(", decoded address: "));
                  Serial.print(IrReceiver.decodedIRData.address, HEX);
                  Serial.print(F(", decoded command: "));
                  Serial.println(IrReceiver.decodedIRData.command, HEX);
        
                  if (IrReceiver.decodedIRData.protocol == NEC) {
                   sAddress1 = IrReceiver.decodedIRData.address;
                   sCommand1 = IrReceiver.decodedIRData.command;
                   break;
                  }
                  IrReceiver.resume(); // Enable receiving of the next value
                }
              }
              IrReceiver.stop();
            } 
            else if (header.indexOf("GET /button1/run") >= 0) {
              Serial.println("IR on");
              IrSender.sendNEC(sAddress1, sCommand1, sRepeats);
            
              Serial.print(F("Send now: address=0x"));
              Serial.print(sAddress1, HEX);
              Serial.print(F(" command=0x"));
              Serial.print(sCommand1, HEX);
              Serial.print(F(" repeats="));
              Serial.print(sRepeats);
              Serial.println();
            } 
            else if (header.indexOf("GET /button2/train") >= 0) {
              Serial.println("Retrain on");
              IrReceiver.start();
              int i = 1;
              while(i=1){
                if (IrReceiver.decode()) {
                 Serial.print(F("Decoded protocol: "));
                 Serial.print(getProtocolString(IrReceiver.decodedIRData.protocol));
                 Serial.print(F(", decoded address: "));
                 Serial.print(IrReceiver.decodedIRData.address, HEX);
                 Serial.print(F(", decoded command: "));
                 Serial.println(IrReceiver.decodedIRData.command, HEX);
        
                 if (IrReceiver.decodedIRData.protocol == NEC) {
                  sAddress2 = IrReceiver.decodedIRData.address;
                  sCommand2 = IrReceiver.decodedIRData.command;
                  break;
                 }
                 IrReceiver.resume(); // Enable receiving of the next value
               }
             }
              IrReceiver.stop();
            }
            else if (header.indexOf("GET /button2/run") >= 0) {
              Serial.println("IR on");
              IrSender.sendNEC(sAddress2, sCommand2, sRepeats);
            
              Serial.print(F("Send now: address=0x"));
              Serial.print(sAddress2, HEX);
              Serial.print(F(" command=0x"));
              Serial.print(sCommand2, HEX);
              Serial.print(F(" repeats="));
              Serial.print(sRepeats);
              Serial.println();
            }
            else if (header.indexOf("GET /button3/train") >= 0) {
              Serial.println("Retrain on");
              IrReceiver.start();
              int i = 1;
              while(i=1){
                if (IrReceiver.decode()) {
                 Serial.print(F("Decoded protocol: "));
                 Serial.print(getProtocolString(IrReceiver.decodedIRData.protocol));
                 Serial.print(F(", decoded address: "));
                 Serial.print(IrReceiver.decodedIRData.address, HEX);
                 Serial.print(F(", decoded command: "));
                 Serial.println(IrReceiver.decodedIRData.command, HEX);
        
                 if (IrReceiver.decodedIRData.protocol == NEC) {
                  sAddress3 = IrReceiver.decodedIRData.address;
                  sCommand3 = IrReceiver.decodedIRData.command;
                  break;
                 }
                 IrReceiver.resume(); // Enable receiving of the next value
               }
             }
              IrReceiver.stop();
            }
            else if (header.indexOf("GET /button3/run") >= 0) {
              Serial.println("IR on");
              IrSender.sendNEC(sAddress3, sCommand3, sRepeats);
            
              Serial.print(F("Send now: address=0x"));
              Serial.print(sAddress2, HEX);
              Serial.print(F(" command=0x"));
              Serial.print(sCommand2, HEX);
              Serial.print(F(" repeats="));
              Serial.print(sRepeats);
              Serial.println();
            }
            
            // 設計 client 上的瀏覽器網頁格式，包括顏色、字型大小、有無邊框、字元等，這一部分是用 HTML 的語言寫成的
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>ESP32 Web Server</h1>");
            
            // Display current state, and ON/OFF buttons for GPIO 26  
            
            client.println("<p>Button1</p>");
            client.println("<p><a href=\"/button1/train\"><button class=\"button\">train</button></a></p>");
            client.println("</body></html>");
            client.println("<p><a href=\"/button1/run\"><button class=\"button\">run</button></a></p>");
            client.println("</body></html>");

            client.println("<p>Button2</p>");
            client.println("<p><a href=\"/button2/train\"><button class=\"button\">train</button></a></p>");
            client.println("</body></html>");
            client.println("<p><a href=\"/button2/run\"><button class=\"button\">run</button></a></p>");
            client.println("</body></html>");

            client.println("<p>Button3</p>");
            client.println("<p><a href=\"/button3/train\"><button class=\"button\">train</button></a></p>");
            client.println("</body></html>");
            client.println("<p><a href=\"/button3/run\"><button class=\"button\">run</button></a></p>");
            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
