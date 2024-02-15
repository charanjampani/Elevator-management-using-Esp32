#include <ESP_Google_Sheet_Client.h>
#include <ESP_Google_Sheet_Client_FS_Config.h>
#include <GS_Const.h>
#include <GS_ESP8266_Supports.h>
#include <GS_Error.h>
#include <GS_Helper.h>
#include <GS_Network.h>
#include <GS_SDHelper.h>

#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 5 //RX slave select
#define RST_PIN 22

MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance.
MFRC522::StatusCode status;
byte card_ID[4]; //card UID size 4byte
byte Name1[4]={0x73,0x2B,0xC2,0x2E};//first UID card
byte Name2[4]={0xE0,0x06,0x61,0x19};//second UID card
int blockNum = 2;  
/* Create an array of 16 Bytes and fill it with data */
/* This is the actual data which is going to be written into the card */
byte blockData [16] = {"Electronics-Hub-"};

/* Create another array to read data from Block */
/* Length of buffer should be 2 Bytes more than the size of Block (16 Bytes) */
byte bufferLen = 18;
byte readBlockData[18];
//if you want the arduino to detect the cards only once
int NumbCard[2];//this array content the number of cards. in my case i have just two cards.
int j=0;        

int const RedLed=6;
int const GreenLed=5;
int const Buzzer=8;

String Name;//user name
String Number;//user number
int n ;//The number of card you want to detect (optional)  

void setup() 
{
  Serial.begin(9600); // Initialize serial communications with the PC
  SPI.begin();  // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522 card

  Serial.println("CLEARSHEET");                 // clears starting at row 1
  Serial.println("LABEL,Date,Time,Name,Number");// make four columns (Date,Time,[Name:"user name"]line 48 & 52,[Number:"user number"]line 49 & 53)

  pinMode(RedLed,OUTPUT);
  pinMode(GreenLed,OUTPUT);
  pinMode(Buzzer,OUTPUT);

}

void loop() 
{
  //look for new card
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;//got to start of loop if there is no card present
  }
 // Select one of the cards
 if ( ! mfrc522.PICC_ReadCardSerial()) 
 {
  return;//if read card serial(0) returns 1, the uid struct contians the ID of the read card.
 }

 for (byte i = 0; i < mfrc522.uid.size; i++) 
 {
    card_ID[i]=mfrc522.uid.uidByte[i];

    if(card_ID[i]==Name1[i])
    {
      Name="CHARAN";//user name
      Number="586";//user number
      j=0;//first number in the NumbCard array : NumbCard[j]
    }
    else if(card_ID[i]==Name2[i])
    {
      Name="SRIKAR";//user name
      Number="5D1";//user number
      j=1;//Second number in the NumbCard array : NumbCard[j]
    }
    else
    {
        digitalWrite(GreenLed,LOW);
        digitalWrite(RedLed,HIGH);
        goto cont;//go directly to line 85
    }
  }
  if(NumbCard[j] != 1 && NumbCard[j]==1)
  { //to check if the card already detect
    //if you want to use LCD
    //Serial.println("Already Exist");
  }
  else
  {
    NumbCard[j] = 1;//put 1 in the NumbCard array : NumbCard[j]={1,1} to let the arduino know if the card was detecting 
    n++;//(optional)
    
    digitalWrite(GreenLed,HIGH);
    digitalWrite(RedLed,LOW);
    digitalWrite(Buzzer,HIGH);
    delay(30);
    digitalWrite(Buzzer,LOW);
    Serial.println("SAVEWORKBOOKAS,Names/WorkNames");
    Sending_To_Spreadsheet();
  }
  delay(1000);
  cont:
  delay(2000);
  digitalWrite(GreenLed,LOW);
  digitalWrite(RedLed,LOW);

//if you want to close the Excel when all card had detected and save Excel file in Names Folder. in my case i have just 2 card (optional)
//if(n==2){

  //  Serial.println("FORCEEXCELQUIT");
 //   }
}

void Sending_To_spreadsheet()   //CONNECTING WITH MYSQL
 {
   if (client.connect(server, 80)) {
    Serial.println("connected");
    // Make a HTTP request:
    client.print("GET /api.pushingbox.com/pushingbox?devid=v8939833D1CC3AAB");  //YOUR URL
    client.print("&Member_ID=");
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
    card_ID[i]=mfrc522.uid.uidByte[i];

    if(card_ID[i]==Name1[i])
    {
      Name="CHARAN";//user name
      Number="586";//user number
      j=0;//first number in the NumbCard array : NumbCard[j]
    }
    else if(card_ID[i]==Name2[i])
    {
      Name="SRIKAR";//user name
      Number="5D1";//user number
      j=1;//Second number in the NumbCard array : NumbCard[j]
    }
    else
    {
        digitalWrite(GreenLed,LOW);
        digitalWrite(RedLed,HIGH);
        goto cont;//go directly to line 85
    }
  }
    client.print(Number);
    client.print("&Name=",Name);
    client.print(" ");      //SPACE BEFORE HTTP/1.1
    client.print("HTTP/1.1");
    client.println();
    client.println("Host: api.pushingbox.com");
    client.println("Connection: close");
    client.println();
  } else {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
  }
 }
