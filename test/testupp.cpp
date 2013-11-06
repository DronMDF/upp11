
#include <upp11.h>

using namespace std;

UP_SUITE_BEGIN(suiteParametrized)

const auto numbers = {
	make_tuple(1, "1"),
	make_tuple(1000000, "1000000"),
	make_tuple(-1000000, "-1000000"),
	make_tuple(9, "9")
};

UP_PARAMETRIZED_TEST(numderShouldConvertToString, numbers, int, const char *)
{
	const int n = get<0>(numbers);
	const string ns = get<1>(numbers);

	ostringstream s;
	s << n;
	UP_ASSERT_EQUAL(s.str(), ns);
}

UP_SUITE_END()

UP_SUITE_BEGIN(suiteEqual)

void CUSTOM_EQUAL_ASSERTION()
{
	UP_ASSERT_EQUAL(-100, -100);
}

UP_TEST(AssertEqualShouldCompareAnyTypes)
{
	UP_ASSERT_EQUAL(100, 100);
	UP_ASSERT_EQUAL("last", "last");
	UP_ASSERT_EQUAL(string("last"), "last");

	const auto cvalue = { 1, 2, 3, 4, 5 };
	const initializer_list<int> ivalue = { 1, 2, 3, 4, 5 };
	const list<int> lvalue = { 1, 2, 3, 4, 5 };
	const vector<int> vvalue = { 1, 2, 3, 4, 5 };
	const array<int, 5> avalue = { 1, 2, 3, 4, 5 };
	const int rvalue[] = { 1, 2, 3, 4, 5 };
	UP_ASSERT_EQUAL(lvalue, lvalue);
	UP_ASSERT_EQUAL(cvalue, lvalue);
	UP_ASSERT_EQUAL(vvalue, lvalue);
	UP_ASSERT_EQUAL(lvalue, ivalue);
	UP_ASSERT_EQUAL(avalue, vvalue);
	UP_ASSERT_EQUAL(rvalue, lvalue);

	const list<string> lstring = { "one", "two", "free" };
	const vector<string> vstring = { "one", "two", "free" };
	UP_ASSERT_EQUAL(lstring, vstring);

	CUSTOM_EQUAL_ASSERTION();
}

UP_TEST(AssertNeShouldCompareAnyTypes)
{
	UP_ASSERT_NE(1, 0);
	UP_ASSERT_NE(0, "null");
	UP_ASSERT_NE("te", "next");
	UP_ASSERT_NE(-1, numeric_limits<unsigned>::max());
}

UP_SUITE_END()

UP_SUITE_BEGIN(suiteExceptions)

UP_TEST(AssertShouldCheckExceptionsByMessage)
{
	UP_ASSERT_EXCEPTION(runtime_error, "exception message", []{
		throw runtime_error("exception message");
	});
	UP_ASSERT_EXCEPTION(logic_error, []{
		throw logic_error("");
	});
}

UP_SUITE_END()

UP_MAIN()
