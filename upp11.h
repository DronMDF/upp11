
#pragma once
#include <algorithm>
#include <chrono>
#include <csignal>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <iterator>
#include <list>
#include <memory>
#include <sstream>
#include <vector>
#include <getopt.h>
#include <string.h>

namespace upp11 {

class TestCollection {
private:
	typedef std::pair<std::string, std::function<bool ()>> test_pair_t;
	std::vector<test_pair_t> tests;
	std::vector<std::string> suites;
public:
	static TestCollection &getInstance() {
		static TestCollection collection;
		return collection;
	}

	void beginSuite(const std::string &name) {
		suites.push_back(name);
	}

	void endSuite() {
		suites.pop_back();
	}

	void addTest(const std::string &name, std::function<bool ()> test) {
		std::string path;
		for (auto s: suites) {
			path += s + "::";
		}
		tests.push_back(std::make_pair(path + name, test));
	}

	bool runAllTests(unsigned seed, bool quiet, bool timestamp) {
		// Отсортируем все по именам, чтобы не зависело от порядка линковки
		std::sort(tests.begin(), tests.end(),
			[](const test_pair_t &A, const test_pair_t &B){ return A.first < B.first; });
		if (seed != 0) {
			if (!quiet) {
				std::cout << "random seed: " << seed << std::endl;
			}
			std::default_random_engine r(seed);
			std::shuffle(tests.begin(), tests.end(), r);
		}
		bool failure = false;
		for (auto t: tests) {
			using namespace std::chrono;
			const high_resolution_clock::time_point st = high_resolution_clock::now();
			const bool success = t.second();
			const high_resolution_clock::time_point et = high_resolution_clock::now();
			const unsigned us = duration_cast<microseconds>(et - st).count();
			if (!quiet || !success) {
				std::cout << t.first;
				if (timestamp) {
					std::cout << " (" << us << "us)";
				}
				std::cout << ": " << (success ? "SUCCESS" : "FAIL") << std::endl;
			}
			failure |= !success;
		}
		return !failure;
	}

	std::string checkpoint_location;
	std::string checkpoint_message;

	void checkpoint(const std::string &location, const std::string &message) {
		checkpoint_location = location;
		checkpoint_message = message;
	}
};

class TestSuiteBegin {
public:
	TestSuiteBegin(const std::string &name) {
		TestCollection::getInstance().beginSuite(name);
	}
};

class TestSuiteEnd {
public:
	TestSuiteEnd() {
		TestCollection::getInstance().endSuite();
	}
};

class TestException {};

template <typename T>
class TestInvoker {
	const std::string location;
public:
	TestInvoker(const std::string &location) : location(location) { }

	bool invoke(std::function<void (T *)> test_function) const {
		std::shared_ptr<T> instance;
		try {
			TestCollection::getInstance().checkpoint(location, "Test constructor");
			instance = std::make_shared<T>();
		} catch (const TestException &) {
			return false;
		} catch (const std::exception &e) {
			std::cout << "exception from test ctor: " << e.what() << std::endl;
			std::cout << TestCollection::getInstance().checkpoint_location
				<< ": last checkpoint "
				<< TestCollection::getInstance().checkpoint_message << std::endl;
			return false;
		} catch (...) {
			std::cout << "unknown exception from test ctor" << std::endl;
			std::cout << TestCollection::getInstance().checkpoint_location
				<< ": last checkpoint "
				<< TestCollection::getInstance().checkpoint_message << std::endl;
			return false;
		}

		try {
			TestCollection::getInstance().checkpoint(location, "Test run");
			test_function(instance.get());
		} catch (const TestException &) {
			return false;
		} catch (const std::exception &e) {
			std::cout << "exception from test run: " << e.what() << std::endl;
			std::cout << TestCollection::getInstance().checkpoint_location
				<< ": last checkpoint "
				<< TestCollection::getInstance().checkpoint_message << std::endl;
			return false;
		} catch (...) {
			std::cout << "unknown exception from test run" << std::endl;
			std::cout << TestCollection::getInstance().checkpoint_location
				<< ": last checkpoint "
				<< TestCollection::getInstance().checkpoint_message << std::endl;
			return false;
		}

		return true;
	}
};

template <typename T>
class TestInvokerTrivial : public TestInvoker<T> {
private:
	bool invoke() {
		return TestInvoker<T>::invoke(std::bind(&T::run, std::placeholders::_1));
	}
public:
	TestInvokerTrivial(const std::string &location, const std::string &name)
		: TestInvoker<T>(location)
	{
		TestCollection::getInstance().addTest(name,
			std::bind(&TestInvokerTrivial::invoke, this));
	}
};

template <typename T, typename C>
class TestInvokerParametrized : public TestInvoker<T> {
private:
	bool invoke(const typename C::value_type &params) {
		return TestInvoker<T>::invoke(std::bind(&T::run, std::placeholders::_1, params));
	}
public:
	TestInvokerParametrized(const std::string &location, const std::string &name, const C &params)
		: TestInvoker<T>(location)
	{
		for (const auto v: params) {
			TestCollection::getInstance().addTest(name,
				std::bind(&TestInvokerParametrized::invoke, this, v));
		}
	}
};

template <typename T>
struct TestValue {
	const std::vector<T> value;
	const bool agregate;

	TestValue(const T &iv)
		: value(1, iv), agregate(false) {}
	template<typename I>
	TestValue(const I &begin, const I &end)
		: value(begin, end), agregate(true) {}
};

struct TestValueFactory {
	template <typename T>
	static TestValue<T> create(const T &t) {
		return TestValue<T>(t);
	}
	template<typename T>
	static TestValue<T> create(const std::initializer_list<T> &t) {
		return TestValue<T>(t.begin(), t.end());
	}
	template<typename T>
	static TestValue<T> create(const std::initializer_list<const T> &t) {
		return TestValue<T>(t.begin(), t.end());
	}
	template<typename T>
	static TestValue<T> create(const std::list<T> &t) {
		return TestValue<T>(t.begin(), t.end());
	}
	template<typename T>
	static TestValue<T> create(const std::vector<T> &t) {
		return TestValue<T>(t.begin(), t.end());
	}
	template<typename T, std::size_t size>
	static TestValue<T> create(const std::array<T, size> &t) {
		return TestValue<T>(t.begin(), t.end());
	}
	template<typename T, std::size_t size>
	static TestValue<T> create(const T (&t)[size]) {
		return TestValue<T>(&t[0], &t[size]);
	}
	static TestValue<std::string> create(const char *t) {
		return TestValue<std::string>(t);
	}
};

template <typename T>
std::ostream &operator << (std::ostream &os, const TestValue<T> &t)
{
	if (t.agregate) { os << "{ "; }
	std::copy(t.value.begin(), t.value.end(), std::ostream_iterator<T>(os, ", "));
	if (t.agregate) { os << " }"; }
	return os;
}

class TestEqual {
	template <typename A, typename B, typename AI, typename BI>
	bool isEqualDiffer(const TestValue<A> &, const TestValue<B> &, AI, BI) const {
		return false;
	}

	template <typename A, typename B>
	bool isEqualSign(const TestValue<A> &ta, const TestValue<B> &tb) const {
		const B abmin = (std::is_unsigned<A>::value || std::is_unsigned<B>::value) ?
			0 : std::numeric_limits<A>::min() /* both signed */ ;
		const B abmax = (std::is_signed<A>::value || std::is_unsigned<B>::value ||
				 sizeof(A) < sizeof(B))
			? std::numeric_limits<A>::max() : std::numeric_limits<B>::max();
		for (size_t i = 0; i < ta.value.size(); i++) {
			if (tb.value[i] < abmin || tb.value[i] > abmax) return false;
			if (ta.value[i] != tb.value[i]) return false;
		}
		return true;
	}
	template <typename A, typename B>
	bool isEqualDiffer(const TestValue<A> &ta, const TestValue<B> &tb,
			   std::true_type, std::true_type) const
	{
		if (ta.agregate != tb.agregate) return false;
		if (ta.value.size() != tb.value.size()) return false;
		if (sizeof(A) <= sizeof(B)) return isEqualSign(ta, tb);
		return isEqualSign(tb, ta);
	}

	template <typename A, typename B>
	bool isEqualValue(const TestValue<A> &ta, const TestValue<B> &tb) const {
		return isEqualDiffer(ta, tb, typename std::is_integral<A>::type(),
				typename std::is_integral<B>::type());
	}

	// Compare equivalent values
	template <typename T>
	bool isEqualValue(const TestValue<T> &ta, const TestValue<T> &tb) const {
		return ta.agregate == tb.agregate && ta.value == tb.value;
	}
public:
	template <typename A, typename B>
	bool isEqual(const A &a, const B &b) const {
		const auto ta = TestValueFactory::create(a);
		const auto tb = TestValueFactory::create(b);
		return isEqualValue(ta, tb);
	}
};

class TestAssert : private TestEqual {
	const std::string location;

	template <typename A, typename B>
	std::string asPrintable(const A &a, const B &b) const {
		const auto ta = TestValueFactory::create(a);
		const auto tb = TestValueFactory::create(b);
		std::ostringstream os;
		os << ta << " vs " << tb;
		return os.str();
	}

public:
	TestAssert(const std::string &location) : location(location) {}

	template <typename A, typename B>
	void assertEqual(const A &a, const B &b, const std::string &expression) const
	{
		if (isEqual(a, b)) { return; }
		std::cout << location << ": check equal (" << expression << ") failed" << std::endl;
		std::cout << "\t" << asPrintable(a, b) << std::endl;
		throw TestException();
	}

	template <typename A, typename B>
	void assertNe(const A &a, const B &b, const std::string &expression) const
	{
		if (!isEqual(a, b)) { return; }
		std::cout << location << ": check not equal (" << expression << ") failed" << std::endl;
		std::cout << "\t" << asPrintable(a, b) << std::endl;
		throw TestException();
	}

	void assert(bool expr, const std::string &expression) const
	{
		if (expr) { return; }
		std::cout << location << ": check " << expression << " failed" << std::endl;
		throw TestException();
	}
};

template <typename E>
struct TestExceptionChecker {
	const std::string location;
	const std::string extype;

	TestExceptionChecker(const std::string &location, const std::string &extype)
		: location(location), extype(extype) {}

	void check(const std::function<void ()> &f) {
		try {
			f();
		} catch (const E &e) {
			return;
		} catch (...) {
		}
		std::cout << location << ": expected exception "
			<< extype << " not throw" << std::endl;
		throw upp11::TestException();
	}

	void check(const std::string &message, const std::function<void ()> &f) {
		if (!std::is_convertible<E, std::exception>::value) {
			std::cout << location << ": expected exception "
				<< extype << " is not child of std::exception" << std::endl;
			throw upp11::TestException();
		}
		bool catched = false;
		try {
			try {
				f();
			} catch (const E &) {
				catched = true;
				throw;
			}
		} catch (const std::exception &e) {
			if (catched) {
				if (e.what() == message) { return; }
				std::cout << location << ": check exception "
					<< extype << "(\"" << message << "\") failed" << std::endl;
				std::cout << "\tcatched exception: \"" << e.what() << "\"" << std::endl;
				throw upp11::TestException();
			}
		} catch (...) {
		}
		std::cout << location << ": expected exception "
			<< extype << "(\"" << message << "\") not throw" << std::endl;
		throw upp11::TestException();
	}
};

class TestMain {
	static void signalHandler(int signum) {
		std::ostringstream out;
		out << "Signal (" << strsignal(signum) << ") received";
		throw std::runtime_error(out.str());
	}

public:
	int main(int argc, char **argv) {
		bool quiet = false;
		bool timestamp = false;
		int seed = time(0);
		while (true) {
			int opt = getopt(argc, argv, "qts:");
			if (opt == -1) { break; }
			if (opt == 'q') { quiet = true; }
			if (opt == 't') { timestamp = true; }
			if (opt == 's') { seed = std::atoi(optarg); }
		};
		signal(SIGFPE, signalHandler);
		signal(SIGILL, signalHandler);
		signal(SIGSEGV, signalHandler);
		return TestCollection::getInstance().runAllTests(seed, quiet, timestamp) ? 0 : -1;
	}
};

} // end of namespace upp11

#define T_(x) #x
#define T(x) T_(x)
#define LOCATION __FILE__ "(" T(__LINE__) ")"

#define UP_MAIN() \
int main(int argc, char **argv) { \
	return upp11::TestMain().main(argc, argv); \
}

#define UP_RUN() \
upp11::TestCollection::getInstance().runAllTests(0, false, false)

#define UP_SUITE_BEGIN(name) \
namespace name { \
	static upp11::TestSuiteBegin suite_begin(#name);

#define UP_SUITE_END() \
	static upp11::TestSuiteEnd suite_end; \
}

#define UP_TEST(testname) \
struct testname { \
	void run(); \
}; \
static upp11::TestInvokerTrivial<testname> testname##_invoker(LOCATION, #testname); \
void testname::run()

#define UP_FIXTURE_TEST(testname, fixture) \
struct testname : public fixture { \
	void run(); \
}; \
static upp11::TestInvokerTrivial<testname> testname##_invoker(LOCATION, #testname); \
void testname::run()

#define UP_PARAMETRIZED_TEST(testname, params) \
struct testname { \
	void run(const decltype(params)::value_type &params); \
}; \
static upp11::TestInvokerParametrized<testname, decltype(params)> \
	testname##_invoker(LOCATION, #testname, params); \
void testname::run(const decltype(params)::value_type &params)

#define UP_FIXTURE_PARAMETRIZED_TEST(testname, fixture, params) \
struct testname : public fixture { \
	void run(const decltype(params)::value_type &params); \
}; \
static upp11::TestInvokerParametrized<testname, decltype(params)> \
	testname##_invoker(LOCATION, #testname, params); \
void testname::run(const decltype(params)::value_type &params)

#define UP_ASSERT(...) \
upp11::TestAssert(LOCATION).assert(__VA_ARGS__, #__VA_ARGS__)

#define UP_ASSERT_EQUAL(...) \
upp11::TestAssert(LOCATION).assertEqual(__VA_ARGS__, #__VA_ARGS__)

#define UP_ASSERT_NE(...) \
upp11::TestAssert(LOCATION).assertNe(__VA_ARGS__, #__VA_ARGS__)

#define UP_ASSERT_EXCEPTION(extype, ...) \
upp11::TestExceptionChecker<extype>(LOCATION, #extype).check(__VA_ARGS__)
