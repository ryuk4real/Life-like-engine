#include <string>
#include <vector>

using namespace std;


class StringRule
{
    private:

        string stringRule;

        vector<int> bornSubset;
        vector<int> survivorSubset;



    public:

        StringRule(string stringRule);


};


StringRule::StringRule(string stringRule)
{
    this->stringRule = stringRule;
    
    this->bornSubset = vector<int>(10, -1);
    this->survivorSubset = vector<int>(10, -1);
}