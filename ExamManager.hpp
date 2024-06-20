#pragma once

#ifndef ESM_EXAMMANAGER_HPP
#define ESM_EXAMMANAGER_HPP

#include <map>
#include <string>
#include <stdexcept>
#include "Student.hpp"

namespace esm
{
	class ExamManager
	{
	public:
		static ExamManager& getInstance();

		ExamModel& newExam(const int id, const std::string title);

		int nextAvailableId() noexcept;

	private:
		std::map<int, ExamModel> exams;

		ExamManager() = default;
		~ExamManager() = default;

		ExamManager(const ExamManager& other) = delete;
		const ExamManager& operator=(const ExamManager& other) = delete;
	};
}
#endif // !EXAMMANAGER_HPP