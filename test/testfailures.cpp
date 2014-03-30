#include <upp11.h>

using namespace std;

UP_SUITE_BEGIN(suiteCheckpoints)

UP_TEST(UnhandledExceptionInTestShouldCheckpointed)
{
	throw false;
}

struct throw_fixture {
	virtual ~throw_fixture() = default;
	throw_fixture() {
		throw runtime_error("setUp exception for checkpoint");
	}
};

UP_FIXTURE_TEST(ExceptionInFixtureShouldCheckpointed, throw_fixture)
{
}

struct zero_fixture {
	virtual ~zero_fixture() = default;
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
	virtual ~deref_fixture() = default;
	int deref(int *a) const;
};

UP_FIXTURE_TEST(SegFaultInTestShouldCheckpointed, deref_fixture)
{
	deref(nullptr);
}

int deref_fixture::deref(int *a) const {
	return *a;
}

UP_TEST(AssertIsCheckpoint)
{
	UP_ASSERT(true);
	throw false;
}

UP_TEST(AssertEqualIsCheckpoint)
{
	UP_ASSERT_EQUAL(1, 1);
	throw false;
}

UP_TEST(AssertNeIsCheckpoint)
{
	UP_ASSERT_NE(1, 0);
	throw false;
}

UP_TEST(AssertExceptionIsCheckpoint)
{
	UP_ASSERT_EXCEPTION(runtime_error, []{ throw runtime_error("true"); });
	throw false;
}

UP_TEST(AssertExceptionMessageIsCheckpoint)
{
	UP_ASSERT_EXCEPTION(runtime_error, "true", []{ throw runtime_error("true"); });
	throw false;
}

struct throw_param_fixture {
	virtual ~throw_param_fixture() = default;
	bool param() const {
		throw false;
	}
};

UP_FIXTURE_TEST(CheckpointShoildBeFixedBeforeArgumentsEvaluate, throw_param_fixture)
{
	UP_ASSERT(param());
}

UP_TEST(ExplicitCheckpointShouldBe)
{
	UP_CHECKPOINT("user checkpoint");
	throw false;
}

UP_FIXTURE_TEST(SecondSignalShouldBeCatched, deref_fixture)
{
	UP_ASSERT_EQUAL(deref(nullptr), 42);
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
