#pragma once
#ifndef ESM_CLASS_MANAGER_HPP
#define ESM_CLASS_MANAGER_HPP

#include "PersistentData.hpp"
#include <vector>

namespace esm
{
	class ClassManager : public PersistentDataSavable
	{
	public:
		static ClassManager& getInstance() noexcept;

		int addClass(const std::string& className);

		int addClass(std::string&& className);

		int getClassId(const std::string& className);

		std::vector<std::string>& getClasses() noexcept;

		bool save() override;

		bool load() override;

	private:
		std::vector<std::string> classes;

		ClassManager() : PersistentDataSavable("data/classes.csv") {}
		~ClassManager() = default;

		ClassManager& operator=(const ClassManager&) = delete;
		ClassManager(const ClassManager&) = delete;
	};
}

#endif
