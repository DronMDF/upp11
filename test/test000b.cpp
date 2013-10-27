
#include <string>
#include <upp11.h>

using namespace std;

UP_TEST(test000b)
{
	UP_FAIL(__PRETTY_FUNCTION__);
}

const auto params = {
	make_tuple(1, "x"),
	make_tuple(2, "xx"),
	make_tuple(3, "xxx")
};

UP_PARAMETRIZED_TEST(test000bp, params, int, const char *)
{
	UP_FAIL(__PRETTY_FUNCTION__);
	cout << get<0>(params) << " " << get<1>(params) << endl;
}
