#pragma once

#ifndef ESM_SUBJECTMANAGER_HPP
#define ESM_SUBJECTMANAGER_HPP

#include <map>
#include <string>
#include "PersistentData.hpp"

namespace esm
{
	class SubjectManager : public PersistentDataSavable
	{
	public:
		static SubjectManager& getInstance() noexcept;

		std::map<int, std::string>& getSubjects() noexcept;

		int getSubjectId(const std::string& name) noexcept;

		const std::string& getSubjectName(const int id) noexcept;

		int addSubject(const std::string& name) noexcept;

		bool renameSubject(const int id, const std::string& name);

		void removeSubject(const int id) noexcept;

		int addSubject(std::string&& name) noexcept;

		bool isSubjectExist(int id) noexcept;

		bool save() override;

		bool load() override;

	private:
		std::map<int, std::string> subjects;

		SubjectManager() : PersistentDataSavable("data/subjects.csv") {}
		~SubjectManager() = default;

		SubjectManager(const SubjectManager& other) = delete;
		const SubjectManager& operator=(const SubjectManager& other) = delete;
	};
}
#endif
