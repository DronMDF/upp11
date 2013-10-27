
#pragma once
#include <functional>
#include <iostream>
#include <list>

namespace upp11 {

class TestCollection {
private:
	std::list<std::function<void ()>> tests;

	static TestCollection &getInstance() {
		static TestCollection collection;
		return collection;
	};

public:
	static void addTest(std::function<void ()> test)
	{
		TestCollection &collection = getInstance();
		collection.tests.push_back(test);
	}

	static void runAllTests()
	{
		TestCollection &collection = getInstance();
		for (auto t: collection.tests) {
			t();
		}
	}
};

template <typename T>
class TestInvokerTrivial {
private:
	void invoke() {
		T test;
		test();
	}
public:
	TestInvokerTrivial() {
		TestCollection::addTest(std::bind(&TestInvokerTrivial::invoke, this));
	}
};

template <typename T, typename ...V>
class TestInvokerParametrized {
private:
	void invoke(const std::tuple<V...> &params) {
		T test;
		test(params);
	}
public:
	TestInvokerParametrized(const std::initializer_list<const std::tuple<V...>> &params)
	{
		for (const auto v: params) {
			TestCollection::addTest(std::bind(&TestInvokerParametrized::invoke, this, v));
		}
	}
};

} // end of namespace upp11

#define UP_RUN() \
	upp11::TestCollection::runAllTests()

#define UP_TEST(name) \
class Test##name { \
public: \
	void operator()(); \
}; \
static upp11::TestInvokerTrivial<Test##name> test##name##invoker; \
void Test##name::operator()()

#define UP_PARAMETRIZED_TEST(name, params, ...) \
class Test##name { \
public: \
	void operator()(const tuple<__VA_ARGS__> &params); \
}; \
static upp11::TestInvokerParametrized<Test##name, __VA_ARGS__> test##name##invoker(params); \
void Test##name::operator()(const tuple<__VA_ARGS__> &params)

#define UP_FAIL(msg) \
	std::cout << msg << std::endl;
