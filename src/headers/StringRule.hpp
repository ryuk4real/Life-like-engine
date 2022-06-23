#include <string>
#include <vector>

#include <stdexcept>
#include <regex>

using namespace std;


class StringRule
{
    private:

        vector<bool> bornSubset;
        vector<bool> survivorSubset;

        string stringRule;

    public:

        StringRule(string &_stringRule);
        StringRule(){}

        string getString() const {return stringRule;}

        bool getNeighbourFromBornSubsetAt(int &position) const;
        bool getNeighbourFromSurvivorSubsetAt(int &position) const;

};


StringRule::StringRule(string &_stringRule)
{
    
    const regex ruleDetection("^B((?![0-8]*?([0-8])[0-8]*?\\2)[0-8]{0,10})\\/S((?![0-8]*?([0-8])[0-8]*?\\4)[0-8]{0,10})");
    smatch matches;
    
    if (regex_search(_stringRule, matches, ruleDetection))
    {
        this->stringRule = _stringRule;


        this->bornSubset = vector<bool>(9, 0);
        string bornSubString = matches.str(1);

        for (int i = 0; i < bornSubString.size(); ++i)
        {
            bornSubset[bornSubString[i] - 48] = 1;
        }


        this->survivorSubset = vector<bool>(9, 0);
        string survivorSubString = matches.str(3);

        for (int i = 0; i < survivorSubString.size(); ++i)
        {
            survivorSubset[survivorSubString[i] - 48] = 1;
        }

        cout<<endl;
    }
    
    else throw runtime_error("ERROR: can't parse stringRule in file settings.json");
}

bool StringRule::getNeighbourFromBornSubsetAt(int &position) const
{
    return bornSubset[position];
}

bool StringRule::getNeighbourFromSurvivorSubsetAt(int &position) const
{
    return survivorSubset[position];
}