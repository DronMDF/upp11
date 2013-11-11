#include <upp11.h>

using namespace std;

UP_SUITE_BEGIN(suiteCheckpoints)

UP_TEST(UnhandledExceptionInTestShouldCheckpointed)
{
	throw false;
}

struct throw_fixture {
	throw_fixture() {
		throw runtime_error("setUp exception for checkpoint");
	}
};

UP_FIXTURE_TEST(ExceptionInFixtureShouldCheckpointed, throw_fixture)
{
}

struct zero_fixture {
	int divide(int a, int b) const;
};

UP_FIXTURE_TEST(DivizionByZeroInTestShouldCheckpointed, zero_fixture)
{
	divide(10, 0);
}

int zero_fixture::divide(int a, int b) const {
	return a / b;
}

struct deref_fixture {
	int deref(int *a) const;
};

UP_FIXTURE_TEST(SegFaultInTestShouldCheckpointed, deref_fixture)
{
	deref(nullptr);
}

int deref_fixture::deref(int *a) const {
	return *a;
}

UP_SUITE_END()

UP_SUITE_BEGIN(suiteAssertEqual)

UP_TEST(ShouldFailByNoEqual)
{
	UP_ASSERT_EQUAL(1, 0);
}

UP_SUITE_END()

UP_SUITE_BEGIN(suiteAssertNe)

UP_TEST(ShouldFailByEqual)
{
	UP_ASSERT_NE(1, 1);
}

UP_SUITE_END()

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
