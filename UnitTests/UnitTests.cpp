#include "CppUnitTest.h"
#include <ACL/general.h>
#include <ACL/log.h>
#include <ACL/type_traits.h>
#include <ACL/container_traits.h>
#include <vector>
#include <map>


using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTests {
	TEST_CLASS(UnitTests) {
	public:
		TEST_METHOD(test_container_traits) {
			using traits_vector = abel::container_traits<std::vector<int>>;
			using traits_carr = abel::container_traits<int[15]>;

			static_assert(std::is_same_v<traits_vector::value_type, int>);
			static_assert(std::is_same_v<traits_carr::value_type, int>);
		}
	};
}
