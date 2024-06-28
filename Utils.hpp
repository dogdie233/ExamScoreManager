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
	struct StudentInfo;

	class Breadcrumb;

	void waitEnterPressed();

	void waitAnyKeyPressed();

	void printNavigator();

	void printCommands(Breadcrumb& breadcrumb);

	int selectMultiOptions(const std::vector<std::string>& options, std::vector<bool>& selections, int focusing = 0);

	int selectOption(const std::vector<std::string>& options, int selecting = 0);

	void clearConsole();

	constexpr int stringCount(const std::string& str, const std::string& pattern)
	{
		int count = 0;
		size_t pos = 0;
		while ((pos = str.find(pattern, pos)) != std::string::npos)
		{
			++count;
			pos += pattern.length();
		}
		return count;
	}

	constexpr int dummyStrLenCalc(const std::string& str)
	{
		int len = str.size() - stringCount(str, "·");
		return len;
	}

	bool CreateCsvWriterSafe(const PersistentData& persistentData, csv::CsvWriter& writer);
	
	bool CreateCsvReaderSafe(const PersistentData& persistentData, csv::CsvReader& writer);

	bool ConfirmDeleteAction(const std::string& itemType, const std::string& itemName);

	bool defaultStudentPtrComparator(const std::shared_ptr<StudentInfo>& lhs, const std::shared_ptr<StudentInfo>& rhs);

	bool defaultStudentComparator(const std::shared_ptr<StudentInfo>& lhs, const StudentInfo& rhs);
}
#endif
