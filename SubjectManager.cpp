#include "SubjectManager.hpp"

#include <vector>
#include <string>

namespace esm
{
	SubjectManager& SubjectManager::getInstance()
	{
		static SubjectManager instance;
		return instance;
	}

	std::vector<std::string>& SubjectManager::getSubjects()
	{
		return subjects;
	}

	int SubjectManager::getSubjectId(const std::string& name)
	{
		return (find(subjects.begin(), subjects.end(), name) - subjects.begin() + 1) % (subjects.size() + 1) - 1;
	}

	const std::string& SubjectManager::getSubjectName(const int id)
	{
		return subjects[id];
	}

	int SubjectManager::addSubject(const std::string& name)
	{
		return addSubject(std::string(name));
	}

	bool SubjectManager::renameSubject(const int id, const std::string& name)
	{
		if (id < 0 || id >= subjects.size() || getSubjectId(name) != -1)
			return false;

		subjects[id] = name;
		return true;
	}

	void SubjectManager::removeSubject(const int id)
	{
		if (id < 0 || id >= subjects.size())
			return;

		subjects[id] = "";
	}

	int SubjectManager::addSubject(std::string&& name)
	{
		if (name.empty())
			return -1;

		int id = getSubjectId(name);
		if (id != -1)
			return id;

		for (int i = 0; i < subjects.size(); i++)
		{
			if (subjects[i].empty())
			{
				subjects[i] = name;
				return i;
			}
		}

		subjects.push_back(std::move(name));
		return subjects.size() - 1;
	}
}