
#include <upp11.h>

using namespace std;

UP_TEST(test000a)
{
	UP_FAIL(__PRETTY_FUNCTION__);
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
	UP_FAIL(__PRETTY_FUNCTION__);
}

int main(int, char **)
{
	UP_RUN_SHUFFLED(time(0));
}
