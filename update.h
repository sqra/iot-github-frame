const int FW_VERSION = 1;
int newVersion;
const char* versionFileUrlBase = "https://raw.githubusercontent.com/sqra/IoTGitHubFrame/master/OTA/IoTGithubFrame.version";
const char* binFileUrlBase = "http://rawcdn.githack.com/sqra/IoTGitHubFrame/80b3b30d40305a3f6320795a1a6f87b01cbd65b5/OTA/IoTGithubFrame.ino.esp32.";

// update firmware
void updateNow() {
  Serial.println( "Preparing to update firmware" );
  String binURL = String( binFileUrlBase );
  binURL.concat(newVersion + ".bin");
  Serial.print( "Firmware .bin file URL: " );
  Serial.println( binURL );
  HTTPClient espClient;
  espClient.begin( binURL );
  int httpCode = espClient.GET();
  if ( httpCode == 200 ) {
    
    t_httpUpdate_return ret = ESPhttpUpdate.update( binURL );
    
    switch (ret) {
      case HTTP_UPDATE_OK:
        Serial.printf("HTTP_UPDATE_OK");
        Serial.println("");
        break;

      case HTTP_UPDATE_FAILED:
        Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
        Serial.println("");
        break;

      case HTTP_UPDATE_NO_UPDATES:
        Serial.println("HTTP_UPDATE_NO_UPDATES");
        break;
    }
  } else {
    Serial.print( "Firmware update failed, got HTTP response code " );
    Serial.println( httpCode );
  }
  espClient.end();
}

// check if there is a new version of the firmware
void checkForUpdates() {
  String fwURL = String( versionFileUrlBase );
  Serial.println( "Checking for firmware updates." );
  Serial.print( "Firmware version URL: " );
  Serial.println( fwURL );

  HTTPClient httpClient;
  httpClient.begin( fwURL );
  int httpCode = httpClient.GET();
  if ( httpCode == 200 ) {
    String newFWVersion = httpClient.getString();

    Serial.print( "Current firmware version: " );
    Serial.println( FW_VERSION );
    Serial.print( "Available firmware version: " );
    Serial.println( newFWVersion );

    newVersion = newFWVersion.toInt();

    if ( newVersion > FW_VERSION ) {
      updateNow();
    }
    else {
      Serial.println( "Already on latest version." );
    }
  }
  else {
    Serial.print( "Firmware version check failed, got HTTP response code " );
    Serial.println( httpCode );
  }
  httpClient.end();
}
