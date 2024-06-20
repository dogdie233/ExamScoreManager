#pragma once

#ifndef ESM_SUBJECTMANAGER_HPP
#define ESM_SUBJECTMANAGER_HPP

#include <vector>
#include <string>

namespace esm
{
	class SubjectManager
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

	private:
		std::vector<std::string> subjects;

		SubjectManager() = default;
		~SubjectManager() = default;

		SubjectManager(const SubjectManager& other) = delete;
		const SubjectManager& operator=(const SubjectManager& other) = delete;
	};
}
#endif
