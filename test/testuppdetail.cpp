
#include <limits>
#include <upp11.h>

using namespace std;
using namespace upp11;

UP_SUITE_BEGIN(suteTestEqual)

UP_TEST(isEqualShouldCompareOtherTypes)
{
	TestEqual base;
	UP_ASSERT(!base.isEqual(1, 0));
	UP_ASSERT(!base.isEqual(1, "0"));
	UP_ASSERT(!base.isEqual("te", "next"));
	UP_ASSERT(base.isEqual(100, 100));
	UP_ASSERT(base.isEqual("last", "last"));

	UP_ASSERT(base.isEqual(list<int>{ 1, 2, 3, 4, 5 }, vector<int>{ 1, 2, 3, 4, 5 }));
	UP_ASSERT(base.isEqual(list<string>{ "one", "two", "free" }, vector<string>{ "one", "two", "free" }));
}

UP_TEST(isEqualShouldCompareOtherSignValues)
{
	TestEqual base;
	UP_ASSERT(base.isEqual(1, 1U));
	UP_ASSERT(!base.isEqual(-1, numeric_limits<unsigned>::max()));
	UP_ASSERT(base.isEqual(0U, 0));
	UP_ASSERT(base.isEqual(127U, numeric_limits<char>::max()));
	UP_ASSERT(base.isEqual(numeric_limits<char>::min(), -128));
	UP_ASSERT(base.isEqual(numeric_limits<uint8_t>::max(), 255));
}

UP_SUITE_END()
