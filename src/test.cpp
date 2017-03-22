#include "test.h"
#include <iostream>

using namespace std;
class Test;
Test::Test()
{
    number_of_calls_=0;
}
int Test::test()
{
    number_of_calls_++;
    cout << "Number of test calls: " << number_of_calls_ << endl;
    return number_of_calls_;
}