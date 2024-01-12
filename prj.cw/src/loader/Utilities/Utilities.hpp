#pragma once
/*!
	\file 
	\brief Заголовочный файл, хранящий в себе дополнительные функции для удобства

	\todo Расширить класс Error, а также перенести его в отдельный файл.
*/
#ifndef UTILITIES_HPP_2023_10_27
#define UTILITIES_HPP_2023_10_27

#include <vector>
#include <string_view>
#include <filesystem>
#include <unordered_set>

namespace ve {
	// Works only with unicode
	inline constexpr char toLower(char a) noexcept {
		if (a <= 'Z' && a >= 'A')
			return a + ('a' - 'A');
		return a;
	}



	// Works only with unicode!
	constexpr inline std::string toLower(const std::string& str) {
		std::string string_temp;
		string_temp.resize(str.size());
		std::transform(str.begin(), str.end(), string_temp.begin(), [](char a) { return toLower(a); });
		return string_temp;
	}

	enum class ErrorCodes {
		OK = 0, /// < Ошибки не было
		FileLargerThanAChunk, /// < Файл оказался больше, чем 1 чанк
		CannotParseImageFromFile, /// < Бэкэнд не смог прочитать изображение из файла(к примеру, файл битый)
		UnsupportedExtension, /// < Расширение не поддерживается
		WasDirty, /// <Класс, который можно использовать только раз, уже использовался
		SumOfFilesLargerThanAThreshold, /// < Сумма веса файлов была больше, чем заданный максимум хранения
		Unfiltered = ~0 /// < Неизвестная ошибка
	};


	/*!
		\brief Эта структура определяет возможные ошибки, следуя из ErrorCodes.
	*/
	struct Error {
		Error() = default;
		//! Создаёт Ошибку исходя из кода и заданного пользователем сообщения.
		Error(ErrorCodes code, const std::string& message) : code(code), message(message) {}
		Error(ErrorCodes code) : code(code) {}
		const ErrorCodes code;
		const std::string message = ""; //! < Кастомное сообщение об ошибке, которое является эквивалентом what() в std::exception
		//! Оператор приведения к bool. 
		//! Инвертирует код ошибки(т.к. ErrorCodes::OK(0) является эквивалентом false).
		//! Используется для удобноо
		operator bool() {
			return !static_cast<bool>(code);
		}
	};


	// TODO :all this should be in its own classes
	//! Функция, которая переводит байты в килобайты (1024 байт == 1 килобайт)
	inline constexpr [[nodiscard]] int64_t toKilobytes(int64_t bytes) noexcept {
		return bytes >> 10;
	}

	inline constexpr [[nodiscard]] int64_t toMegabytes(int64_t bytes) noexcept {
		return ve::toKilobytes(bytes) >> 10;
	}

	inline constexpr [[nodiscard]] int64_t toGigabytes(int64_t bytes) noexcept {
		return ve::toMegabytes(bytes) >> 10;
	}

	inline constexpr [[nodiscard]] int64_t fromKilobytes(int64_t kilobytes) noexcept {
		return kilobytes << 10;
	}

	inline constexpr [[nodiscard]] int64_t fromMegabytes(int64_t megabytes) noexcept {
		return fromKilobytes(megabytes) << 10;
	}

	inline constexpr [[nodiscard]] int64_t fromGigabytes(int64_t gigabytes) noexcept {
		return fromMegabytes(gigabytes) << 10;
	}
	
	//! Показывает, в каком направлении должна обходиться папка
	enum class DirectoryIteration {
		NORMAL = 0, /// < Прямой обход
		RECURSIVE = 1 /// < Рекурсивный обход, т.е. включая все подпапки.
	};

	namespace {
		using Path = std::filesystem::path;
		using DirectoryEntry = std::filesystem::directory_entry;
	}

	/*!
		Выводит список файлов в целевой директории
		\param[in] directory Путь к целевой папке, из которой хотим получить список файлов
		\param[in] it_type Как следует обходить папку
		\return Список файлов целевой директории
	*/
	inline std::vector<DirectoryEntry> listFiles(const Path& directory, DirectoryIteration it_type = DirectoryIteration::NORMAL) {
		using namespace std::filesystem;
		std::error_code err;

		std::vector<DirectoryEntry> all_files;

		switch (it_type) {
		case DirectoryIteration::NORMAL:
			for (auto& el : directory_iterator(directory)) {
				all_files.push_back(el);
			}
			break;
		case DirectoryIteration::RECURSIVE:
			for (auto& el : recursive_directory_iterator(directory)) {
				all_files.push_back(el);
			}
			break;
		}

		return all_files;

	}
}

#endif