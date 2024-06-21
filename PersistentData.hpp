#pragma once

#ifndef ESM_PERSISTENT_DATA_HPP
#define ESM_PERSISTENT_DATA_HPP

#include <string>
#include "CsvHelper.hpp"

namespace esm
{
	class PersistentData
	{
	public:
		PersistentData(const std::string path);

		bool isExist() const noexcept;

		csv::CsvWriter CreateWriter() const;

		csv::CsvReader CreateReader() const;
	private:
		const std::string path;
	};

	class PersistentDataSavable
	{
	public:
		virtual bool save() = 0;

		virtual bool load() = 0;

		PersistentDataSavable(const std::string path) : persistentData(path) {}
	protected:
		esm::PersistentData persistentData;
	};
}

#endif