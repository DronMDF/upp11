
#include <upp11.h>

using namespace std;

UP_SUITE_BEGIN(suiteTrivial)

UP_TEST(test000a)
{
	cout << __PRETTY_FUNCTION__ << endl;
}

struct fixture {
	fixture() {
		cout << "setUp" << endl;
	}
	~fixture() {
		cout << "tearDown" << endl;
	}
};

UP_FIXTURE_TEST(test000af, fixture)
{
	cout << __PRETTY_FUNCTION__ << endl;
}

UP_SUITE_END()

UP_MAIN();
