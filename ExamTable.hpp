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
	class ExamTable : public PersistentDataSavable
	{
	public:
		const int id;
		std::string title;

		void setStudentScore(const std::shared_ptr<StudentInfo>& pStudent, int subjectId, float score);

		float getStudentScore(const std::shared_ptr<StudentInfo>& pStudent, int subjectId) const;

		bool isStudentInTable(const std::shared_ptr<StudentInfo>& student) const;

		void removeSubject(int subjectId);

		std::vector<std::pair<std::shared_ptr<StudentInfo>, std::vector<float>>>& getTable() noexcept;

		ExamTable();
		ExamTable(const int id);
		ExamTable(const int id, const std::string& title);

		bool save() override;

		bool load() override;

	private:
		std::vector<std::pair<std::shared_ptr<StudentInfo>, std::vector<float>>> table;  // 看不懂，菜就多练

		ExamTable& operator=(const ExamTable& other) = delete;
	};
}

#endif // !ESM_EXAM_TABLE_HPP
