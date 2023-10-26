#pragma once

#ifndef BRUH_HPP_2023_10_27
#define BRUH_HPP_2023_10_27

#include <filesystem>


namespace ve {
	// Move only
	class ILoader {
	public:
		ILoader() = default;
		ILoader& operator=(const ILoader&) = delete;
		ILoader(const ILoader&) = delete;

		ILoader& operator=(ILoader&&) = default;
		ILoader(ILoader&&) = default;

		virtual ~ILoader() = default;


		virtual constexpr bool loadFromFile(const std::filesystem::path& path) const = 0;
	};

	class TestLoader final : public ILoader {
	public:
		TestLoader() = default;
		virtual constexpr bool loadFromFile(const std::filesystem::path& path) const override { return true; };
		virtual ~TestLoader() = default;
	};
}

#endif