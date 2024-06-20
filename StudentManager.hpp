#pragma once

#ifndef ESM_STUDENT_MANAGER_HPP
#define ESM_STUDENT_MANAGER_HPP

#include <vector>
#include <memory>
#include "Student.hpp"

namespace esm
{
	class StudentManager
	{
	public:
		static StudentManager& getInstance() noexcept;

		void addStudent(const StudentInfo& student);

		std::vector<std::shared_ptr<StudentInfo>>& getStudents() noexcept;

		int addClass(const std::string& className);

		int addClass(std::string&& className);

		int getClassId(const std::string& className);

		std::vector<std::string>& getClasses() noexcept;

	private:
		std::vector<std::shared_ptr<StudentInfo>> students;
		std::vector<std::string> classes;

		StudentManager() = default;
		~StudentManager() = default;

		StudentManager(const StudentManager& other) = delete;
		const StudentManager& operator=(const StudentManager& other) = delete;
	};
}
#endif
