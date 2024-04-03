#include "hx_simple_test_driver.h"

int main(int argc, char* argv[])
{
    CHXSimpleTestDriver testDriver;

    int ret = -1;
    if (argc > 1)
    {
	if (testDriver.Run(argc, argv))
	    ret = 0;
    }
    else if (testDriver.Run())
	ret = 0;

    return ret;
}
