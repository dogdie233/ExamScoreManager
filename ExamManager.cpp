#include "ExamManager.hpp"

#include <map>
#include <string>
#include <sstream>
#include <stdexcept>
#include "Student.hpp"
#include "Utils.hpp"

namespace esm
{
	ExamManager& ExamManager::getInstance() noexcept
	{
		static ExamManager instance;
		return instance;
	}

	std::shared_ptr<ExamTable> ExamManager::newExam(const int id, const std::string& title)
	{
		if (exams.find(id) != exams.end())
			throw std::runtime_error("Exam id already exists.");
		auto exam = std::make_shared<ExamTable>(ExamTable(id, title));
		exam->title = title;
		exams.insert(std::make_pair(id, exam));
		save();
		exams[id]->save();
		return exams[id];
	}

	void ExamManager::removeExam(const int id)
	{
		auto it = exams.find(id);
		if (it != exams.end())
		{
			it->second->deleteSave();
			exams.erase(it);
			save();
		}
	}

	void ExamManager::removeSubject(const int subjectId) noexcept
	{
		for (auto& kvp : exams)
		{
			kvp.second->removeSubject(subjectId);
			kvp.second->save();
		}
	}

	std::map<int, std::shared_ptr<ExamTable>> ExamManager::getExams() noexcept
	{
		return this->exams;
	}

	int ExamManager::nextAvailableId() noexcept
	{
		if (exams.size() == 0)
			return 0;
		auto tail = exams.rbegin();
		return tail->first + 1;
	}

	bool ExamManager::save()
	{
		auto writer = csv::CsvWriter(nullptr);
		if (!CreateCsvWriterSafe(this->persistentData, writer))
			return false;

		for (auto& kvp : exams)
		{
			std::stringstream ss;
			if (kvp.second->subjects.size() > 0)
				ss << kvp.second->subjects[0];
			for (int i = 1; i < kvp.second->subjects.size(); i++)
				ss << ',' << kvp.second->subjects[i];
			writer << kvp.first << kvp.second->title << ss.str() << csv::endl;
		}
		return true;
	}

	bool ExamManager::load()
	{
		auto reader = csv::CsvReader(nullptr);
		if (!CreateCsvReaderSafe(this->persistentData, reader))
			return false;

		exams.clear();
		while (reader.hasNext())
		{
			int id;
			std::string title, subjects;
			reader >> id >> title >> subjects;
			std::stringstream ss(subjects);
			std::vector<int> subjectIds;
			while (!ss.eof())
			{
				if (ss.peek() == ',')
					ss.ignore(1);
				int subjectId;
				ss >> subjectId;
				subjectIds.push_back(subjectId);
			}

			exams.insert(std::make_pair(id, std::make_shared<ExamTable>(ExamTable(id, std::move(subjectIds), title))));
		}
		return true;
	}
}