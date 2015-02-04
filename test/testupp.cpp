
#include <map>
#include <assert.h>
#include <upp11.h>

using namespace std;

UP_SUITE_BEGIN(suiteParametrized)

const auto numbers = {
	make_tuple(1, "1"),
	make_tuple(1000000, "1000000"),
	make_tuple(-1000000, "-1000000"),
	make_tuple(9, "9")
};

UP_PARAMETRIZED_TEST(numderShouldConvertToString, numbers)
{
	const int n = get<0>(numbers);
	const string ns = get<1>(numbers);

	ostringstream s;
	s << n;
	UP_ASSERT_EQUAL(s.str(), ns);
}

const auto lens = {
	make_pair(1, "1"),
	make_pair(2, "10"),
	make_pair(7, "1000000"),
	make_pair(1, "x")
};

UP_PARAMETRIZED_TEST(stringLenShouldDetermine, lens)
{
	const string::size_type n = lens.first;
	const string ns = lens.second;
	UP_ASSERT_EQUAL(n, ns.size());
}

const auto prime = { 3, 5, 7, 13 };

// Fixture is not only for setUp/tearDown, but for help functions
struct prime_checker {
	virtual ~prime_checker() = default;
	bool isPrime(int p) {
		for (int n = 2; n < p; n++) {
			if (p % n == 0) {
				return false;
			}
		}
		return true;
	}
};

UP_FIXTURE_PARAMETRIZED_TEST(numbersShouldBePrime, prime_checker, prime)
{
	UP_ASSERT(isPrime(prime));
}

const auto collect = {
	list<int>{ 0, 1, 2, 3 },
	list<int>{ 2, 4 },
	list<int>{ 6 }
};

UP_PARAMETRIZED_TEST(collectSumShouldBe6, collect)
{
	UP_ASSERT_EQUAL(accumulate(collect.begin(), collect.end(), 0), 6);
}

UP_SUITE_END()

UP_SUITE_BEGIN(suiteEqual)

UP_TEST(AssertShouldIgnoreComma)
{
	UP_ASSERT(map<int, int>().empty());
}

void CUSTOM_EQUAL_ASSERTION()
{
	UP_ASSERT_EQUAL(-100, -100);
}

UP_TEST(AssertEqualShouldCompareAnyTypes)
{
	UP_ASSERT_NE(-1, numeric_limits<unsigned>::max());
	UP_ASSERT_EQUAL(100, 100);
	UP_ASSERT_EQUAL("last", "last");
	UP_ASSERT_EQUAL(string("last"), "last");
	UP_ASSERT_EQUAL(string("test").size(), 4);
	UP_ASSERT_EQUAL(string("test").size(), char(4));

	const auto cvalue = { 1, 2, 3, 4, 5 };
	const initializer_list<int> ivalue = { 1, 2, 3, 4, 5 };
	const list<int> lvalue = { 1, 2, 3, 4, 5 };
	const vector<int> vvalue = { 1, 2, 3, 4, 5 };
	const array<int, 5> avalue = {{ 1, 2, 3, 4, 5 }};
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

UP_TEST(AssertCompareEnums)
{
	enum { ie = 12345 };
	const int ii = 12345;
	const unsigned iu = 12345;

	UP_ASSERT_EQUAL(ii, ie);
	UP_ASSERT_EQUAL(ie, iu);
	UP_ASSERT_EQUAL(iu, ie);

	enum class strong : uint64_t { is = 12345 };
	UP_ASSERT_EQUAL(iu, strong::is);
	UP_ASSERT_EQUAL(strong::is, ii);
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
