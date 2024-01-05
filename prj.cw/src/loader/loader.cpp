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
			return { ve::ErrorCodes::CannotParseImageFromFile, file_path.string() };

		}			
		temp.push_back(std::move(cv::imread(file_path.string())));
	}
	
	for (auto& el : temp) {
		mats_.emplace_back(std::move(el));
	}
	return ve::ErrorCodes::OK;
}

int64_t ve::ImageLoader::getCurrentSize() const noexcept {
	// TODO: check if this correct.
	// Here I check the size by just getting its area and multiplying by sizeof(int) since
	// Technically that's how a mat's stored in the memory,
	// but I am not completely sure about that.
	int64_t size_sum = 0;
	for (const auto& el : mats_) {
		// ?????
		size_sum += el.elemSize() * el.total();
		//size_sum += el.size().area() * sizeof(int);
	}
	return size_sum;
}

ve::Error ve::DirectoryLoader::loadFromDirectory(const ve::Path& file_path) {
	if (isDirty()) {
		return ve::ErrorCodes::WasDirty;
	}

	std::vector<std::filesystem::directory_entry> entries = ve::listFiles(file_path);
	for (auto& loader : loaders) {
		for (const auto& entry : entries) {
			if (loader->isExtensionSupported(entry.path().extension().string())) {
				ve::Error err = loader->loadFromFile(entry.path());
				if (getCurrentSizeOfBlocks() > Options::getInstance().getMaxSize()) {
					throw std::exception("Dont load so much, geez");
				}
				if (err.code != ve::ErrorCodes::OK) return err;
			}
		}
	}
	return ve::ErrorCodes::OK;
}



