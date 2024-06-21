#pragma once

#ifndef ESM_STUDENT_MANAGER_HPP
#define ESM_STUDENT_MANAGER_HPP

#include <vector>
#include <memory>
#include "Student.hpp"
#include "PersistentData.hpp"

namespace esm
{
	class StudentManager : public PersistentDataSavable
	{
	public:
		static StudentManager& getInstance() noexcept;

		std::shared_ptr<StudentInfo> getStudentById(const std::string& id);

		std::vector<std::shared_ptr<StudentInfo>> getStudentsByNameRegex(const std::string& regex);

		void addStudent(const StudentInfo& student);

		std::vector<std::shared_ptr<StudentInfo>>& getStudents() noexcept;

		void removeStudent(std::shared_ptr<StudentInfo>&& pStudent);

		bool save() override;

		bool load() override;

	private:
		std::vector<std::shared_ptr<StudentInfo>> students;

		StudentManager() : PersistentDataSavable("data/students.csv") {}
		~StudentManager() = default;

		StudentManager(const StudentManager& other) = delete;
		const StudentManager& operator=(const StudentManager& other) = delete;
	};
}
#endif
