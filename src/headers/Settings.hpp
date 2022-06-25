#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include <fstream> //ifstream
#include <iostream> //cout

#include "StringRule.hpp" // parsing the string rule
#include "json.hpp" // parsing json file
using json = nlohmann::json;


using namespace std;

struct rgb{int r; int g; int b;};

class Settings
{

    private:

        bool randomizedFirstGeneration;

        int numberOfGenerations;
        rgb generationTextColor;
        int generationTypefaceSize;
        bool displayGenerationOnScreen;

        int displaySize;
        int matrixSize;

        rgb aliveCellColor;
        rgb deadCellColor;

        int millisecondsToWaitForEachGeneration;

        StringRule stringRule;
        Settings();

    public:


        // Getters -------------------------------------------------------------------------------------
        bool isFirstGenerationRandomized() const {return this->randomizedFirstGeneration;}

        int getNumberOfGenerations() const {return this->numberOfGenerations;}
        rgb getGenerationTextColor() const {return this->generationTextColor;}
        int getGenerationTextTypefaceSize() const {return this->generationTypefaceSize;}

        bool areGenerationsDisplayedOnScreen() const {return this->displayGenerationOnScreen;}

        int getDisplaySize() const {return this->displaySize;}
        int getMatrixSize() const {return this->matrixSize;}

        rgb getAliveCellColor() const {return this->aliveCellColor;}
        rgb getDeadCellColor() const {return this->deadCellColor;}
        int getMillisecodsToWaitForEachGeneration() const {return this->millisecondsToWaitForEachGeneration;}

        StringRule getStringRule() const {return this->stringRule;}
        // ----------------------------------------------------------------------------------------------

        //Utilities
        inline int checkRGBValue(int value) const;
        inline int checkPositive(int value) const;

};

Settings::Settings()
{

    // Read json settings file ----------------------------------------------------------------------------
    ifstream settings("src/settings/settings.json");

    if(!settings){ throw runtime_error("ERROR: Couldn't open/find settings.json file"); }
    
    json jsonSettings;
    settings >> jsonSettings;
    //----------------------------------------------------------------------------------------------------

    randomizedFirstGeneration = checkPositive(jsonSettings["isFirstGenerationRandom"]);

    numberOfGenerations = checkPositive(jsonSettings["numberOfGenerations"]);

    generationTextColor.r = checkRGBValue(jsonSettings["generationTextColor"][0]);
    generationTextColor.g = checkRGBValue(jsonSettings["generationTextColor"][1]);
    generationTextColor.b = checkRGBValue(jsonSettings["generationTextColor"][2]);

    generationTypefaceSize = checkPositive(jsonSettings["generationTypefaceSize"]);

    displayGenerationOnScreen = checkPositive(jsonSettings["displayGenerationsOnScreen"]);

    displaySize = checkPositive(jsonSettings["displaySize"]);

    matrixSize = checkPositive(jsonSettings["matrixSize"]);

    aliveCellColor.r = checkRGBValue(jsonSettings["aliveCellColor"][0]);
    aliveCellColor.g = checkRGBValue(jsonSettings["aliveCellColor"][1]);
    aliveCellColor.b = checkRGBValue(jsonSettings["aliveCellColor"][2]);

    deadCellColor.r = checkRGBValue(jsonSettings["deadCellColor"][0]);
    deadCellColor.g = checkRGBValue(jsonSettings["deadCellColor"][1]);
    deadCellColor.b = checkRGBValue(jsonSettings["deadCellColor"][2]);

    millisecondsToWaitForEachGeneration = checkPositive(jsonSettings["millisecondsToWaitForEachGeneration"]);

    string s = jsonSettings["stringRule"];
    stringRule = StringRule(s);

}

inline int Settings::checkRGBValue(int value) const
{
    if (value >= 0 && value <= 255) { return value; }

    throw range_error("ERROR: RGB values must be 0 <= value <= 255");
}

inline int Settings::checkPositive(int value) const
{
    if (value >= 0) return value;

    throw range_error("ERROR: Passed a negative value, positive expected.");
}

#endif