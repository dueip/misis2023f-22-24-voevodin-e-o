#include <iostream>
#include <loader/loader.hpp>
#include <cxxopts.hpp>



int main(int argc, char** argv) {
//	std::cout << "\x1B[31mTexting\033[0m\t\t" << std::endl;
	try {
		cxxopts::Options options("Program test", "A simple CLI test for Loader");
		options.add_options()
			("s, size", "How much test data should this program generate\n [In \x1B[36mmegabytes\033[0m]", cxxopts::value<int>())
			("i, image", "Path to a test image", cxxopts::value<std::string>())
			("f, folder", "Where should data be cached. Notice that the folder should be empty!", cxxopts::value<std::string>()->default_value("./cache"))
			("r,should_remove", "Specifes whether files from the cache directory should be removed to regenerate cache. All files will be deleted permentantly!", cxxopts::value<bool>()->default_value("false"))
			("v,verbose", "Verbose output", cxxopts::value<bool>()->default_value("false"))
			("h,help", "Print usage")
			;
		//,
		auto result = options.parse(argc, argv);
		if (result.count("help")) {
			std::cout << options.help() << std::endl;
			return 0;
		}



		const std::filesystem::path cache_path = result["folder"].as<std::string>();
		const std::filesystem::path image_path = result["image"].as<std::string>();
		const int cache_size = result["size"].as<int>();
		ve::ImageLoader loader;
		ve::Error err = loader.loadFromFile(image_path);
		if (!err) {
			if (err.code == ve::ErrorCodes::UnsupportedExtension) {
				if (err.message.empty()) {
					std::cerr << "Folders are not supported. Try using a jpg or png file" << std::endl;
				}
				else {
					std::cerr << "Images with this (\x1B[36m" << err.message << "\033[0m) extension are not supported. Try using jpg or png." << std::endl;
				}
			}
			else if (err.code == ve::ErrorCodes::FileDoesNotExist) {
				std::cerr << "(\x1B[36m" + image_path.string() + "\033[0m) could not be found." << std::endl;
			}
			else if (err.code == ve::ErrorCodes::CannotParseImageFromFile) {
				std::cerr << "(\x1B[36m" + image_path.string() + "\033[0m) could not decode the image. Maybe the file is damaged?" << std::endl;
			}
			std::cerr << "\x1B[31mAborting!\033[0m" << std::endl;
			return -1;
		}
		std::cout << "Generating test data at (\x1B[36m" << cache_path.string() << "\033[0m) of size " << cache_size << " filled with (\x1B[36m" + image_path.string() + "\033[0m)..." << std::endl;
		const int size_of_1_image = loader.getCurrentSize();
		if (result.count("r")) {
			std::filesystem::remove_all(cache_path);
		}
		
		if (!std::filesystem::exists(cache_path) || (std::filesystem::exists(cache_path) && std::filesystem::is_empty(cache_path))) {
			int n;
			n = ve::generateTestData(cache_path, loader.at(0), ve::fromMegabytes(cache_size));
			std::cout << "Done generating test data!" << std::endl;
			std::cout << "Generated " + std::to_string(n) + " images at (\x1B[36m" + cache_path.string() + "\033[0m)" << std::endl;
		
		}
		else if (std::filesystem::exists(cache_path)) {
			std::cout << "Already filled with images, reading from there" << std::endl;
		}


		std::cout << "Started reading into memory...\n";
		{
			ve::DirectoryLoader dir_loader;
			std::cout << "With a size of a chunk: \x1B[36m" + std::to_string(ve::Options::getInstance().getChunkSize()) << "\033[0m(about " << ve::toMegabytes(ve::Options::getInstance().getChunkSize()) << " MB)\n";
			ve::Error err_dir = dir_loader.loadFromDirectory(cache_path);
			std::cout << "Done. Read a total of: " << dir_loader.getCurrentSizeOfBlocks() << " bytes of data (about " << ve::toMegabytes(dir_loader.getCurrentSizeOfBlocks()) << "MB)\n";
			std::cout << "(\x1B[36mabout " + std::to_string(dir_loader.getCurrentSizeOfBlocks() / size_of_1_image) << " images\033[0m)" << std::endl;
			dir_loader.reset();
		}
		{
			ve::DirectoryLoader dir_loader;
			ve::Options::getInstance().setChunkSize(ve::fromMegabytes(512) / 2);
			std::cout << "With a size of a chunk: \x1B[36m" + std::to_string(ve::Options::getInstance().getChunkSize()) << "\033[0m(about " << ve::toMegabytes(ve::Options::getInstance().getChunkSize()) << " MB)\n";
			ve::Error err_dir = dir_loader.loadFromDirectory(cache_path);
			std::cout << "Done. Read a total of: " << dir_loader.getCurrentSizeOfBlocks() << " bytes of data (about " << ve::toMegabytes(dir_loader.getCurrentSizeOfBlocks()) << "MB)\n";
			std::cout << "(\x1B[36mabout " + std::to_string(dir_loader.getCurrentSizeOfBlocks() / size_of_1_image) << " images\033[0m)" << std::endl;
			dir_loader.reset();
		}
		{
			ve::DirectoryLoader dir_loader;
			ve::Options::getInstance().setChunkSize(ve::fromMegabytes(1024));
			std::cout << "With a size of a chunk: \x1B[36m" + std::to_string(ve::Options::getInstance().getChunkSize()) << "\033[0m(about " << ve::toMegabytes(ve::Options::getInstance().getChunkSize()) << " MB)\n";
			ve::Error err_dir = dir_loader.loadFromDirectory(cache_path);
			std::cout << "Done. Read a total of: " << dir_loader.getCurrentSizeOfBlocks() << " bytes of data (about " << ve::toMegabytes(dir_loader.getCurrentSizeOfBlocks()) << "MB)\n";
			std::cout << "(\x1B[36mabout " + std::to_string(dir_loader.getCurrentSizeOfBlocks() / size_of_1_image) << " images\033[0m)" << std::endl;
			dir_loader.reset();
		}

	}
	catch (const std::exception& ex) {
		std::cerr << "\x1B[31m" << ex.what() << "; Aborting!\033[0m";
	}
}