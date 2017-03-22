#ifndef TEST
#define TEST
using namespace std;

class Test
{
public:
    Test();
    ~Test();
    int test();
private:
    int number_of_calls_;
};
#endif