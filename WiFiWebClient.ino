
#ifndef __CC3200R1M1RGC__
// Do not include SPI for CC3200 LaunchPad
#include <SPI.h>
#endif
#include <WiFi.h>

#define PotPin 6
#define HalogenPin 38


// your network name also called SSID
char ssid[] = "AIRTIES-GUEST";
// your network password
char password[] = "wirelesslife";

int potValue;
int potValuePrev;

// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
//IPAddress server(50,62,217,1);  // numeric IP for Google (no DNS)
char server[] = "cis450.com";    // name address for Google (using DNS)

// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
WiFiClient client;

int moduleToNum(char *module) {
  
  if (strcmp(module,"FxHit4thSA")==0) {
    return 1;
  } else if (strcmp(module,"6LkEwn2IXM")==0) {
    return 4;
  } else if (strcmp(module,"ZRx0iKWXuM")==0) {
    return 5;
  } else if (strcmp(module,"0JEBuCn7WL")==0) {
    return 7;
  }
}

int numToPin(int num) {
  Serial.println("!!!!**");
    Serial.println(num);
      Serial.println("!!!!**");

  if (num==1) {
    return 38;
  } else if (num==3) {
    return 6;
  } else if (num==4) {
    return 12;
  } else if (num==7) {
    return 10;
  }
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

void connection(char *r) {
  if (client.connect(server, 80)) {
    // Make a HTTP request:
    char req[80];
    bzero(req, 80);
    strcat(req,"GET /");
    strcat(req,r);
    strcat(req," HTTP/1.1");
    client.println(req);
    Serial.println(req);
    client.println("Host: cis450.com");
    client.println("Connection: close");
    client.println();
  }
}

void setup() {
  Serial.println("starting");
  //Initialize serial and wait for port to open:
  Serial.begin(115200);

  pinMode(19, OUTPUT);
  pinMode(12, OUTPUT); //fan
  pinMode(10, OUTPUT); //led
  pinMode(38, OUTPUT); //lamp
  
  digitalWrite(12,LOW);
  digitalWrite(10,LOW);
  analogWrite(38,300);
  
  
  // attempt to connect to Wifi network:
  Serial.print("Attempting to connect to Network named: ");
  // print the network name (SSID);
  Serial.println(ssid); 
  // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
  WiFi.begin(ssid, password);
  while ( WiFi.status() != WL_CONNECTED) {
    // print dots while we wait to connect
    Serial.print(".");
    delay(300);
  }
  
  Serial.println("\nYou're connected to the network");
  Serial.println("Waiting for an ip address");
  
  while (WiFi.localIP() == INADDR_NONE) {
    // print dots while we wait for an ip addresss
    Serial.print(".");
    delay(300);
  }

  Serial.println("\nIP Address obtained");
  printWifiStatus();

  Serial.println("\nStarting connection to server...");
  // if you get a connection, report back via serial:
  
}

void HalogenLamp() {
  potValue = analogRead(PotPin) * (70.0 /1023) ; 
  if (abs(potValue - potValuePrev) > 10) {
    analogWrite(HalogenPin, potValue);
  }
  
}

void event(char *command, char *module, char *url, char *isAnalog) {
  int pin = numToPin(moduleToNum(module));
  int num;
  if (strcmp(command,"on")==0) {
    Serial.println("ON!!!!");
    num = 1;
  } else if (strcmp(command,"off")==0) {
    Serial.println("OFF!!!!");
    num = 0;
  }
  
  if (isAnalog[0]=='t') {
    if (num==1) {
      applyAnalog(pin, 0);
    } else {
      applyAnalog(pin, 300);
    }
    
  } else {
    applyDigital(pin, num);
  }
  
  connection(url);

}

void applyDigital(int pin, int num) {
  if (num==1) {
     digitalWrite(pin, HIGH);
  } else {
    digitalWrite(pin, LOW);
  }
}

void applyAnalog(int pin, int num) {
  potValuePrev = potValue;
  Serial.println(num);
  analogWrite(pin,num);
}

void echo() {
  // if there are incoming bytes available
  // from the server, read them and print them:
  
  char string[500];
  char *p;
  int i = 0;
  int j = 0;

  char *url;
  char *command;
  char *module;
  char *isAnalog;
  
  int isparsed = 0;
  
  bzero(string, 500);
  while (client.available()) {
    string[i] = client.read();
    i++;
  }
  
  p = strtok(string, "**");
  while(p != NULL) {      
      if (j==1) {
        url = (char*)malloc(sizeof(char)*strlen(p)+1);
        strcpy(url,p);
      } else if (j==2) {
        command = (char*)malloc(sizeof(char)*strlen(p)+1);
        strcpy(command,p);
      } else if (j==3) {
        module = (char*)malloc(sizeof(char)*strlen(p)+1);
        strcpy(module,p);
      } else if (j==4) {
        isAnalog = (char*)malloc(sizeof(char)*strlen(p)+1);
        strcpy(isAnalog,p);
        isparsed = 1;
      }

      p = strtok(NULL, "**");
      j++; 
  }
  
  
 if (isparsed) {
   event(command, module, url, isAnalog);
 }
 
 if (!client.connected()) {
    client.stop();
  }
 
  
  free(url);
  free(command);
  free(module);
  free(isAnalog);
  
  isparsed = 0;
  
}

void loop() {
  
  connection("requests.php");
  echo();
  
  HalogenLamp();
  
  
}


