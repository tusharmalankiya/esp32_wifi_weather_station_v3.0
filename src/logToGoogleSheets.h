#ifndef LOGTOGOOGLESHEETS_H
#define LOGTOGOOGLESHEETS_H

#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ESP_Google_Sheet_Client.h>
#include "secrets.h"

// Service Account's private key
const char PRIVATE_KEY[] PROGMEM = GOOGLE_PRIVATE_KEY;

// The ID of the spreadsheet where you'll publish the data
const char spreadsheetId[] = SPREADSHEET_ID;

// Token Callback function
void tokenStatusCallback(TokenInfo info);
void logToGoogleSheet(String updated, float temp, int humidity, String condition);
void logToGoogleSheetSetUp();

void logToGoogleSheetSetUp()
{
    GSheet.printf("ESP Google Sheet Client v%s\n\n", ESP_GOOGLE_SHEET_CLIENT_VERSION);

    // Set the callback for Google API access token generation status (for debug only)
    GSheet.setTokenCallback(tokenStatusCallback);

    // Set the seconds to refresh the auth token before expire (60 to 3540, default is 300 seconds)
    GSheet.setPrerefreshSeconds(10 * 60);

    // Begin the access token generation for Google API authentication
    GSheet.begin(CLIENT_EMAIL, PROJECT_ID, PRIVATE_KEY);

}

void logToGoogleSheet(String updated, float temp, int humidity, String condition){
    // Call ready() repeatedly in loop for authentication checking and processing
    bool ready = GSheet.ready();

        FirebaseJson response;

        Serial.println("\nAppend spreadsheet values...");
        Serial.println("----------------------------");

        FirebaseJson valueRange;

        valueRange.add("majorDimension", "ROWS");
        valueRange.set("values/[0]/[0]", updated);
        valueRange.set("values/[0]/[1]", temp);
        valueRange.set("values/[0]/[2]", humidity);
        valueRange.set("values/[0]/[3]", condition);

        // For Google Sheet API ref doc, go to https://developers.google.com/sheets/api/reference/rest/v4/spreadsheets.values/append
        // Append values to the spreadsheet
        bool success = GSheet.values.append(&response /* returned response */, spreadsheetId /* spreadsheet Id to append */, "Sheet1!A1" /* range to append */, &valueRange /* data range to append */);
        if (success){
            response.toString(Serial, true);
            valueRange.clear();
        }
        else{
            Serial.println(GSheet.errorReason());
        }
        Serial.println();
        Serial.println(ESP.getFreeHeap());
}

void tokenStatusCallback(TokenInfo info){
    if (info.status == token_status_error){
        GSheet.printf("Token info: type = %s, status = %s\n", GSheet.getTokenType(info).c_str(), GSheet.getTokenStatus(info).c_str());
        GSheet.printf("Token error: %s\n", GSheet.getTokenError(info).c_str());
    }
    else{
        GSheet.printf("Token info: type = %s, status = %s\n", GSheet.getTokenType(info).c_str(), GSheet.getTokenStatus(info).c_str());
    }
}

#endif