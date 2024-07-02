#ifndef OMEGAPLANT_H
#define OMEGAPLANT_H

#include <Arduino.h>

#include <nvs.h>
#include <nvs_flash.h>

/*
    Settings
*/

#define MEASUREMENT_PER_XP_GAIN 24
#define MEASUREMENT_INERVAL_SEC 20
#define MAX_PLANTS 5
/*
    End Settings
*/


enum RCMD{
    ALL_FINE = 0,
    TOO_HOT,
    TOO_COLD,
    TOO_HUMID,
    TOO_DRY,
    TOO_SUNNY,
    TOO_DARK,
    TOO_MOIST,
    TOO_ARID,
}



enum Unlockables{
    ULCK_NONE = 0x1FFF,

//Items:
    ULCK_SUNGLASSES = 1 << 0,
    ULCK_GLASSES = 1 << 1,
    ULCK_BALOON = 1 << 2,
    ULCK_BEARD = 1 << 3,
    ULCK_TIE = 1 << 4,
    ULCK_TIE2 = 1 << 5,
    ULCK_CROWN = 1 << 6,
    ULCK_HAT = 1 << 7,
//Avatars
    ULCK_DEFAULTPLANT = 1<<8,
    ULCK_CACTUS1 = 1<<9,
    ULCK_VASE1 = 1<<10,
    
    //1<<11
    //1<<12
    //1<<13

//Backgrounds    
    ULCK_BG1 = 1 << 14,
    ULCK_BG2 = 1 << 15,
    

    ULCK_ALL = 0xFFFF // 10 bits for 10 items
};

struct PlantProfile
{
    char name[16] ={'P','l','a','n','t'};

    uint8_t tempc =22;
    uint8_t hum = 60;
    uint8_t soil_moisture = 50;
    unsigned int light = 10000;
   
};

struct sensorData{
    uint8_t temperature;
    uint8_t humidity;
    uint8_t moisture;
    uint8_t lightIntensity;
};




struct PlantSaveData{
    uint8_t plantID = 0;

    PlantProfile savedProfile;
    
    uint16_t savedExp= 0;
    uint16_t unlockedItems= 0;
    uint16_t unlockedBg= 0;
    uint16_t unlockedAvatar = 0;

};

class omegaPlant{
private:
    PlantSaveData myCurrentState;

    sensorData lastData[MEASUREMENT_PER_XP_GAIN]; // Save The last Day
    uint8_t moodHistory[MEASUREMENT_PER_XP_GAIN];
    sensorData currentData;
    
    uint8_t plantID; // Memory Location id for managing savestates
    uint8_t currentLevel;
    uint32_t myExp;
    


public:
    
    void getMeasurement(sensorData);                    //1. Updates the Plant State 
    uint8_t calculateMood(sensorData);                  //2. Compairsion to PlantProfile
    uint8_t calculateXP();                              //3. Decides When to award xp
    uint8_t gainXP(uint8_t);                            //4. Increase current xp and returns it
    uint8_t calculateLevel(uint32_t);                   //5. returns Current level
    bool isLevelUp();                                      //Checks current xp. if level up availabe set new level and returns true
    uint8_t getRCMD(sensorData);                        //Returns a reccomendations based on current sensor data

    PlantSaveData generateSaveData(omegaPlant*);         //Generate save file for current instance
    bool saveMyState(uint8_t);                      //Writes the SafeData into memory
    bool LoadFromMemory(uint8_t,PlantSaveData*);
    omegaPlant(uint8_t);                                //Automatically  

    omegaPlant(PlantSaveData); // If savedata is available
    omegaPlant(PlantProfile); // Create new Plant 
    
    ~omegaPlant();
};

omegaPlant::omegaPlant(PlantProfile extPlantProfile){myCurrentState.savedProfile = extPlantProfile;}

omegaPlant::omegaPlant(PlantSaveData extSaveData): myCurrentState(extSaveData){}

omegaPlant::omegaPlant(uint8_t index)
{
    PlantSaveData data;
    LoadFromMemory(index,&data); //Stores the read memory;
    


}
omegaPlant::~omegaPlant()
{
}

void omegaPlant::getMeasurement(sensorData newData){
    static int measureCounter =0;
    uint8_t rcmnd =0;

    measureCounter++;

    uint8_t newMood = calculateMood(newData);
    updateMoodHistory(newMood);

    if (newMood<90)
    {   
        // Calculate Recommendations
        rcmnd = getRCMD(newData);

    }

    if (measureCounter%MEASUREMENT_PER_XP_GAIN == 0)
    {
        uint8_t rewardedXP = calculateXP();
        
        uint32_t currentXP = gainXP(rewardedXP);
        uint8_t currentLevel = calculateLevel(currentXP);
        
        // On LevelUP
        if(isLevelUp()){
            //Unlock stuff;

        
        } 
    }

}

uint8_t omegaPlant::calculateMood(sensorData currentData){

        uint8_t mood = 100; // Start with 100% mood
        uint8_t range_temp = 5;
        uint8_t range_hum = 30
        uint32_t range_light = 100;
        uint8_t range_soil_moisture = 100;

        // Check Temperature
        if (currentData.temperature < myProfile.min_tempc - range_temp / 4 || currentData.temperature > myProfile.max_tempc + range_temp / 4)
        {
            mood -= 25;
        }
        else if (currentData.temperature < myProfile.min_tempc)
        {
            mood -= (myProfile.min_tempc - currentData.temperature) * 25 / (range_temp / 4);
        }
        else if (currentData.temperature > myProfile.max_tempc)
        {
            mood -= (currentData.temperature - myProfile.max_tempc) * 25 / (range_temp / 4);
        }

        // Check Humidity
        if (currentData.humidity < myProfile.min_hum - range_hum / 4 || currentData.humidity > myProfile.max_hum + range_hum / 4)
        {
            mood -= 25;
        }
        else if (currentData.humidity < myProfile.min_hum)
        {
            mood -= (myProfile.min_hum - currentData.humidity) * 25 / (range_hum / 4);
        }
        else if (currentData.humidity > myProfile.max_hum)
        {
            mood -= (currentData.humidity - myProfile.max_hum) * 25 / (range_hum / 4);
        }

        // Check Light
        if (currentData.lightIntensity < myProfile.min_light - range_light / 4 || currentData.lightIntensity > myProfile.max_light + range_light / 4)
        {
            mood -= 25;
        }
        else if (currentData.lightIntensity < myProfile.min_light)
        {
            mood -= (myProfile.min_light - currentData.lightIntensity) * 25 / (range_light / 4);
        }
        else if (currentData.lightIntensity > myProfile.max_light)
        {
            mood -= (currentData.lightIntensity - myProfile.max_light) * 25 / (range_light / 4);
        }

        // Check Soil Moisture
        if (currentData.moisture < myProfile.min_soil_moisture - range_soil_moisture / 4 || currentData.moisture > myProfile.max_soil_moisture + range_soil_moisture / 4)
        {
            mood -= 25;
        }
        else if (currentData.moisture < myProfile.min_soil_moisture)
        {
            mood -= (myProfile.min_soil_moisture - currentData.moisture) * 25 / (range_soil_moisture / 4);
        }
        else if (currentData.moisture > myProfile.max_soil_moisture)
        {
            mood -= (currentData.moisture - myProfile.max_soil_moisture) * 25 / (range_soil_moisture / 4);
        }

        // Ensure mood is not less than 0
        if (mood > 100) // Checking for overflow case
        {
            mood = 0;
        }

        return mood; // Mood in percentage (0-100%)
    } 

void omegaPlant::updateMoodHistory(uint8_t newMood) {
  static uint8_t moodHistorySize;
  if (moodHistorySize < MEASUREMENT_PER_XP_GAIN) {
        moodHistory[moodHistorySize++] = newMood;
    } else {
        // Shift the history array to the left and add the new mood at the end
        for (uint8_t i = 1; i < MEASUREMENT_PER_XP_GAIN; ++i) {
            moodHistory[i - 1] = moodHistory[i];
        }
        moodHistory[MEASUREMENT_PER_XP_GAIN - 1] = newMood;
    }
}

uint8_t omegaPlant::calculateLevel(uint32_t exp) {
    uint8_t level = 1;
    uint32_t exp_needed = base_exp;

    while (exp >= exp_needed) {
        exp -= exp_needed;
        level++;
        exp_needed = 2*level;
    }

    return level;
}

uint8_t omegaPlant::getRCMD(sensorData newData){

        // Check temperature
    if (newData.temperature < myProfile.min_tempc)
        return TOO_COLD;
    if (newData.temperature > myProfile.max_tempc)
        return TOO_HOT;

    // Check humidity
    if (newData.humidity < myProfile.min_hum)
        return TOO_DRY;
    if (newData.humidity > myProfile.max_hum)
        return TOO_HUMID;

    // Check soil moisture
    if (newData.moisture < myProfile.min_soil_moisture)
        return TOO_ARID;
    if (newData.moisture > myProfile.max_soil_moisture)
        return TOO_MOIST;

    // Check light intensity
    if (newData.lightIntensity < myProfile.min_light)
        return TOO_DARK;
    if (newData.lightIntensity > myProfile.max_light)
        return TOO_SUNNY;

    // If all conditions are within optimal ranges
    return ALL_FINE;
}


uint8_t omegaPlant::calculateXP(){
    uint8_t baseXP = 3;
    uint16_t accMood = 0;

    for (size_t i = 0; i < MEASUREMENT_PER_XP_GAIN; i++)
    {
        accMood+=moodHistory[i];
    }

    return baseXP * (accMood/MEASUREMENT_PER_XP_GAIN)/100;

}

uint8_t omegaPlant::gainXP(uint8_t gainedXP){
    myCurrentState.savedExp += gainedXP;

    return myCurrentState.savedExp;

}

bool omegaPlant::isLevelUp(){
    

    static uint8_t level = 0;

    uint8_t newLevel = calculateLevel(myCurrentState.savedExp);

    if (newLevel > level){
    
        level = newLevel;
        return true;
    }
    else{
        level = newLevel;
      return false;   
    }

 

}   

bool omegaPlant::saveMyState(uint8_t index){
    
    PlantSaveData mydata = generateSaveData(this);
    PlantSaveData * data = &mydata; 
    
    if(!data) return;


    if (index < 0 || index >= MAX_PLANTS) {
        return ESP_ERR_INVALID_ARG;
    }

    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK) {
        return err;
    }

    char key[16];
    snprintf(key, sizeof(key), "plant%d", index);
    err = nvs_set_blob(nvs_handle, key, data, sizeof(PlantSaveData));
    if (err != ESP_OK) {
        nvs_close(nvs_handle);
        return err;
    }

    err = nvs_commit(nvs_handle);
    nvs_close(nvs_handle);
    return err;

}

bool omegaPlant::LoadFromMemory(uint8_t index,PlantSaveData * data)
{

    if (index < 0 || index >= MAX_PLANTS || data == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }

    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open("storage", NVS_READONLY, &nvs_handle);
    if (err != ESP_OK) {
        return err;
    }

    char key[16];
    snprintf(key, sizeof(key), "plant%d", index);
    size_t required_size = sizeof(PlantSaveData);
    err = nvs_get_blob(nvs_handle, key, data, &required_size);
    nvs_close(nvs_handle);
    return err;
}
#endif
