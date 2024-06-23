#pragma once

#ifndef ESM_UTILS_HPP
#define ESM_UTILS_HPP

#include <ranges>
#include <vector>
#include <string>
#include <list>
#include "PersistentData.hpp"

namespace esm
{
	class Breadcrumb;

	void waitEnterPressed();

	void waitAnyKeyPressed();

	void printNavigator();

	void printCommands(Breadcrumb& breadcrumb);

	int selectOption(const std::vector<std::string>& options, int selecting = 0);

	void clearConsole();

	int stringCount(const std::string& str, const std::string& pattern);

	int dummyStrLenCalc(const std::string& str);

	bool CreateCsvWriterSafe(const PersistentData& persistentData, csv::CsvWriter& writer);
	
	bool CreateCsvReaderSafe(const PersistentData& persistentData, csv::CsvReader& writer);

	bool ConfirmDeleteAction(const std::string& itemType, const std::string& itemName);
}
#endif
