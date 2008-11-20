#include "DayTime.hpp"
#include "CommonTime.hpp"
#include <iostream>


using namespace gpstk;
using namespace std;


int main()
{
CommonTime common = CommonTime();
DayTime day = DayTime();

day = common;
common = day;

day = CommonTime();
common = DayTime();

return 0;
}
