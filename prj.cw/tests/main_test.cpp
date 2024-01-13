//#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#define DOCTEST_CONFIG_NO_UNPREFIXED_OPTIONS
#define DOCTEST_CONFIG_IMPLEMENT

#include <doctest/doctest.h>
#include <loader/loader.hpp>
#include <algorithm>
#include <opencv2/highgui.hpp>
#include <map>
#include <cxxopts.hpp>


class dt_removed {
	std::vector<const char*> vec;
public:
	dt_removed(const char** argv_in) {
		for (; *argv_in; ++argv_in)
			if (strncmp(*argv_in, "--dt-", strlen("--dt-")) != 0)
				vec.push_back(*argv_in);
		vec.push_back(nullptr);
	}

	int          argc() { return static_cast<int>(vec.size()) - 1; }
	const char** argv() { return &vec[0]; } // Note: non-const char **:
};

std::string location;
int program(int argc, const char** argv);

int main(int argc, const char** argv) {
	doctest::Context context(argc, argv);


	dt_removed args(argv);
	try {
		cxxopts::Options options("Unit Tests", "Unit tests for a weirdo");
		options.add_options()
			("f,file", "File name", cxxopts::value<std::string>()->default_value(CMAKE_TEST_PATH));
		options.allow_unrecognised_options();
		auto result = options.parse(args.argc(), args.argv());
		location = result["file"].as<std::string>();
		if (location.empty()) {
			location = CMAKE_TEST_PATH;
		}
	}
	catch (const std::exception& ex) {
		std::cout << ex.what();
		return 1 ;
	}

	int test_result = context.run(); // run queries, or run tests unless --no-run

	if (context.shouldExit()) // honor query flags and --exit
		return test_result;

	int app_result = program(args.argc(), args.argv());

	return test_result + app_result; // combine the 2 results
}

int program(int argc, const char** argv) {
	return EXIT_SUCCESS;
}


namespace ve {
	class TestLoader final : public ILoader {
	public:
		TestLoader() {
			mats_.reserve(5);
		}
		virtual ~TestLoader() = default;

		[[nodiscard]] std::vector<cv::Mat>& getData() noexcept override { is_dirty_ = true; return mats_; };

		[[nodiscard]] const std::vector<cv::Mat>& getData() const noexcept override { return mats_; };

		[[nodiscard]] std::vector<cv::Mat> copyData() const noexcept override { return mats_; };

		ve::Error loadFromFile(const Path& path) override { return ve::ErrorCodes::OK; };
		bool isDirty() const noexcept { return is_dirty_; };


	private:
		std::vector<cv::Mat> mats_;
		bool is_dirty_ = false;
	};
}

namespace {
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

	auto check_list_files = []
	(const std::vector<ve::Path>& answers, const std::filesystem::path& rel_test_path,
		ve::DirectoryIteration it = ve::DirectoryIteration::NORMAL) -> bool
		{
			const std::filesystem::path test_dir_path = std::filesystem::path(location).string() + rel_test_path.string();
			auto test_dir_entires = ve::listFiles(test_dir_path, it);
			return check_if_everything_was_found_correctly(test_dir_entires, answers, test_dir_path);
		};
}

TEST_CASE("List files") {
	
	SUBCASE("Normal") {
		const std::vector<ve::Path> normal_entries = { "test_rec_dir", "file.txt" };
		const std::vector<ve::Path> empty = { };

		const std::map<std::string, std::vector<ve::Path>> cases =
		{
			{"test_dir", normal_entries}
		};

		REQUIRE_NOTHROW(ve::listFiles(location));

		for (const auto& test_case : cases) {
			CHECK(check_list_files(test_case.second, test_case.first));
		}
	
	}

	SUBCASE("Recursive") {
		REQUIRE_NOTHROW(ve::listFiles(location));
		CHECK(check_list_files({"file.txt","test_rec_dir", "test_rec_dir/file2.txt"}, "test_dir", ve::DirectoryIteration::RECURSIVE));
	}
}

TEST_CASE("Translation units") {
	CHECK(ve::fromKilobytes(1) == 1024);
	CHECK(ve::fromMegabytes(1) == 1024 * 1024);
	// Проверяем, равен ли 1 мегабайт 1024 килобайтам
	CHECK(ve::fromMegabytes(1) == ve::fromKilobytes(1024));
	CHECK(ve::fromGigabytes(1) == 1024 * 1024 * 1024);
	CHECK(ve::fromKilobytes(1) + 512 == 1536);

	CHECK(ve::toKilobytes(1024) == 1);
	CHECK(ve::toMegabytes(ve::fromMegabytes(1)) == 1);
}

TEST_CASE("Image loader") {
	ve::ImageLoader image_loader;
	CHECK(image_loader.isDirty() == false);
	
	SUBCASE("Check if everything loads correctly") {
		CHECK_FALSE(image_loader.loadFromFile("./"));
		CHECK(image_loader.loadFromFile("./").code == ve::ErrorCodes::UnsupportedExtension);
		const std::vector<std::filesystem::path> files{
			std::string(location) + "images/1.jpg",
			std::string(location) + "images/3.png"
		};
		for (const auto& el : files) {
			CHECK(image_loader.loadFromFile(el));
		}
		CHECK(image_loader.getLoadedSize() == 2);
		CHECK(image_loader.getLoadedSize() == image_loader.getData().size());
		cv::Mat mat1 = image_loader.at(1);
		CHECK(!mat1.empty());
		CHECK(image_loader.getLoadedSize() == 2);
		cv::Mat mat = image_loader.at(0);
		CHECK(!mat.empty());
		CHECK(image_loader.getLoadedSize() == 2);
		
		image_loader.reset();
		
	}
}

TEST_CASE("Directory Loader") {
	ve::DirectoryLoader dir_loader;

	SUBCASE("DirectoryLoading, [loading]") {
		
		CHECK(dir_loader.loadFromDirectory("./"));
		dir_loader.reset();
		CHECK(dir_loader.loadFromDirectory(location));
		CHECK(dir_loader.copyData().size() == 0);
		dir_loader.reset();
		const std::vector<std::filesystem::path> files{
			std::string(location) + "images/1.jpg",
			std::string(location) + "images/3.png"
		};

		CHECK(dir_loader.loadFromFiles(files.cbegin(), files.cend()));
		CHECK(dir_loader.copyData().size() == 2);
		dir_loader.reset();
		CHECK(dir_loader.loadFromDirectory(std::string(location) + "images/"));
		CHECK(dir_loader.copyData().size() == 2);
		dir_loader.reset();
		ve::Error err = dir_loader.loadFromDirectory(std::string(location) + "images/bugged_images");
		CHECK(!err);
		CHECK(err.message == std::string(location) + "images/bugged_images\\1.jpg");
		CHECK(dir_loader.copyData().size() == 0);
		dir_loader.reset();

		SUBCASE("DirectoryLoading, [Load a big amount of data]") {
			CHECK(dir_loader.loadFromFiles(files.cbegin(), files.cend()));
			const std::string big_data_path = std::string(location) + "/big_data/";
			const auto& data = dir_loader.getData();
			ve::generateTestData(big_data_path, data[0]);
			dir_loader.reset();

			{
				ve::ImageLoader image_loader;
				image_loader.loadFromFile(big_data_path + "mask_0.jpg");
				ve::Options::getInstance().setChunkSize(ve::calculateMatSize(image_loader.at(0)));
			}
			dir_loader.loadFromDirectory(big_data_path);
			auto number_of_images_loaded = ceil(ve::calculateMatSize(dir_loader.getData()[0]) / static_cast<float>(ve::calculateMatSize(dir_loader.getData()[0])));
			CHECK(dir_loader.getData().size() == 1);
			cv::Mat a = dir_loader.getData()[0];
			dir_loader.requestNewChunk();
			cv::Mat b = dir_loader.getData()[0];
			CHECK(b.data != a.data);
			ve::Options::getInstance().setChunkSize(ve::fromMegabytes(512));
			dir_loader.reset();
			dir_loader.loadFromDirectory(big_data_path);

		}
	}
}

TEST_CASE("ImageWriter") {
	ve::ImageWriter writer(std::string(location) + "/test_masks/");
	
	ve::DirectoryLoader dir_loader;
	
	SUBCASE("Image Writer, [writing 1 cv::Mat to tests/masks]") {
		const std::vector<std::filesystem::path> files{
			std::string(location) + "images/1.jpg"
		};
		dir_loader.loadFromFiles(files.cbegin(), files.cend());
		CHECK(writer.saveMasks(dir_loader.getData().cbegin(), dir_loader.getData().cend()));
	}
	SUBCASE("Image Writer, [writing 2 cv::Mat to tests/masks]") {
		const std::vector<std::filesystem::path> files{
			std::string(location) + "images/1.jpg",
			std::string(location) + "images/3.png"
		};
		dir_loader.loadFromFiles(files.cbegin(), files.cend());
		const auto& data = dir_loader.getData();
		CHECK(writer.saveMasks(data.cbegin(), data.cend()));
	}
}




TEST_CASE("Dicom loader") {
	
	
}