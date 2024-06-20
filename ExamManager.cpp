#include "ExamManager.hpp"

#include <map>
#include <string>
#include <stdexcept>
#include "Student.hpp"

namespace esm
{
	ExamManager& ExamManager::getInstance()
	{
		static ExamManager instance;
		return instance;
	}

	ExamModel& ExamManager::newExam(const int id, const std::string title)
	{
		if (exams.find(id) != exams.end())
			throw std::runtime_error("Exam id already exists.");
		auto exam = ExamModel(id);
		exam.title = title;
		exams.insert(std::make_pair(id, exam));
		return exams[exams.size() - 1];
	}

	int ExamManager::nextAvailableId() noexcept
	{
		auto tail = exams.rbegin();
		return tail->first + 1;
	}
}