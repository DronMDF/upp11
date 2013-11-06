
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

UP_SUITE_END()
