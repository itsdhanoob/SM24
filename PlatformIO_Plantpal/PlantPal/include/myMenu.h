#ifndef MENU_H
#define MENU_H





#include <WiFi.h>
#include "esp_wifi.h"
#include <esp_now.h>

#include <vector>

#include <omegaTFT.h>
#include <omegaWireless.h>
#include <omegaNOW.h>
#include "NotoSansMonoSCB20.h"
#include "NotoSansBold15.h"

volatile int gyroy = 0;
volatile int gyrop = 0;
volatile int gyror = 0;
uint8_t *test;
omegaWireless wirelessManager = omegaWireless("PlantPal");
EspNowManager espNOW;

 bool espNowActive= false;

TFT_eSPI tft;  // Create object "tft"

TFT_eSprite menuSprite(&tft);
TFT_eSprite valueSprite(&tft);
TFT_eSprite accessorySprite(&tft);



std::vector<MacAddress> connectedMacAddresses;
/*########################################## Menu Functions ##################################################*/

std::vector<MacAddress> getConnectedMacAddresses() {
    std::vector<MacAddress> macAddresses;
  // Get the number of connected stations
  int numStations = WiFi.softAPgetStationNum();
  Serial.print("Number of connected devices: ");
  Serial.println(numStations);

  // Get the list of connected stations
  wifi_sta_list_t stationList;
  tcpip_adapter_sta_list_t adapterList;
 

  if (esp_wifi_ap_get_sta_list(&stationList) == ESP_OK) {
    if (tcpip_adapter_get_sta_list(&stationList, &adapterList) == ESP_OK) {
      for (int i = 0; i < adapterList.num; i++) {

        tcpip_adapter_sta_info_t station = adapterList.sta[i];
        Serial.print("Device ");
        Serial.print(i + 1);
        Serial.print(": MAC Address: ");
        Serial.printf("%02X:%02X:%02X:%02X:%02X:%02X\n",
                      station.mac[0], station.mac[1], station.mac[2],
                      station.mac[3], station.mac[4], station.mac[5]);
        MacAddress macAddress;
        memcpy(macAddress.mac, adapterList.sta[i].mac, sizeof(macAddress.mac)); // Copy MAC address
        macAddresses.push_back(macAddress); // Add MAC address to vector
      }
    }
  }
  return macAddresses;
}



bool drawTest(TFT_eSprite * funSprite,TFT_eSprite * txtSprite, TFT_eSprite * accsrySprite)
{
  accsrySprite->setSwapBytes(1);
  accsrySprite->pushImage(80,80,50,10,icon_Plant1);
  accsrySprite->pushSprite(0,0,TFT_BLACK);

  return 0;



}

bool drawHomeScreen(TFT_eSprite * funSprite,TFT_eSprite * txtSprite, TFT_eSprite * accsrySprite)
{
  static int frameCounter = 0;
  bool doAnimate = true;
  static bool loadFont = true;
  
  if (loadFont)
  {
     txtSprite->loadFont(NotoSansBold15);
     loadFont = false;
  }
  
  if(frameCounter== 0 || frameCounter >7 ) doAnimate = true;

  frameCounter++;
  frameCounter = frameCounter%100;

  static int val1 = 0;
  static int val2 = 0;
  static int val3 = 0;

  val1 = val1%100;
  val2 = val2%100;
  val3= val3%100;

  uint16_t startAngl = 30;
  uint16_t endAngl = 180-startAngl;

  uint16_t angl1 = map(val1,-1, 101,startAngl,endAngl);
  uint16_t angl2 = map(val2,-1, 101,startAngl,endAngl);
  uint16_t angl3 = map(val3,-1, 101,startAngl,endAngl);  

  funSprite->setSwapBytes(1);
  funSprite->fillScreen(0);

  txtSprite->fillScreen(TFT_BLACK);
  txtSprite->setCursor(0,0);
  txtSprite->setTextColor(TFT_WHITE);
  txtSprite->printf("Age: %d Days\n",val1);  

  txtSprite->setTextColor(COLOR_PURPLE);
  txtSprite->printf("%d %%|",val2);  	
  txtSprite->setTextColor(COLOR_BROWN);
  txtSprite->printf("%d %%\n",val1);

  txtSprite->setTextColor(COLOR_RED);
  txtSprite->printf("%d C|",val3);
  txtSprite->setTextColor(COLOR_YELLOW);
  txtSprite->printf("%d \n",val3);
  //txtSprite->printf("%d C\n%d %\n%d $", val1,val2,val3);
  txtSprite->setSwapBytes(1);
  txtSprite->setTextColor(TFT_WHITE);
  txtSprite->pushToSprite(funSprite,85,185);

  funSprite->drawSmoothArc(120,120,118,114,startAngl,angl1,COLOR_RED,TFT_TRANSPARENT,true);
  funSprite->drawSmoothArc(120,120,110,106,startAngl,angl2,COLOR_YELLOW,TFT_TRANSPARENT,true);
  funSprite->drawSmoothArc(120,120,102,98,startAngl,angl3,COLOR_PURPLE,TFT_TRANSPARENT,true);
  funSprite->drawSmoothArc(120,120,94,90,startAngl,angl3,COLOR_BROWN,TFT_TRANSPARENT,true);

  funSprite->drawSmoothArc(120,120,118,110,180+40,360-40,0x5D83,TFT_TRANSPARENT,true);
  funSprite->drawSmoothArc(120,120,106,102,180+40,360-40,0x3F29,TFT_TRANSPARENT,true);
  
  if(doAnimate||1)
  {
    int iPlant =(frameCounter/(100/plantArrLen))%plantArrLen;
  //funSprite->pushImage(60,60,120,120, plantArray[iPlant]);
  funSprite->drawFastHLine(0,120,35,TFT_WHITE);
  funSprite->setCursor(80,20);
  funSprite->printf("Plant %d",iPlant);

  accsrySprite->setSwapBytes(0);
  accsrySprite->fillScreen(TFT_BLACK);
  funSprite->pushImage(60,60,120,120, plantArray[iPlant]);

  int animate = (frameCounter%10)/3;
  accsrySprite->pushImage(40+animate,30-animate,50,10,icon_sunglasses);
  accsrySprite->pushToSprite(funSprite,60,60,TFT_BLACK);

  }
  funSprite->pushSprite(0,0);
  //funSprite->pushSprite(0,0);
  val1++;
  val2+=2;
  val3+=3;
  

  return false; //bleibe im Menu



}

bool myFunc(TFT_eSprite * funSprite,TFT_eSprite * textSpr,TFT_eSprite * accsrySprite)
{
   
  uint16_t angly = map(gyroy,-18000, 18000,41,319 );
  uint16_t anglp = map(gyrop,-18000, 18000,41,319 );
  uint16_t anglr = map(gyror,-18000, 18000,41,319 );
  
  Serial.print(angly/100);

  funSprite->fillScreen(TFT_BLACK);
  funSprite->loadFont(NotoSansMonoSCB20);

  funSprite->setCursor(80,100);
  funSprite->setTextColor(TFT_GOLD);
  funSprite->print("Yaw: ");
  funSprite->print(gyroy/100);

  funSprite->setCursor(80,120);
  funSprite->setTextColor(TFT_SILVER);
  funSprite->print("Pitch: ");
  funSprite->print(gyrop/100);
  
  funSprite->setCursor(80,140);
  funSprite->setTextColor(TFT_VIOLET);
  funSprite->print("Roll: ");
  funSprite->print(gyror/100);
  

  funSprite->drawSmoothArc(120,120,118,114,40,angly,TFT_GOLD,TFT_TRANSPARENT,true);
  funSprite->drawSmoothArc(120,120,110,106,40,anglp,TFT_SILVER,TFT_TRANSPARENT,true);
  funSprite->drawSmoothArc(120,120,102,98,40,anglr,TFT_VIOLET,TFT_TRANSPARENT,true);
  
  funSprite->pushSprite(0,0);
  return 0; // Bleibe im Menu
}

bool startAP(TFT_eSprite * funSprite,TFT_eSprite * textSpr,TFT_eSprite * accsrySprite)
{
 wirelessManager.startAP();
  return true;
}

bool getMAC(TFT_eSprite * funSprite,TFT_eSprite * textSpr,TFT_eSprite * accsrySprite)
{
  wirelessManager.getConnectedMacAddresses();
  return true;
}

bool stopAP(TFT_eSprite * funSprite,TFT_eSprite * textSpr,TFT_eSprite * accsrySprite)
{
  wirelessManager.stopAP();
  return true;
}

void sendData()
{
//wirelessManager.sendESPNOW();


}

bool espnowConfig(TFT_eSprite * funSprite,TFT_eSprite * textSpr,TFT_eSprite * accsrySprite)
{


  textSpr->loadFont(NotoSansBold15);
  funSprite->fillScreen(0);
  textSpr->fillScreen(0);
  textSpr->setCursor(0,0);
  //textSpr-> setCursor(30,0);
  textSpr->println("MAC-Address:");
  textSpr->println(WiFi.macAddress());

  std::vector<MacAddress> list;
  list = wirelessManager.getLastDevices();

  for (size_t i = 0; i < list.size(); i++)
  {

  textSpr->printf("Device: %d\n%02X:%02X:%02X:%02X:%02X:%02X\n",
                      i,
                      list[i].mac[0],
                      list[i].mac[1],
                      list[i].mac[2],
                      list[i].mac[3],
                      list[i].mac[4],
                      list[i].mac[5]);

  }

  textSpr->pushToSprite(funSprite,40,40);
  funSprite->pushSprite(0,0);

  static struct_message msg;
  msg.id ++;
    if (list.size()>0)
  {
    uint8_t addr[] ={0x48,0x27,0xE2,0x4B,0x35,0x8A};
  wirelessManager.initESPNOW(true);
  wirelessManager.sendESPNOW(list[0].mac);  }  

/*


     esp_err_t result = esp_now_send(list[0].mac, (uint8_t*)&msg, sizeof(msg));
    Serial.printf("Sendig: %d", msg.id);
    if (result == ESP_OK)
      Serial.print("OK");
    else
      Serial.print("Fail");


  }  
  */   
 


  delay(200);

  return false;

}

bool restartESP(TFT_eSprite * funSprite, TFT_eSprite * txtSprite,TFT_eSprite * accsrySprite)
{
  
  funSprite->fillScreen(TFT_BLACK);
  

  txtSprite->fillScreen(TFT_BLACK);
  txtSprite->setTextColor(TFT_RED);
  txtSprite->setCursor(0,0);
  txtSprite->print("Restarting System...");
  txtSprite->pushToSprite(funSprite,60,100);
  funSprite->pushSprite(0,0);



  ESP.restart();
  return 0;

}



/*########################################## Menu Structure ##################################################*/
char* macToString(const uint8_t* mac) {
    char macStr[18];
    snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    return macStr;
}


std::vector<omegaTFT> macMenu(void)
{
  Serial.println("MACMENU");

  std::vector<omegaTFT> items;
  items.push_back(omegaTFT(EXIT, "Back",icon_cross));

  std::vector<MacAddress> macList = wirelessManager.getConnectedMacAddresses();
  uint8_t i = 0; 
      for (const MacAddress&mac : macList) {
        items.push_back(omegaTFT(VALUE,"Device:",icon_numeric,macList[i].mac[5]));
        i++;

    }
    
 return items;

}






omegaTFT wifiMenu[]{
omegaTFT(FUNCTION,"Start AP", icon_satellite_antenna,startAP),
omegaTFT(FUNCTION,"Stop AP", icon_satellite_antenna,stopAP),
omegaTFT(MENU_FUNCTION,"Get MAC",icon_satellite_antenna,macMenu),
omegaTFT(EXIT,"Back")


};

omegaTFT settingsMenu [] = {
  omegaTFT(SUBMENU,"WiFi", icon_wifi,wifiMenu,sizeof(wifiMenu)/sizeof(omegaTFT)),
  omegaTFT(FUNCTION,"ESP-NOW", icon_satellite_antenna,espnowConfig),
  omegaTFT(EMPTY,"BLE", icon_bluetooth),
  omegaTFT(EXIT,"Back")
};

omegaTFT submenus [] = {
  omegaTFT(FUNCTION, "Home Screen",icon_potted_plant,drawHomeScreen),
  omegaTFT(SUBMENU, "Settings",icon_gear,settingsMenu, sizeof(settingsMenu)/sizeof(omegaTFT)),
  omegaTFT(FUNCTION,"Restart Device",icon_cross,restartESP)
};

omegaTFT myMenu = omegaTFT(SUBMENU,"Main Menu",nullptr,submenus,sizeof(submenus)/sizeof(omegaTFT));

#endif