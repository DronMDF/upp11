
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

UP_SUITE_BEGIN(suiteAssertions)

UP_TEST(EqualShouldThrow)
{
	TestBase base;
	UP_ASSERT(!base.isEqual(1, 0));
	UP_ASSERT(!base.isEqual(1, "0"));
	UP_ASSERT(!base.isEqual("te", "next"));
	UP_ASSERT(base.isEqual(100, 100));
	UP_ASSERT(base.isEqual("last", "last"));

	UP_ASSERT_NE(1, 0);
	UP_ASSERT_NE(0, "null");
	UP_ASSERT_NE("te", "next");
	UP_ASSERT_EQUAL(100, 100);
	UP_ASSERT_EQUAL("last", "last");
}

UP_SUITE_END()

UP_MAIN()
