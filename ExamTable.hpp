#pragma once
#ifndef ESM_EXAM_TABLE_HPP
#define ESM_EXAM_TABLE_HPP

#include <map>
#include <vector>
#include <string>
#include "Student.hpp"
#include "PersistentData.hpp"

namespace esm
{
	struct ExamScoreRecord
	{
		std::shared_ptr<StudentInfo> pStudent;
		std::vector<float> scores;

		ExamScoreRecord() : pStudent(nullptr), scores(){}
		ExamScoreRecord(const std::shared_ptr<StudentInfo>& pStudent, std::vector<float>&& scores)
			: pStudent(pStudent), scores{ scores } {}
	};

	class ExamTable : public PersistentDataSavable
	{
	public:
		const int id;
		std::vector<int> subjects;
		std::string title;

		void setStudentScore(const std::shared_ptr<StudentInfo>& pStudent, int subjectId, float score);

		float getStudentScore(const std::shared_ptr<StudentInfo>& pStudent, int subjectId) const;

		bool isStudentInTable(const std::shared_ptr<StudentInfo>& student) const;

		void addSubject(int subjectId);

		void removeSubject(int subjectId);

		bool removeStudent(std::shared_ptr<StudentInfo> pStu) noexcept;

		std::vector<ExamScoreRecord>& getTable() noexcept;

		ExamTable();
		ExamTable(const int id);
		ExamTable(const int id, const std::string& title);
		ExamTable(const int id, std::vector<int>&& subjects, const std::string& title);

		bool save() override;

		bool load() override;

	private:
		std::vector<ExamScoreRecord> table;

		ExamTable& operator=(const ExamTable& other) = delete;
	};
}

#endif // !ESM_EXAM_TABLE_HPP
