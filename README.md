upp11
=====

Lightweight C++11 single header unit test framework

To use framework:

<ol>
<li value=1>Copy upp11.h in you project dir.</li>
<li value=2>Create unit test source files or modify existing file</li>
</ol>

```C++
#include "upp11.h"
```

<ol>
<li value=3>Write the tests</li>
</ol>

```C++
UP_TEST(test1)
{
	// test code
}

// tuple - for example. Any type possible, but params should be monotypes.
const auto params = {
	make_tuple(1, "str1"),
	make_tuple(2, "str222")
	// etc
};

UP_PARAMETRIZED_TEST(test2, params)
{
	const auto i = get<0>(params);
	const auto s = get<1>(params);
	// parametrized test code
}

struct fixture {
};

UP_FIXTURE_TEST(test3, fixture)
{
	// test code with fixture
}

// parametrized with fixture available too...
```

<ol>
<li value=4>Using test assertions</li>
</ol>

```C++
UP_TEST(test)
{
	UP_ASSERT(0 < 1);
	UP_ASSERT_EQUAL("right", "right");
	UP_ASSERT_NE(list<int>{1, 2, 3, 4, 5 }, vector<int>{5, 4, 3, 2, 1});

	// check exception by type
	UP_ASSERT_EXCEPTION(runtime_error, []{
		// code under test here...
	});

	// check exception by what
	UP_ASSERT_EXCEPTION(runtime_error, "exception message", []{
		// code under test here...
	});
}
```

<ol>
<li value=5>Group tests</li>
</ol>

```C++
UP_SUITE_BEGIN(suite_name);

// tests and child suites here

UP_SUITE_END();
```

<ol>
<li value=6>Compile and run the test</li>
</ol>

```C++
// once for all test source files of test runner
UP_MAIN();
```

```shell
$ runner [-q] [-t] [-s <seed>]
```

<ol>
<li value=7>Enjoy</li>
</ol>

