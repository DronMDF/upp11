
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

UP_SUITE_BEGIN(suiteOutput)

UP_FIXTURE_TEST(PrimitiveOutputTest, TestPrinter)
{
	UP_ASSERT_EQUAL(printable(1), "1");
	UP_ASSERT_EQUAL(printable(1LLU), "1");
	UP_ASSERT_EQUAL(printable(true), "true");
}

const auto int_values = {
	make_pair(-1, "-1"),
	make_pair(0, "0"),
	make_pair(1000000000, "1000000000"),
	make_pair(-2000000000, "-2000000000")
};

UP_FIXTURE_PARAMETRIZED_TEST(shouldOutIntScalar, TestPrinter, int_values)
{
	UP_ASSERT_EQUAL(printable(get<0>(int_values)), get<1>(int_values));
}

const auto vint_values = {
	make_pair(list<int>{-1}, "{ -1 }"),
	make_pair(list<int>{0}, "{ 0 }"),
	make_pair(list<int>{1000000000}, "{ 1000000000 }"),
	make_pair(list<int>{-2000000000}, "{ -2000000000 }"),
	make_pair(list<int>{-1, 1, 5, 100}, "{ -1, 1, 5, 100 }")
};

UP_FIXTURE_PARAMETRIZED_TEST(shouldOutIntCollection, TestPrinter, vint_values)
{
	UP_ASSERT_EQUAL(printable(get<0>(vint_values)), get<1>(vint_values));
}

UP_FIXTURE_TEST(StringShouldQuoted, TestPrinter)
{
	UP_ASSERT_EQUAL(printable("const char *"), "\"const char *\"");
	UP_ASSERT_EQUAL(printable(string("string")), "\"string\"");
}

UP_FIXTURE_TEST(PairShouldColonSeparated, TestPrinter)
{
	UP_ASSERT_EQUAL(printable(make_pair(0, 1)), "0, 1");
	UP_ASSERT_EQUAL(printable(make_tuple(0, 1, "aaa")), "0, 1, \"aaa\"");
}

UP_SUITE_END()
