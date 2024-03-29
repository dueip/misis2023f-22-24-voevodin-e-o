﻿#pragma once

#ifndef BRUH_HPP_2023_10_27
#define BRUH_HPP_2023_10_27

#include <vector>
#include <string_view>
#include <filesystem>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include <unordered_set>
#include <Options/Options.hpp>
#include <iostream>


namespace ve {

	//! Возвращает размер cv::Mat по формуле mat.elemSize() * mat.total();
	inline [[nodiscard]] int64_t calculateMatSize(const cv::Mat& mat) noexcept {
		return mat.elemSize() * mat.total();
	}


	//! Класс, предоставляющий интерфейс для Ленивой загрузки Mat(по необходимости)
	class MatButCooler final {
	public:
		MatButCooler(const std::filesystem::path& path) {
			path_to_the_mat = path;
		}

		//! Приведение к cv::Mat, которое загружает низлежащий cv::Mat по необходимости.
		//! Устанавливает флаг is_loaded в true
		operator cv::Mat() const {
			if (mat.empty()) {
				mat = cv::imread(path_to_the_mat.string());
			}
			is_loaded = true;
			return mat;
		}

		//! Загрузка низлежего cv::Mat.
		//! Устанавливает флаг is_loaded в true
		void preLoad() {
			if (mat.empty()) {
				mat = cv::imread(path_to_the_mat.string());
			}
			is_loaded = true;
		}

		[[nodiscard]] bool isLoaded() const noexcept {
			return is_loaded;
		}

		//! Технически является удалением, однако из-за особенности работы cv::Mat просто опускает из данного хранения.
		void markReadyToDelete() {
			// Just dropss the mat???
			int refCount = (mat.u) ? ((mat.u)->refcount) : 0;
			if (refCount > 1) {
				//throw std::exception("WHAT IS HAPPPENING THIS SHOULD HAVE NEVER BEEN THE CASE");
			}
			mat.release();

			is_loaded = false;
		}

	private:
		mutable bool is_loaded = false;
		mutable cv::Mat mat;
		std::filesystem::path path_to_the_mat;
	};

	/*!
		Класс, который предоставляет функции для сохранения масок в определенную папку, а также дальнейших их подгрузок.
	*/
	class ImageWriter {
	public:
		ImageWriter() = delete;
		ImageWriter(const std::filesystem::path& path) {
			setPath(path);
		}
		ImageWriter(ImageWriter&&) = default;
		ImageWriter& operator=(ImageWriter&&) = default;

		ImageWriter& operator=(const ImageWriter&) = delete;
		ImageWriter(const ImageWriter&) = delete;

		~ImageWriter() = default;


		enum class Extensions {
			JPG,
			PNG
		};

		void setPath(const std::filesystem::path& path_to_add) noexcept {
			// Если путь -- директория и при этом её не существует, то создаём его
			if (!std::filesystem::exists(path_to_add)){
				std::filesystem::create_directory(path_to_add);
			}
			cached_path_ = path_to_add;
		}

		/*!  \todo Добавить поддержку  нескольких расширений сразу
		* Сохраняет маски, переданные в эту функцию, в кэшированную папку, заданную в конструкторе.
		*
		* \param [in] begin Итератор, указывающий на начало контейнера с масками
		* \param [in] end Итератор, указывающий на конец контейнера с масками
		* \param [in] extension Сохранять с каким расширением (см enum Extensions)
		* \return OK если не было ошибки. UnsupportedExtension, если расширение по какой-либо причине не поддерживается. CannotParseImageFromFile, если по какой-то бэкэнд не может сохранить картинку.
		*/
		template<std::forward_iterator ForwardIt>
		ve::Error saveMasks(const ForwardIt begin, const  ForwardIt end, Extensions extension = Extensions::JPG) const   {
			return saveMasks(begin, end, cached_path_, extension);
		}


		/*!  \todo Добавить поддержку  нескольких расширений сразу
		* Сохраняет маски, переданные в эту функцию, в целевую папку
		* 
		* \param [in] begin Итератор, указывающий на начало контейнера с масками
		* \param [in] end Итератор, указывающий на конец контейнера с масками
		* \param [in] path Целевая папка
		* \param [in] extension Сохранять с каким расширением (см enum Extensions)
		* \return OK если не было ошибки. UnsupportedExtension, если расширение по какой-либо причине не поддерживается. CannotParseImageFromFile, если по какой-то бэкэнд не может сохранить картинку.
		*/
		template<std::forward_iterator ForwardIt>
		static ve::Error saveMasks(const ForwardIt begin, const ForwardIt end, const std::filesystem::path& path, Extensions extension = Extensions::JPG) {
			for (ForwardIt it = begin; it != end; ++it) {
				std::string string_extension;
				switch (extension) {
				case Extensions::JPG:
					string_extension = ".jpg";
					break;
				case Extensions::PNG:
					string_extension = ".png";
					break;
				default:
					return ve::ErrorCodes::UnsupportedExtension;
				}
				
				std::string where_to_save = path.string() + "/mask_" + std::to_string(masks_saved_count) + string_extension;
				if (!cv::imwrite(where_to_save, *it)) {
					return ve::ErrorCodes::CannotParseImageFromFile;
				}
				++masks_saved_count;

			}
			return ve::ErrorCodes::OK;
		}
 
	private:
		std::filesystem::path cached_path_;
		inline static int masks_saved_count = 0;
	};

	//! \returns Количество сохраннёных изображений
	inline int generateTestData(const std::filesystem::path& path_to_test_folder, cv::Mat test_mat, int64_t size_of_test_data = ve::fromGigabytes(1)) {
		if (std::filesystem::exists(path_to_test_folder) && std::filesystem::is_directory(path_to_test_folder)) {
			if (!std::filesystem::is_empty(path_to_test_folder)) {
				return 0;
			}
		}
		ImageWriter writer(path_to_test_folder);
		int64_t how_much_images_should_be_in_the_folder = size_of_test_data / ve::calculateMatSize(test_mat);
		std::vector<cv::Mat> a;
		a.reserve(how_much_images_should_be_in_the_folder);
		for (int i = 0; i < how_much_images_should_be_in_the_folder; ++i) {
			a.push_back(test_mat);
		}
		
		writer.saveMasks(a.begin(), a.end());
		return how_much_images_should_be_in_the_folder;
	}


	/*!
	* Интерфейс для всех загрузчиков(кроме DirectoryLoader).
	* Все функции абстрактные, так что ILoader не имеет никакой имплементации в себе.
	* Не имеет никаких скрытых членов.
	*/
	class ILoader {
	public:
		ILoader() = default;

		//! Копирование загрузчика запрещено, т.к. это не имеет смысла.
		ILoader& operator=(const ILoader&) = delete;
		ILoader(const ILoader&) = delete;

		ILoader& operator=(ILoader&&) = default;
		ILoader(ILoader&&) = default;

		virtual ~ILoader() = default;

		
		//! Загрузка из файла. Если файла не существует, то файл не создсатся.
		virtual ve::Error loadFromFile(const Path& path) = 0;

		//! Флаг, который показывает, было ли что-то загружено. В основном используется для блокировки повторного считывания.
		virtual bool isDirty() const noexcept = 0;

		//! Очищает Загрузчик, а также ставит флаг Dirty в false.
		virtual void reset() = 0;

		virtual [[nodiscard]] std::vector<cv::Mat>& getData() noexcept = 0;
		
		virtual [[nodiscard]] const std::vector<cv::Mat>& getData() const noexcept = 0;

		virtual [[nodiscard]] std::vector<cv::Mat> copyData() const noexcept = 0;

		//! Проверяет, поддерживает ли данный загрузчик конкретное расширение (например, jpg)
		virtual constexpr [[nodiscard]] bool isExtensionSupported(const std::string& extension) const noexcept = 0;

		//! То же самое, что и \code Options::getInstance().getChunkSize(); \endcode
		virtual [[nodiscard]] int64_t getMaxChunkSize() const noexcept = 0;
		
		//! Размер загруженных данных на данный  момент
		virtual [[nodiscard]] int64_t getCurrentSize() const noexcept = 0;
		//! Синоним \code getCurrentSize(); \endcode
		virtual [[nodiscard]] int64_t getLoadedSize() const noexcept = 0;

		/*!
			В отличие от STL не выбрасывает исключения при выходе за границы.
			\param [in] index Индекс, откуда взять cv::Mat. 
			\returns cv::Mat на данном индексе.
		*/
		virtual [[nodiscard]] cv::Mat at(int64_t index) = 0;
		
		/*! 
		* \returns Курсор на последний обработанный at элемент.
		*/
		virtual [[nodiscard]] int64_t getCursor() const noexcept = 0;
	};

	/*!
	*	Данный класс не поддерживается, однако является заготовкой со всеми готовыми функциями для подгрузки Dicom.
	*	\brief Не поддерживается из-за странностей в работе DCMTK с MCVS. У меня так и не получилось решить проблему :(
	*/
	class DicomLoader final : public ILoader {
	public:
		DicomLoader() = default;
		DicomLoader(const DicomLoader& other) = delete;
		DicomLoader& operator=(const DicomLoader& other) = delete;
		
		DicomLoader(DicomLoader&&) = default;
		DicomLoader& operator=(DicomLoader&&) = default;
		
		virtual ~DicomLoader() = default;

		[[nodiscard]] std::vector<cv::Mat>& getData() noexcept { is_dirty_ = true; return mats_; };
		[[nodiscard]] const std::vector<cv::Mat>& getData() const noexcept { return mats_; };
		[[nodiscard]] std::vector<cv::Mat> copyData() const noexcept { return mats_; };
		
		/*!
			\param [in] path Путь как Dicom файлу 
			\returns CannotParseImageFromFile независимо от path, т.к. загрузка невозможна
		*/
		ve::Error loadFromFile(const Path& path) override {
			//if (isExtensionSupported(path.extension().string())) {
			//	return ve::ErrorCodes::UnsupportedExtension;
			//}
			//DicomImage image(path.string().c_str());
			//if (image.getStatus() != EIS_Normal) {
			//	//! \todo Add support for  different reasons why it's not possible to read the dicom.
			//	return ve::ErrorCodes::CannotParseImageFromFile;
			//}

			//cv::Mat mat(int(image.getWidth()), int(image.getHeight()), CV_8U, (uchar*)image.getOutputData(8));
			//mats_.push_back(mat)
			return ve::ErrorCodes::CannotParseImageFromFile;
		}

		bool isDirty() const noexcept { return is_dirty_; };
		
		//! Поддерживвает только dcm
		constexpr [[nodiscard]] bool isExtensionSupported(const std::string& extension) const noexcept override {
			return extension == ".dcm";
		}

		void reset() override { is_dirty_ = false; mats_.clear(); }

		[[nodiscard]] int64_t getMaxChunkSize() const noexcept override { return Options::getInstance().getChunkSize(); }
		[[nodiscard]] int64_t getCurrentSize() const noexcept override {
			int64_t size_sum = 0;
			for (const auto& el : mats_) {
				size_sum += el.elemSize() * el.total();
			}
			return size_sum;
		}

	private:
		std::vector<cv::Mat> mats_;
		bool is_dirty_ = false;
	};


	/*! 
		Основной класс, поддерживающий загрузку изображений (jpg и png) с помощью cv бэкэнда.
		Для загрузки используется cv::imread, всё хранится в cv::Mat. 
		Т.к. cv::Mat привносит большой оверхед при загрузке его в память, то загружаются не сразу все изображения, а только часть
		(1 чанк, размер чанка определяется в Options). При этом иногда может загружаться 1 чанк + 1 дополнительная картинка.
		Остальные же cv::Mat не хранятся загруженными, а хранятся в ve::MatButCooler, который использует ленивую загрузку
		(подгружает cv::Mat только по нашей просьбе/привидении его в cv::Mat).
		Также работает автоматическое удаление неиспользуемых(выходящих за рамки Чанка cv::Mat), но, учитывая что cv::Mat работает
		как std::shared_ptr, невсегда получается гарантироват удаление.

		\todo Может добавить подгрузку определенных картинок просто в память, т.к. cv::Mat привносит довольно большой overhead
	
	*/
	class ImageLoader final : public ILoader {
	public:

		ImageLoader() = default;
		ImageLoader(const ImageLoader&) = delete;
		ImageLoader& operator=(const ImageLoader&) = delete;

		ImageLoader(ImageLoader&&) = default;
		ImageLoader& operator=(ImageLoader&&) = default;
		
		virtual ~ImageLoader() = default;

		//! Возвращает <i>доступные</i> данные. Т.е. возвращает только те cv::Mat, которые уже загружены в память.
		[[nodiscard]] std::vector<cv::Mat>& getData() noexcept { is_dirty_ = true; getAvailableMats(); return cached_mats_; };
		//! Возвращает <i>доступные</i> данные. Т.е. возвращает только те cv::Mat, которые уже загружены в память.
		[[nodiscard]] const std::vector<cv::Mat>& getData() const noexcept { return getAvailableMats(); };
		//! Возвращает <i>доступные</i> данные. Т.е. возвращает только те cv::Mat, которые уже загружены в память.
		[[nodiscard]] std::vector<cv::Mat> copyData() const noexcept { return getAvailableMats(); };

		
		[[nodiscard]] int64_t getLoadedSize() const noexcept { return mats_.size(); }


		ve::Error loadFromFile(const Path& path) override;

		bool isDirty() const noexcept { return is_dirty_; };

		
		/*! Возвращает true только для jpg, jpeg и png
		* \param [in] extension Расширение для проверки. При этом расширение может быть в любом формате(как ".jpg", так и "jpg"), а также любой строчности букв.
		* \returns Поддерживается ли расширение или нет.
		*/
		[[nodiscard]] bool isExtensionSupported(const std::string& extension) const noexcept override {
			std::string fixed_extension;
			fixed_extension.reserve(fixed_extension.size() + 1);
			if (extension.size() > 0 && extension[0] != '.') {
				fixed_extension = '.' + extension;
			}
			else {
				fixed_extension = extension;
			}
			return (supported_extensions.find(ve::toLower(fixed_extension)) != supported_extensions.end());
		}

		[[nodiscard]] int64_t getCursor() const noexcept override {
			return cursor;
		}


		void reset() override { is_dirty_ = false; mats_.clear(); cached_mats_.clear(); }

		constexpr [[nodiscard]] int64_t getMaxChunkSize() const noexcept override { return chunk_size; }
		[[nodiscard]] int64_t getCurrentSize() const noexcept override;

		/*!
		* Возвращает cv::Mat по индексу, при этом загружая в память область number_of_images_in_a_chunk и разгружая остальные. 
		*/
		[[nodiscard]] cv::Mat at(int64_t index) {
			cached_mats_.clear();
			for (auto& el : mats_) {
				if (el.isLoaded()) {
					el.markReadyToDelete();
				}
			}
			//TODO: check if correct
			for (int i = index - number_of_images_in_a_chunk / 2; i <= index + number_of_images_in_a_chunk / 2; ++i) {
				if (i < 0)
					continue;
				if (i > mats_.size())
					break;
				mats_[i].preLoad();
			}
			cursor = index;
			auto where_to_get = getAvailableMats().size() > 0 ? index % getAvailableMats().size() : index;
			return getAvailableMats()[where_to_get];
		}
	private:

		std::vector<cv::Mat> getAvailableMats() const {
			if (cached_mats_.empty()) {
				for (int i = cursor; i < mats_.size(); ++i) {
					if (mats_[i].isLoaded()) {
						cached_mats_.push_back(static_cast<cv::Mat>(mats_[i]));
					}
				}
			}
			return cached_mats_;
		}

		mutable std::vector<cv::Mat> cached_mats_;

		std::vector<MatButCooler> mats_;
		bool is_dirty_ = false;

		const inline static int64_t chunk_size = Options::getInstance().getChunkSize();
		// For now we'll go with only those 3 types, but we'll see.
		const inline static std::unordered_set<std::string> supported_extensions = { ".png", ".jpg", ".jpeg" };

		int64_t number_of_images_in_a_chunk = 0;
		int64_t cursor = 0;
	};


	class ImageStorage final {
	public:
	private:

	};
	

	/*!
	* Класс, отвечающий за загрузку определенной папки. Несмотря на своё название, не относится к ILoader(хотя и имеет схожий интерфейс).
	* Сам по себе он не производит загрузку данных, а полагается на использование массива загрузчиков. Т.е. этот класс является просто упрощённым интерфейсом для использование ImageLoader'а.
	* В себе хранит только кэшированные данные 1 чанка, по запросу выдаёт эти же данные. 
	* При этом можно попросить сдвинуться на 1 чанк.
	*/

	class DirectoryLoader final  {
	public:
		DirectoryLoader() = default;
		virtual ~DirectoryLoader() = default;

		/*!
		* Загружает данные из папки
		* Загрузка невозможна, если флаг Dirty активен
		* \param [in] path Папка, откуда нужно загрузить данные.
		*/
		ve::Error loadFromDirectory(const Path& path);

		/*!
		* Загружает несколько файлов.
		* Загрузка невозможна, если флаг Dirty активен.
		* \param [in] begin Forward Итератор, указывающий на начало последовательности
		* \param [in] end Forward Итератор, указывабщий на конец последовательности
		*/
		template<std::forward_iterator PathIt>
		ve::Error loadFromFiles(const PathIt& begin, const PathIt& end);


		[[nodiscard]] const std::vector<cv::Mat>& getData() const noexcept { constructVector(); return cached_vector_; };
		[[nodiscard]] std::vector<cv::Mat> copyData() const noexcept { return constructVector(); };

		
		bool isDirty() const noexcept { for (const auto& el : loaders) { if (el->isDirty()) return true; } return false; };

		void reset() { cached_vector_.clear(); for (auto& el : loaders) { el->reset(); } }

		//! Возвращает комбинацию всех доступных расширений низлежащих загрузчиков
		constexpr [[nodiscard]] bool isExtensionSupported(const std::string& extension) const noexcept {
			for (const auto& el : loaders) { if (el->isExtensionSupported(extension)) return true; } return false;
		}
		
		 [[nodiscard]] int64_t getMaxChunkSize() const noexcept {
			return Options::getInstance().getChunkSize();
		}
		int64_t getCurrentSizeOfBlocks() const noexcept {
			// potentially can overflow if we get big enough numbers
			// but for now shouldn't worry since there's no way any picture file will outgrow 2^63 (~9 000 000 000) gigabytes
			int64_t sum_of_sizes = 0;
			for (const auto& loader : loaders) {
				sum_of_sizes += loader->getCurrentSize();
			}
			return sum_of_sizes;
		}

		//! Запрашивает новый чанк и удаляет старый. При этом курсор передвигается на конец даты.
		std::vector<cv::Mat> requestNewChunk() {
			cached_vector_.clear();
			for (auto& loader : loaders) {
				loader->at(loader->getData().size());
			}
			
			return constructVector();
		}

	private:

		// Mutable since we do want our getChunkSize function to be available anywhere
		// -1 means that we don't have it cached just yet
		mutable int64_t chunk_size_cached_ = -1;
		

		const std::vector<std::shared_ptr<ILoader>> loaders = {
			std::make_shared<ImageLoader>()
		};


		mutable std::vector<cv::Mat> cached_vector_;

		constexpr [[nodiscard]] std::vector<cv::Mat> constructVector() const {
			if (cached_vector_.empty()) {
				cached_vector_.clear();
				std::vector<cv::Mat> vec;
				for (const auto& loader : loaders) {
					const std::vector<cv::Mat>& loader_data = loader->getData();
					for (const cv::Mat& mat : loader_data) {
						vec.push_back(mat);
					}
				}
				cached_vector_ = std::move(vec);
			}
			return cached_vector_;
		}
	};

}


template<std::forward_iterator PathIt>
ve::Error ve::DirectoryLoader::loadFromFiles(const PathIt& begin, const PathIt& end) {	

	if (isDirty()) {
		return ve::ErrorCodes::WasDirty;
	}

	for (auto& loader : loaders) {
		for (PathIt entry = begin; entry != end; ++entry) {	
			if (loader->isExtensionSupported(entry->extension().string())) {
				ve::Error err = loader->loadFromFile(*entry);
				if (getCurrentSizeOfBlocks() > Options::getInstance().getMaxSize()) {
					throw std::exception("Dont load so much, geez");
				}
				if (err.code != ve::ErrorCodes::OK) return err;
			}
		}
	}
	cached_vector_ = constructVector();
	return ve::ErrorCodes::OK;
}

#endif