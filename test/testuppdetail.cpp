
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
