
#include <upp11.h>
#include <deque>
#include <forward_list>
#include <set>
#include <unordered_set>

using namespace std;
using namespace upp11;
using namespace upp11::detail;

// detail::type_traits
class A {};
static_assert(is_same<type_traits<A>::type, A>::value,
	"A is not convert to A");

static_assert(is_same<type_traits<bool>::type, bool>::value,
	"bool is not convert to bool");

static_assert(is_same<type_traits<char>::type, int64_t>::value,
	"char is not convert to int64_t");
static_assert(is_same<type_traits<short>::type, int64_t>::value,
	"short is not convert to int64_t");
static_assert(is_same<type_traits<int>::type, int64_t>::value,
	"int is not convert to int64_t");
static_assert(is_same<type_traits<long int>::type, int64_t>::value,
	"long int is not convert to int64_t");
static_assert(is_same<type_traits<long long int>::type, int64_t>::value,
	"long long int is not convert to int64_t");

static_assert(is_same<type_traits<uint8_t>::type, uint64_t>::value,
	"uint8_t is not convert to uint64_t");
static_assert(is_same<type_traits<unsigned short>::type, uint64_t>::value,
	"unsigned short is not convert to uint64_t");
static_assert(is_same<type_traits<uint32_t>::type, uint64_t>::value,
	"uint32_t is not convert to uint64_t");
static_assert(is_same<type_traits<long unsigned>::type, uint64_t>::value,
	"long unsigned is not convert to uint64_t");
static_assert(is_same<type_traits<long long unsigned>::type, uint64_t>::value,
	"long long unsigned is not convert to uint64_t");
static_assert(is_same<type_traits<unsigned>::type, uint64_t>::value,
	"unsigned is not convert to uint64_t");

static_assert(is_same<type_traits<char [5]>::type, string>::value,
	"char [] is not convert to string");
static_assert(is_same<type_traits<const char *>::type, string>::value,
	"const char * is not convert to string");
// This is generic conversion
static_assert(is_same<type_traits<string>::type, string>::value,
	"string is not convert to string");

enum enum_s { value_s = -100 };
static_assert(is_same<type_traits<enum_s>::type, int64_t>::value,
	"enum is not convert to int64_t");
enum enum_u { value_u = 100 };
static_assert(is_same<type_traits<enum_u>::type, uint64_t>::value,
	"enum is not convert to uint64_t");
enum class enum_ss : short { value = 299 };
static_assert(is_same<type_traits<enum_ss>::type, int64_t>::value,
	"strong enum is not convert to int64_t");
enum class enum_su : uint64_t { value = numeric_limits<uint64_t>::max() };
static_assert(is_same<type_traits<enum_su>::type, uint64_t>::value,
	"strong enum is not convert to uint64_t");

static_assert(is_same<type_traits<int[3]>::type, vector<int64_t>>::value,
	"int[N] is not convert to vector<int64_t>");
static_assert(is_same<type_traits<list<uint8_t>>::type, vector<uint64_t>>::value,
	"list<uint8_t> is not convert to vector<uint64_t>");
static_assert(is_same<type_traits<vector<int>>::type, vector<int64_t>>::value,
	"vector<int> is not convert to vector<int64_t>");
static_assert(is_same<type_traits<array<long, 4>>::type, vector<int64_t>>::value,
	"array<int, 4> is not convert to vector<int64_t>");
static_assert(is_same<type_traits<initializer_list<long long>>::type, vector<int64_t>>::value,
	"initializer_list<long long> is not convert to vector<int64_t>");
static_assert(is_same<type_traits<deque<char>>::type, vector<int64_t>>::value,
	"deque<char> is not convert to vector<int64_t>");
static_assert(is_same<type_traits<forward_list<size_t>>::type, vector<uint64_t>>::value,
	"forward_list<size_t> is not convert to vector<uint64_t>");

// This is a strange, but only for test...
static_assert(is_same<type_traits<set<uint32_t>>::type, vector<uint64_t>>::value,
	"set<uint32_t> is not convert to vector<uint64_t>");
static_assert(is_same<type_traits<unordered_set<int64_t>>::type, vector<int64_t>>::value,
	"unordered_set<int64_t> is not convert to vector<int64_t>");

// And combinations
static_assert(is_same<type_traits<vector<set<list<short>>>>::type,
		vector<vector<vector<int64_t>>>>::value,
	"vector<set<list<short>>> is not convert to vector<vector<vector<int64_t>>>");
