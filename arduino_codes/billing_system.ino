/*-------------------------------
* Created by: Yash Oswal
* Date: 16/04/2023
* -------------------------------
* GitHub: https://github.com/yashoswalyo
* LinkedIn: https://www.linkedin.com/in/yashoswalyo
* -------------------------------
* 
* MFRC522 interfacing with Node MCU 
* 
* Pin layout used:
* ---------------------
* 
* MFRC522   NODE MCU
* Pin       Pin
* ---------------------
* RST       D3 (GPIO 0)
* SDA(SS)   D4 (GPIO 2)
* MOSI      D7 (GPIO 13)
* MISO      D6 (GPIO 12)
* SCK       D5 (GPIO 14)
* 3.3V      3.3V
* GND       GND
* 
*/

#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecureBearSSL.h>

#include <MFRC522.h>
#include <SPI.h>

const char *ssid = "ENTER_YOUR_SSID";
const char *password = "ENTER_YOUR_PASSWORD";

// Api url
String serverUrl = "https://apibackend-1-u9070050.deta.app";
const uint8_t fingerprint[20] = {0x63, 0x86, 0x14, 0x45, 0x78, 0x19, 0x61, 0xCE, 0xFE, 0x93, 0x89, 0x4B, 0x47, 0xB6, 0x34, 0xE7, 0xB6, 0x94, 0x4B, 0x75};


String customer_id = "";
String card_type = "";

// RFID config
int blocks[] = {4, 5};
#define total_blocks (sizeof(blocks) / sizeof(blocks[0]))

// PINs D3, D4
#define RST_PIN 0
#define SS_PIN 2

MFRC522 rfid(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;
MFRC522::StatusCode status;

int blockNum = 2;
// store block data
byte bufferLen = 18;
byte readBlockData[18];

bool isConnected = false;

void setup()
{
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  isConnected = false;
  Serial.println();

  SPI.begin();
  WiFi.begin(ssid, password);
  Serial.print("Connecting to ");
  Serial.print(ssid);
  Serial.println("...");

  int i = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.print(++i);
    Serial.print(" ");
  }

  Serial.println('\n');
  Serial.println("Connection established!");
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());
  rfid.PCD_Init();
}

void loop()
{
  if (!rfid.PICC_IsNewCardPresent())
    return;
  if (!rfid.PICC_ReadCardSerial())
    return;

  std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);
  // Ignore SSL certificate validation
  client->setFingerprint(fingerprint);
  

  HTTPClient http;

  Serial.println();
  Serial.println(F("Reading data"));
  String data;

  readDataFromBlock(blocks[0], readBlockData);
  data = String((char *)readBlockData);
  data.trim();
  card_type = data;
  if (card_type.equals("customer"))
  {
    readDataFromBlock(blocks[1], readBlockData);
    data = String((char *)readBlockData);
    data.trim();
    customer_id = data;
    String connUrl;
    if (isConnected == true)
    {
      connUrl = serverUrl + "/user/disconnect/" + customer_id;
      isConnected = false;
      Serial.println("\nDisconnect");
      customer_id = "";
    }
    else
    {
      connUrl = serverUrl + "/user/connect/" + customer_id;
      isConnected = true;
      Serial.println("\nConnect");
    }
    http.begin(*client,connUrl);
    int httpResponseCode = http.POST("");
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    http.end();
  }
  if (card_type.equals("product") && !customer_id.isEmpty())
  {
    readDataFromBlock(blocks[1], readBlockData);
    data = String((char *)readBlockData);
    String product_id = String(random(9));
    Serial.println("Product id: " + product_id);
    Serial.println();
    String connUrl = serverUrl + "/order/" + customer_id + "/products/" + product_id;
    http.begin(*client, connUrl);
    int httpResponseCode = http.PUT("");
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    http.end();
  }

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}

void readDataFromBlock(int blockNum, byte readBlockData[])
{
  /* Prepare the ksy for authentication */
  /* All keys are set to FFFFFFFFFFFFh at chip delivery from the factory */
  for (byte i = 0; i < 6; i++)
  {
    key.keyByte[i] = 0xFF;
  }
  status = rfid.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, blockNum, &key, &(rfid.uid));
  if (status != MFRC522::STATUS_OK)
  {
    Serial.print("Authentication failed for read: ");
    Serial.println(rfid.GetStatusCodeName(status));
    return;
  }
  else
  {
    Serial.println("Authnetication success");
  }
  status = rfid.MIFARE_Read(blockNum, readBlockData, &bufferLen);
  if (status != MFRC522::STATUS_OK)
  {
    Serial.print("Reading failed: ");
    Serial.println(rfid.GetStatusCodeName(status));
    return;
  }
  else
  {
    readBlockData[16] = ' ';
    readBlockData[17] = ' ';
    Serial.println("Block was read successfully");
  }
}
