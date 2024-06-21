#pragma once

#ifndef ESM_SUBJECTMANAGER_HPP
#define ESM_SUBJECTMANAGER_HPP

#include <vector>
#include <string>
#include "PersistentData.hpp"

namespace esm
{
	class SubjectManager : public PersistentDataSavable
	{
	public:
		static SubjectManager& getInstance();

		std::vector<std::string>& getSubjects();

		int getSubjectId(const std::string& name);

		const std::string& getSubjectName(const int id);

		int addSubject(const std::string& name);

		bool renameSubject(const int id, const std::string& name);

		void removeSubject(const int id);

		int addSubject(std::string&& name);

		bool save() override;

		bool load() override;
	private:
		std::vector<std::string> subjects;

		SubjectManager() : PersistentDataSavable("data/subjects.csv") {}
		~SubjectManager() = default;

		SubjectManager(const SubjectManager& other) = delete;
		const SubjectManager& operator=(const SubjectManager& other) = delete;
	};
}
#endif
