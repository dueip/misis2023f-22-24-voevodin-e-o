#include <loader/loader.hpp>

ve::Error ve::ImageLoader::loadFromFile(const ve::Path& file_path) {
	using namespace std::filesystem;
	//std::vector<std::filesystem::directory_entry> entries = ve::listFiles(path);
	std::vector<cv::Mat> temp;
	
	if (is_regular_file(file_path) && extensions.find(ve::toLower(file_path.extension().string())) != extensions.end()) {

		if (file_size(file_path) > chunk_size) {
			return Error{ ve::ErrorCodes::FileLargerThanAChunk, "File was larger than a chunk" };
		}
		if (!cv::haveImageReader(file_path.string())) {
			return { ve::ErrorCodes::CannotParseImageFromFile, "Couldn't read an image from a file (" + file_path.string() + ")"};

		}			
		temp.push_back(std::move(cv::imread(file_path.string())));
	}
	
	mats_ = std::move(temp);
	return ve::ErrorCodes::OK;
}

ve::Error ve::DirectoryLoader::loadFromFile(const ve::Path& file_path) {
	std::vector<std::filesystem::directory_entry> entries = ve::listFiles(file_path);
	for (auto& loader : loaders) {
		for (const auto& entry : entries) {
			if (loader->isExtensionSupported(entry.path().extension().string())) {
				ve::Error err = loader->loadFromFile(file_path);
				if (err) return err;
			}
		}
	}
	return ve::ErrorCodes::OK;
}



ve::Error ve::DirectoryLoader::loadFromFiles(const auto begin, const ve::DirectoryLoader::It& end) {
	
	for (auto& loader : loaders) {
		for (ve::DirectoryLoader::It entry{}; entry != end; ++entry) {
			
			if (loader->isExtensionSupported(entry.path().extension().string())) {
				ve::Error err = loader->loadFromFile(file_path);
				if (err) return err;
			}
		}
	}
	return ve::ErrorCodes::OK;
}