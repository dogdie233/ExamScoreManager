#include <vector>
#include "Student.hpp"
#include <algorithm>

namespace esm
{
	class StudentManager
	{
	public:
		static StudentManager& getInstance()
		{
			static StudentManager instance;
			return instance;
		}

		void addStudent(const StudentInfo& student)
		{
			students.push_back(student);
		}

		std::vector<StudentInfo>& getStudents() noexcept
		{
			return students;
		}

		std::vector<StudentInfo>::const_iterator begin() const noexcept
		{
			return students.begin();
		}

		std::vector<StudentInfo>::const_iterator end() const noexcept
		{
			return students.end();
		}

	private:
		std::vector<StudentInfo> students;

		StudentManager() = default;
		~StudentManager() = default;

		StudentManager(const StudentManager& other) = delete;
		const StudentManager& operator=(const StudentManager& other) = delete;
	};
}