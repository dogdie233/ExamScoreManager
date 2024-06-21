#pragma once

#ifndef ESM_STUDENT_HPP
#define ESM_STUDENT_HPP

#include <string>
#include <vector>
#include <memory>
#include <map>

namespace esm
{
	struct StudentInfo
	{
	public:
		std::string name;
		std::string id;
		int classId = -1;
	};

	struct ClassInfo
	{
	public:
		std::string title;
	};

	class ExamModel
	{
	public:
		const int id;
		std::string title;

		void AddScoreInfo(std::pair<std::shared_ptr<StudentInfo>, std::vector<float>>&& pair)
		{
			scores.push_back(std::move(pair));
		}

		std::vector<std::pair<std::shared_ptr<StudentInfo>, std::vector<float>>>& getScores()
		{
			return scores;
		}

		ExamModel() : id(-1) {}
		ExamModel(const int id) : id(id) {}
		ExamModel(const ExamModel& other) : id(other.id), title(other.title) {}
		ExamModel& operator=(const ExamModel& other)
		{
			if (this != &other)
			{
				title = other.title;
				scores = other.scores;
			}
			return *this;
		}
	private:
		std::vector<std::pair<std::shared_ptr<StudentInfo>, std::vector<float>>> scores;  // 看不懂，菜就多练
	};
}
#endif