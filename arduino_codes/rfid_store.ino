/* ------------------------------------------------------------------------
 * Created by: Tauseef Ahmad
 * Created on: 17 July, 2022
 *
 * Tutorial: https://youtu.be/Bgs_3F5rL5Q
 * ------------------------------------------------------------------------
 * Download Resources
 * ------------------------------------------------------------------------
 * Preferences--> Aditional boards Manager URLs :
 * For ESP8266 and NodeMCU - Board Version 2.6.3
 * http://arduino.esp8266.com/stable/package_esp8266com_index.json
 * ------------------------------------------------------------------------
 * HTTPS Redirect Library:
 * https://github.com/jbuszkie/HTTPSRedirect
 * Example Arduino/ESP8266 code to upload data to Google Sheets
 * Follow setup instructions found here:
 * https://github.com/StorageB/Google-Sheets-Logging
 * ------------------------------------------------------------------------*/

#include <MFRC522.h>
#include <SPI.h>
//--------------------------------------------------
// GPIO 0 --> D3
// GPIO 2 --> D4
const uint8_t RST_PIN = 0;
const uint8_t SS_PIN = 2;
//--------------------------------------------------
MFRC522 rfid(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;
//--------------------------------------------------
/* Be aware of Sector Trailer Blocks */
int blockNum = 4;
/* Create array to read data from Block */
/* Length of buffer should be 4 Bytes
more than the size of Block (16 Bytes) */
byte bufferLen = 18;
byte readBlockData[18];
//--------------------------------------------------
MFRC522::StatusCode status;
//--------------------------------------------------

void setup()
{
  //------------------------------------------------------
  // Initialize serial communications with PC
  Serial.begin(9600);
  //------------------------------------------------------
  // Initialize SPI bus
  SPI.begin();
  //------------------------------------------------------
  // Initialize MFRC522 Module
  rfid.PCD_Init();
  Serial.println("Scan a MIFARE 1K Tag to write data...");
  //------------------------------------------------------
}

/****************************************************************************************************
 * loop() function
 ****************************************************************************************************/
void loop()
{
  //------------------------------------------------------------------------------
  /* Prepare the ksy for authentication */
  /* All keys are set to FFFFFFFFFFFFh at chip delivery from the factory */

  for (byte i = 0; i < 6; i++)
  {
    key.keyByte[i] = 0xFF;
  }
  //------------------------------------------------------------------------------
  /* Look for new cards */
  /* Reset the loop if no new card is present on RC522 Reader */
  if (!rfid.PICC_IsNewCardPresent())
  {
    return;
  }
  //------------------------------------------------------------------------------
  /* Select one of the cards */
  if (!rfid.PICC_ReadCardSerial())
  {
    return;
  }
  //------------------------------------------------------------------------------
  Serial.print("\n");
  Serial.println("**Card Detected**");
  /* Print UID of the Card */
  Serial.print(F("Card UID:"));
  for (byte i = 0; i < rfid.uid.size; i++)
  {
    Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(rfid.uid.uidByte[i], HEX);
  }
  Serial.print("\n");
  /* Print type of card (for example, MIFARE 1K) */
  Serial.print(F("PICC type: "));
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  Serial.println(rfid.PICC_GetTypeName(piccType));

  //------------------------------------------------------------------------------
  byte buffer[18];
  byte len;
  // wait until 20 seconds for input from serial
  Serial.setTimeout(20000L);

  // MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  Serial.println(F("---------------------------------------"));
  Serial.println(F("Enter Card Type, ending with #"));
  len = Serial.readBytesUntil('#', (char *)buffer, 16);
  // add empty spaces to the remaining bytes of buffer
  for (byte i = len; i < 16; i++)
    buffer[i] = ' ';
  blockNum = 4;
  WriteDataToBlock(blockNum, buffer);
  ReadDataFromBlock(blockNum, readBlockData);
  dumpSerial(blockNum, readBlockData);

  // MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  Serial.println(F("---------------------------------------"));
  Serial.println(F("Enter Card Id, ending with #"));
  len = Serial.readBytesUntil('#', (char *)buffer, 16);
  for (byte i = len; i < 16; i++)
    buffer[i] = ' ';
  blockNum = 5;
  WriteDataToBlock(blockNum, buffer);
  ReadDataFromBlock(blockNum, readBlockData);
  dumpSerial(blockNum, readBlockData);

  // // MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  // Serial.println(F("---------------------------------------"));
  // Serial.println(F("Enter Last Name, ending with #"));
  // len = Serial.readBytesUntil('#', (char *)buffer, 16);
  // for (byte i = len; i < 16; i++) buffer[i] = ' ';
  // blockNum = 6;
  // WriteDataToBlock(blockNum, buffer);
  // ReadDataFromBlock(blockNum, readBlockData);
  // dumpSerial(blockNum, readBlockData);

  // // MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  // Serial.println(F("---------------------------------------"));
  // Serial.println(F("Enter Phone Number, ending with #"));
  // len = Serial.readBytesUntil('#', (char *)buffer, 16);
  // for (byte i = len; i < 16; i++) buffer[i] = ' ';
  // blockNum = 8;
  // WriteDataToBlock(blockNum, buffer);
  // ReadDataFromBlock(blockNum, readBlockData);
  // dumpSerial(blockNum, readBlockData);

  // Serial.println(F("---------------------------------------"));
  // Serial.println(F("Enter Address, ending with #"));
  // len = Serial.readBytesUntil('#', (char *)buffer, 16);
  // for (byte i = len; i < 16; i++) buffer[i] = ' ';
  // blockNum = 9;
  // WriteDataToBlock(blockNum, buffer);
  // ReadDataFromBlock(blockNum, readBlockData);
  // dumpSerial(blockNum, readBlockData);
}

/****************************************************************************************************
 * Writ() function
 ****************************************************************************************************/
void WriteDataToBlock(int blockNum, byte blockData[])
{
  // Serial.print("Writing data on block ");
  // Serial.println(blockNum);
  //------------------------------------------------------------------------------
  /* Authenticating the desired data block for write access using Key A */
  status = rfid.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, blockNum, &key, &(rfid.uid));
  if (status != MFRC522::STATUS_OK)
  {
    Serial.print("Authentication failed for Write: ");
    Serial.println(rfid.GetStatusCodeName(status));
    return;
  }
  //------------------------------------------------------------------------------
  else
  {
    // Serial.print("Authentication OK - ");
  }
  //------------------------------------------------------------------------------
  /* Write data to the block */
  status = rfid.MIFARE_Write(blockNum, blockData, 16);
  if (status != MFRC522::STATUS_OK)
  {
    Serial.print("Writing to Block failed: ");
    Serial.println(rfid.GetStatusCodeName(status));
    return;
  }
  else
  {
    // Serial.println("Write OK");
  }
  //------------------------------------------------------------------------------
}

/****************************************************************************************************
 * ReadDataFromBlock() function
 ****************************************************************************************************/
void ReadDataFromBlock(int blockNum, byte readBlockData[])
{
  // Serial.print("Reading data from block ");
  // Serial.println(blockNum);
  //----------------------------------------------------------------------------
  /* Prepare the ksy for authentication */
  /* All keys are set to FFFFFFFFFFFFh at chip delivery from the factory */
  for (byte i = 0; i < 6; i++)
  {
    key.keyByte[i] = 0xFF;
  }
  //------------------------------------------------------------------------------
  /* Authenticating the desired data block for Read access using Key A */
  status = rfid.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, blockNum, &key, &(rfid.uid));
  //------------------------------------------------------------------------------
  if (status != MFRC522::STATUS_OK)
  {
    Serial.print("Authentication failed for Read: ");
    Serial.println(rfid.GetStatusCodeName(status));
    return;
  }
  else
  {
    // Serial.print("Authentication OK - ");
  }
  //------------------------------------------------------------------------------
  /* Reading data from the Block */
  status = rfid.MIFARE_Read(blockNum, readBlockData, &bufferLen);
  if (status != MFRC522::STATUS_OK)
  {
    Serial.print("Reading failed: ");
    Serial.println(rfid.GetStatusCodeName(status));
    return;
  }
  else
  {
    // readBlockData[16] = ' ';
    // readBlockData[17] = ' ';
    // Serial.println("Read OK");
  }
  //------------------------------------------------------------------------------
}

/****************************************************************************************************
 * dumpSerial() function
 ****************************************************************************************************/
void dumpSerial(int blockNum, byte blockData[])
{
  Serial.print("\n");
  Serial.print("Data saved on block");
  Serial.print(blockNum);
  Serial.print(": ");
  for (int j = 0; j < 16; j++)
  {
    Serial.write(readBlockData[j]);
  }
  Serial.print("\n");

  // Empty readBlockData array
  for (int i = 0; i < sizeof(readBlockData); ++i)
    readBlockData[i] = (char)0; // empty space
}
