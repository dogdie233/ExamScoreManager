#include "PersistentData.hpp"

#include <iostream>
#include <filesystem>

namespace esm
{
	PersistentData::PersistentData(const std::string path) : path(path) {}

    bool PersistentData::isExist() const noexcept
    {
        return std::filesystem::exists(path);
    }

	csv::CsvReader PersistentData::CreateReader() const
	{
		if (path.empty())
			throw std::runtime_error("Path is empty.");

		auto fs = std::make_shared<std::ifstream>(path);
		if (!fs->is_open())
			throw std::runtime_error("Failed to open file: " + path);

		return csv::CsvReader(fs);
	}

	csv::CsvWriter PersistentData::CreateWriter() const
	{
		if (path.empty())
			throw std::runtime_error("Path is empty.");

		auto filePath = std::filesystem::path(path);
		auto dirPath = filePath.parent_path();

		if (!dirPath.empty())
			std::filesystem::create_directories(dirPath);

		auto fs = std::make_shared<std::ofstream>(path, std::ios::out | std::ios::trunc);
		if (!fs->is_open())
			throw std::runtime_error("Failed to open file: " + path);

		return csv::CsvWriter(fs);
	}
}
