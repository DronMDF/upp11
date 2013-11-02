
#include <upp11.h>

using namespace std;
using namespace upp11;

UP_SUITE_BEGIN(suteTestInvoker)

UP_TEST(InvokerShouldCatchAllExceptionInTestCtor)
{
	// Given
	struct TestWithException {
		TestWithException() {
			throw runtime_error("setUp exception");
		}
	};
	TestInvoker<TestWithException> invoker;
	// When/Then
	UP_ASSERT(!invoker.invoke([](TestWithException *){}));
}

UP_TEST(InvokerShouldCatchAllUnknownExceptionInTestCtor)
{
	// Given
	struct TestWithException {
		TestWithException() {
			throw 1;
		}
	};
	TestInvoker<TestWithException> invoker;
	// When/Then
	UP_ASSERT(!invoker.invoke([](TestWithException *){}));
}

UP_TEST(InvokerShouldCatchAllExceptionInTestRun)
{
	// Given
	struct Test {};
	TestInvoker<Test> invoker;
	// When/Then
	UP_ASSERT(!invoker.invoke([](Test *){ throw runtime_error("run exception"); }));
}

UP_TEST(InvokerShouldCatchAllUnknownExceptionInTestRun)
{
	// Given
	struct Test {};
	TestInvoker<Test> invoker;
	// When/Then
	UP_ASSERT(!invoker.invoke([](Test *){ throw 1; }));
}

UP_SUITE_END()

UP_SUITE_BEGIN(suiteTestBase)

UP_TEST(isEqualShouldCompareOtherTypes)
{
	TestBase base;
	UP_ASSERT(!base.isEqual(1, 0));
	UP_ASSERT(!base.isEqual(1, "0"));
	UP_ASSERT(!base.isEqual("te", "next"));
	UP_ASSERT(base.isEqual(100, 100));
	UP_ASSERT(base.isEqual("last", "last"));

	const list<int> lvalue = { 1, 2, 3, 4, 5 };
	const vector<int> vvalue = { 1, 2, 3, 4, 5 };
	UP_ASSERT(base.isEqual(lvalue, vvalue));

	const list<string> lstring = { "one", "two", "free" };
	const vector<string> vstring = { "one", "two", "free" };
	UP_ASSERT(base.isEqual(lstring, vstring));
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
