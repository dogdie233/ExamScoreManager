#include "SubjectManager.hpp"
#include "Utils.hpp"

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
				save();
				return i;
			}
		}

		subjects.push_back(std::move(name));
		save();
		return subjects.size() - 1;
	}

	bool SubjectManager::renameSubject(const int id, const std::string& name)
	{
		if (id < 0 || id >= subjects.size() || getSubjectId(name) != -1)
			return false;

		subjects[id] = name;
		save();
		return true;
	}

	void SubjectManager::removeSubject(const int id)
	{
		if (id < 0 || id >= subjects.size())
			return;

		subjects[id] = "";
		save();
	}

	bool SubjectManager::save()
	{
		csv::CsvWriter writer(nullptr);
		if (!CreateCsvWriterSafe(this->persistentData, writer))
			return false;

		for (int i = 0; i < subjects.size(); i++)
		{
			if (!subjects[i].empty())
				writer << i << subjects[i] << csv::endl;
		}
	}

	bool SubjectManager::load()
	{
		csv::CsvReader reader(nullptr);
		if (!CreateCsvReaderSafe(this->persistentData, reader))
			return false;

		subjects.clear();
		while (reader.hasNext())
		{
			std::string title;
			int id;
			reader >> id >> title;
			while (subjects.size() <= id)
				subjects.push_back("");
			subjects[id] = title;
		}

		return true;
	}
}