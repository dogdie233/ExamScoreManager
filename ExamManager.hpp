#pragma once

#ifndef ESM_EXAMMANAGER_HPP
#define ESM_EXAMMANAGER_HPP

#include <map>
#include <string>
#include <stdexcept>
#include "Student.hpp"
#include "PersistentData.hpp"
#include "ExamTable.hpp"

namespace esm
{
	class ExamManager : public PersistentDataSavable
	{
	public:
		static ExamManager& getInstance() noexcept;

		std::shared_ptr<ExamTable> newExam(const int id, const std::string& title);

		void removeExam(const int id);

		void removeSubject(const int subjectId) noexcept;

		std::map<int, std::shared_ptr<ExamTable>> getExams() noexcept;

		int nextAvailableId() noexcept;

		bool save() override;

		bool load() override;

	private:
		std::map<int, std::shared_ptr<ExamTable>> exams;

		ExamManager() : PersistentDataSavable("data/exams.csv") {}
		~ExamManager() = default;

		ExamManager(const ExamManager& other) = delete;
		const ExamManager& operator=(const ExamManager& other) = delete;
	};
}
#endif // !EXAMMANAGER_HPP