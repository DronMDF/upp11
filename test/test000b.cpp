
#include <string>
#include <upp11.h>

using namespace std;

UP_SUITE_BEGIN(suiteParametrized)

const auto params = {
	make_tuple(1, "x"),
	make_tuple(2, "xx"),
	make_tuple(3, "xxx")
};

UP_PARAMETRIZED_TEST(test000bp, params, int, const char *)
{
	cout << __PRETTY_FUNCTION__ << " " << get<0>(params) << " " << get<1>(params) << endl;
}

struct fixture {
	fixture() {
		cout << "setUp parametrized" << endl;
	}
	~fixture() {
		cout << "tearDown parametrized" << endl;
	}
};

UP_FIXTURE_PARAMETRIZED_TEST(test000bf, fixture, params, int, const char *)
{
	cout << __PRETTY_FUNCTION__ << " " << get<0>(params) << " " << get<1>(params) << endl;
}

UP_SUITE_END()
