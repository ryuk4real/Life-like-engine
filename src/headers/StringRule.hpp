#include <string>
#include <vector>

#include <stdexcept>
#include <regex>

using namespace std;


class StringRule
{
    private:


        vector<int> bornSubset;
        vector<int> survivorSubset;

        string stringRule;

        bool parseStringRule(string s);


    public:

        StringRule(string _stringRule);
        StringRule(){}

        string getString() const {return stringRule;}


};


StringRule::StringRule(string _stringRule)
{

    regex ruleDetection("^B((?!\\d*?(\\d)\\d*?\2)\\d{0,10})\\/S((?!\\d*?(\\d)\\d*?\4)\\d{0,10})");
    smatch matches;
    
    if (regex_search(_stringRule, matches, ruleDetection))
    {
        this->stringRule = _stringRule;
        this->bornSubset = vector<int>(10, -1);
        this->survivorSubset = vector<int>(10, -1);
    }
    else throw runtime_error("ERROR: can't parse stringRule in file settings.json");
}