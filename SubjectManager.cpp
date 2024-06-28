#include "SubjectManager.hpp"
#include "Utils.hpp"

#include <vector>
#include <string>

namespace esm
{
	static std::string emptyString = std::string();

	SubjectManager& SubjectManager::getInstance() noexcept
	{
		static SubjectManager instance;
		return instance;
	}

	std::map<int, std::string>& SubjectManager::getSubjects() noexcept
	{
		return subjects;
	}

	int SubjectManager::getSubjectId(const std::string& name) noexcept
	{
		for (auto& kvp : subjects)
		{
			if (kvp.second == name)
				return kvp.first;
		}
		return -1;
	}

	const std::string& SubjectManager::getSubjectName(const int id) noexcept
	{
		auto it = subjects.find(id);
		return it == subjects.end() ? emptyString : it->second;
	}

	int SubjectManager::addSubject(const std::string& name) noexcept
	{
		return addSubject(std::string(name));
	}

	int SubjectManager::addSubject(std::string&& name) noexcept
	{
		if (name.empty())
			return -1;

		int id = subjects.empty() ? 0 : subjects.crbegin()->first + 1;
		subjects[id] = name;
		save();
		return id;
	}

	bool SubjectManager::isSubjectExist(int id) noexcept
	{
		return id >= 0 && id < subjects.size() && !subjects[id].empty();
	}

	bool SubjectManager::renameSubject(const int id, const std::string& name)
	{
		if (id < 0 || id >= subjects.size() || getSubjectId(name) != -1)
			return false;

		subjects[id] = name;
		save();
		return true;
	}

	void SubjectManager::removeSubject(const int id) noexcept
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

		for (const auto& kvp : subjects)
		{
			writer << kvp.first << kvp.second << csv::endl;
		}
		return true;
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
			int id = 0;
			reader >> id >> title;
			subjects[id] = title;
		}

		return true;
	}
}