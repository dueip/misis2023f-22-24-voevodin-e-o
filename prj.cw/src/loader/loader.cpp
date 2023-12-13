#include <loader/loader.hpp>

ve::Error ve::ImageLoader::loadFromFile(const ve::Path& file_path) {
	if (!isExtensionSupported(file_path.extension().string())) {
		return { ve::ErrorCodes::UnsupportedExtension, file_path.extension().string() };
	}
	using namespace std::filesystem;
	//std::vector<std::filesystem::directory_entry> entries = ve::listFiles(path);
	std::vector<cv::Mat> temp;
	
	if (is_regular_file(file_path)) {

		if (file_size(file_path) > chunk_size) {
			return Error{ ve::ErrorCodes::FileLargerThanAChunk, "File was larger than a chunk" };
		}
		if (!cv::haveImageReader(file_path.string())) {
			return { ve::ErrorCodes::CannotParseImageFromFile, "Couldn't read an image from a file (" + file_path.string() + ")"};

		}			
		temp.push_back(std::move(cv::imread(file_path.string())));
	}
	
	for (auto& el : temp) {
		mats_.emplace_back(std::move(el));
	}
	return ve::ErrorCodes::OK;
}

ve::Error ve::DirectoryLoader::loadFromFile(const ve::Path& file_path) {
	if (isDirty()) {
		return ve::ErrorCodes::WasDirty;
	}

	std::vector<std::filesystem::directory_entry> entries = ve::listFiles(file_path);
	for (auto& loader : loaders) {
		for (const auto& entry : entries) {
			if (loader->isExtensionSupported(entry.path().extension().string())) {
				ve::Error err = loader->loadFromFile(entry.path());
				if (err.code != ve::ErrorCodes::OK) return err;
			}
		}
	}
	return ve::ErrorCodes::OK;
}



