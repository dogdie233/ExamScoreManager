#include "ExamManager.hpp"

#include <map>
#include <string>
#include <stdexcept>
#include "Student.hpp"
#include "Utils.hpp"

namespace esm
{
	ExamManager& ExamManager::getInstance()
	{
		static ExamManager instance;
		return instance;
	}

	std::shared_ptr<ExamTable> ExamManager::newExam(const int id, const std::string& title)
	{
		if (exams.find(id) != exams.end())
			throw std::runtime_error("Exam id already exists.");
		auto exam = std::make_shared<ExamTable>(ExamTable(id));
		exam->title = title;
		exams.insert(std::make_pair(id, exam));
		save();
		return exams[exams.size() - 1];
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

	std::map<int, std::shared_ptr<ExamTable>> ExamManager::getExams()
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
			writer << kvp.first << kvp.second->title << csv::endl;
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
			std::string title;
			reader >> id >> title;
			exams.insert(std::make_pair(id, std::make_shared<ExamTable>(ExamTable(id, title))));
		}
		return true;
	}
}