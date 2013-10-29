
#pragma once
#include <algorithm>
#include <functional>
#include <iostream>
#include <memory>
#include <vector>

namespace upp11 {

class TestCollection {
private:
	std::vector<std::pair<std::string, std::function<bool ()>>> tests;
	std::vector<std::string> suites;

	static TestCollection &getInstance() {
		static TestCollection collection;
		return collection;
	};

public:
	static void addTest(const std::string &name, std::function<bool ()> test)
	{
		TestCollection &collection = getInstance();
		std::string path;
		for (auto s: collection.suites) {
			path += s + "/";
		}
		collection.tests.push_back(std::make_pair(path + name, test));
	}

	static void runAllTests(unsigned seed)
	{
		TestCollection &collection = getInstance();
		if (seed != 0) {
			std::default_random_engine r(seed);
			std::shuffle(collection.tests.begin(), collection.tests.end(), r);
		}
		for (auto t: collection.tests) {
			const bool result = t.second();
			std::cout << t.first << ": " << (result ? "SUCCESS" : "FAIL") << std::endl;
		}
	}

	static void beginSuite(const std::string &name)
	{
		TestCollection &collection = getInstance();
		collection.suites.push_back(name);
	}

	static void endSuite()
	{
		TestCollection &collection = getInstance();
		collection.suites.pop_back();
	}
};

class TestSuiteBegin {
public:
	TestSuiteBegin(const std::string &name) {
		TestCollection::beginSuite(name);
	}
};

class TestSuiteEnd {
public:
	TestSuiteEnd() {
		TestCollection::endSuite();
	}
};

class TestException : public std::exception {
};

template <typename T>
class TestInvokerTrivial {
public:
	bool invoke() {
		std::shared_ptr<T> instance;
		try {
			instance = std::make_shared<T>();
		} catch (const TestException &) {
			return false;
		} catch (const std::exception &e) {
			std::cout << "exception from test: " << e.what() << std::endl;
			return false;
		} catch (...) {
			std::cout << "unknown exception from test" << std::endl;
			return false;
		}

		try {
			(*instance)();
		} catch (const TestException &) {
			return false;
		} catch (const std::exception &e) {
			std::cout << "exception from test: " << e.what() << std::endl;
			return false;
		} catch (...) {
			std::cout << "unknown exception from test" << std::endl;
			return false;
		}

		return true;
	}
	TestInvokerTrivial(const std::string &name) {
		TestCollection::addTest(name, std::bind(&TestInvokerTrivial::invoke, this));
	}
};

template <typename T, typename ...V>
class TestInvokerParametrized {
public:
	bool invoke(const std::tuple<V...> &params) {
		std::shared_ptr<T> instance;
		try {
			instance = std::make_shared<T>();
		} catch (const TestException &) {
			return false;
		} catch (const std::exception &e) {
			std::cout << "exception from test: " << e.what() << std::endl;
			return false;
		} catch (...) {
			std::cout << "unknown exception from test" << std::endl;
			return false;
		}

		try {
			(*instance)(params);
		} catch (const TestException &) {
			return false;
		} catch (const std::exception &e) {
			std::cout << "exception from test: " << e.what() << std::endl;
			return false;
		} catch (...) {
			std::cout << "unknown exception from test" << std::endl;
			return false;
		}

		return true;
	}
	TestInvokerParametrized(const std::string &name,
		const std::initializer_list<const std::tuple<V...>> &params)
	{
		for (const auto v: params) {
			TestCollection::addTest(name,
				std::bind(&TestInvokerParametrized::invoke, this, v));
		}
	}
};

} // end of namespace upp11

#define UP_MAIN() \
int main(int, char **) { \
	UP_RUN_SHUFFLED(time(0)); \
}

#define UP_RUN() \
upp11::TestCollection::runAllTests(0)
#define UP_RUN_SHUFFLED(seed) \
upp11::TestCollection::runAllTests(seed)

#define UP_SUITE_BEGIN(name) \
namespace name { \
	static upp11::TestSuiteBegin suite_begin(#name);

#define UP_SUITE_END() \
	static upp11::TestSuiteEnd suite_end; \
}

#define UP_TEST(name) \
struct Test##name { \
	void operator()(); \
}; \
static upp11::TestInvokerTrivial<Test##name> test##name##invoker(#name); \
void Test##name::operator()()

#define UP_FIXTURE_TEST(name, fixture) \
struct Test##name : public fixture { \
	void operator()(); \
}; \
static upp11::TestInvokerTrivial<Test##name> test##name##invoker(#name); \
void Test##name::operator()()

#define UP_PARAMETRIZED_TEST(name, params, ...) \
struct Test##name { \
	void operator()(const tuple<__VA_ARGS__> &params); \
}; \
static upp11::TestInvokerParametrized<Test##name, __VA_ARGS__> test##name##invoker(#name, params); \
void Test##name::operator()(const tuple<__VA_ARGS__> &params)

#define UP_FIXTURE_PARAMETRIZED_TEST(name, fixture, params, ...) \
struct Test##name : public fixture { \
	void operator()(const tuple<__VA_ARGS__> &params); \
}; \
static upp11::TestInvokerParametrized<Test##name, __VA_ARGS__> test##name##invoker(#name, params); \
void Test##name::operator()(const tuple<__VA_ARGS__> &params)

#define UP_FAIL(msg) \
std::cout << msg << std::endl;

#define UP_ASSERT(expr) \
if (!(expr)) { \
	std::cout << __FILE__ << "(" << __LINE__ << "): check " << #expr << " failed" << std::endl; \
	throw TestException(); \
}
