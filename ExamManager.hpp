#pragma once
#include <vector>
#include <string>
#include "Student.hpp"

namespace esm
{
	class ExamManager
	{
	public:
		static ExamManager& getInstance()
		{
			static ExamManager instance;
			return instance;
		}

		void newExam(const std::string title)
		{
			auto exam = ExamModel();
			exam.id = exams.size();
			exam.title = title;
		}

	private:
		std::vector<ExamManager> exams;

		ExamManager() = default;
		~ExamManager() = default;

		ExamManager(const ExamManager& other) = delete;
		const ExamManager& operator=(const ExamManager& other) = delete;
	}
}