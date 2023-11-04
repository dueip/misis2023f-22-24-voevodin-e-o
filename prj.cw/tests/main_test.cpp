#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include <bruh.hpp>
#include <algorithm>


auto check_if_everything_was_found_correctly = []
(const std::vector<ve::DirectoryEntry>& whats_being_checked, const std::vector<ve::Path>& to_check_with,
	const ve::Path& original_path
	) -> bool {
	for (auto& el : whats_being_checked) {
		auto relative_path = std::filesystem::relative(el, original_path);
		if (std::find(to_check_with.begin(), to_check_with.end(), relative_path) == to_check_with.end()) {
			return false;
		}
	}
	return true;
};
	

TEST_CASE("List files") {
	
	SUBCASE("Normal") {
		
		std::filesystem::path test_dir_path = std::filesystem::path(CMAKE_TEST_PATH).string() + "/test_dir";
		std::vector<ve::Path> normal_entries = { "test_rec_dir", "file.txt" };
		REQUIRE_NOTHROW(ve::listFiles(test_dir_path));
		auto test_dir_entires = ve::listFiles(test_dir_path);
		CHECK(check_if_everything_was_found_correctly(test_dir_entires, normal_entries, test_dir_path));

	
	}

	SUBCASE("Recursive") {

	}
}
