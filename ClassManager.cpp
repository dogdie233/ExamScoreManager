#include "ClassManager.hpp"
#include "Utils.hpp"

namespace esm
{
	ClassManager& ClassManager::getInstance() noexcept
	{
		static ClassManager instance;
		return instance;
	}

	int ClassManager::addClass(const std::string& className)
	{
		return addClass(std::string(className));
	}

	int ClassManager::addClass(std::string&& className)
	{
		if (className.empty())
			return -1;

		int id = getClassId(className);
		if (id != -1)
			return id;

		classes.push_back(std::move(className));
		save();
		return classes.size() - 1;
	}

	int ClassManager::getClassId(const std::string& className)
	{
		return (find(classes.begin(), classes.end(), className) - classes.begin() + 1) % (classes.size() + 1) - 1;
	}

	std::vector<std::string>& ClassManager::getClasses() noexcept
	{
		return classes;
	}

	bool ClassManager::save()
	{
		csv::CsvWriter writer(nullptr);
		if (!CreateCsvWriterSafe(this->persistentData, writer))
			return false;

		for (const auto& className : classes)
			writer << className;

		return true;
	}

	bool ClassManager::load()
	{
		csv::CsvReader reader(nullptr);
		if (!CreateCsvReaderSafe(this->persistentData, reader))
			return false;

		classes.clear();
		while (reader.hasNext())
		{
			std::string className;
			reader >> className;
			classes.push_back(className);
		}

		return true;
	}
}
