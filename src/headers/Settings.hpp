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

        bool areSettingsDefault;

        int numberOfGenerations;
        rgb generationTextColor;
        int generationTypefaceSize;

        int displaySize;
        int matrixSize;

        rgb aliveCellColor;
        rgb deadCellColor;

        int millisecondsToWaitForEachGeneration;

        StringRule stringRule;

    public:



        Settings();

        // Getters -------------------------------------------------------------------------------------
        int getNumberOfGenerations() const {return this->numberOfGenerations;}
        rgb getGenerationTextColor() const {return this->generationTextColor;}
        int getGenerationTextTypefaceSize() const {return this->generationTypefaceSize;}

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


    numberOfGenerations = checkPositive(jsonSettings["Settings"]["numberOfGenerations"]);

    generationTextColor.r = checkRGBValue(jsonSettings["Settings"]["generationTextColor"][0]);
    generationTextColor.g = checkRGBValue(jsonSettings["Settings"]["generationTextColor"][1]);
    generationTextColor.b = checkRGBValue(jsonSettings["Settings"]["generationTextColor"][2]);

    generationTypefaceSize = checkPositive(jsonSettings["Settings"]["generationTypefaceSize"]);

    displaySize = checkPositive(jsonSettings["Settings"]["displaySize"]);

    matrixSize = checkPositive(jsonSettings["Settings"]["matrixSize"]);

    aliveCellColor.r = checkRGBValue(jsonSettings["Settings"]["aliveCellColor"][0]);
    aliveCellColor.g = checkRGBValue(jsonSettings["Settings"]["aliveCellColor"][1]);
    aliveCellColor.b = checkRGBValue(jsonSettings["Settings"]["aliveCellColor"][2]);

    deadCellColor.r = checkRGBValue(jsonSettings["Settings"]["deadCellColor"][0]);
    deadCellColor.g = checkRGBValue(jsonSettings["Settings"]["deadCellColor"][1]);
    deadCellColor.b = checkRGBValue(jsonSettings["Settings"]["deadCellColor"][2]);

    millisecondsToWaitForEachGeneration = checkPositive(jsonSettings["Settings"]["millisecondsToWaitForEachGeneration"]);

    string s = jsonSettings["Settings"]["stringRule"];
    cout << s << endl;
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