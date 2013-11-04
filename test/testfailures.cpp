#include <upp11.h>

using namespace std;

UP_SUITE_BEGIN(suiteAssertExceptionWithMessage)

UP_TEST(ShouldFailByType)
{
	UP_ASSERT_EXCEPTION(int, "hello", []{
		throw runtime_error("goodby");
	});
}

UP_TEST(ShouldFailByNoThrow)
{
	UP_ASSERT_EXCEPTION(runtime_error, "hello", []{
		// no throw
	});
}

UP_TEST(ShouldFailByNotEqualMessage)
{
	UP_ASSERT_EXCEPTION(exception, "message", []{
		throw runtime_error("another message");
	});
}

UP_TEST(ShouldFailByChildException)
{
	UP_ASSERT_EXCEPTION(overflow_error, "message", []{
		throw runtime_error("message");
	});
}

UP_SUITE_END()

UP_SUITE_BEGIN(suiteAssertException)

UP_TEST(ShouldFailByType)
{
	UP_ASSERT_EXCEPTION(int, []{
		throw runtime_error("goodby");
	});
}

UP_TEST(ShouldFailByNoThrow)
{
	UP_ASSERT_EXCEPTION(runtime_error, []{
		// no throw
	});
}

UP_SUITE_END()

UP_MAIN()
