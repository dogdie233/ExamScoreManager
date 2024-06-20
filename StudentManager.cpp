#pragma once
#include "StudentManager.hpp"

#include <vector>
#include <memory>
#include "Student.hpp"

namespace esm
{
	StudentManager& StudentManager::getInstance() noexcept
	{
		static StudentManager instance;
		return instance;
	}

	void StudentManager::addStudent(const StudentInfo& student)
	{
		students.push_back(std::make_shared<StudentInfo>(student));
	}

	std::vector<std::shared_ptr<StudentInfo>>& StudentManager::getStudents() noexcept
	{
		return students;
	}

	int StudentManager::addClass(const std::string& className)
	{
		return addClass(std::string(className));
	}

	int StudentManager::addClass(std::string&& className)
	{
		if (className.empty())
			return -1;

		int id = getClassId(className);
		if (id != -1)
			return id;

		classes.push_back(std::move(className));
		return classes.size() - 1;
	}

	int StudentManager::getClassId(const std::string& className)
	{
		return (find(classes.begin(), classes.end(), className) - classes.begin() + 1) % (classes.size() + 1) - 1;
	}

	std::vector<std::string>& StudentManager::getClasses() noexcept
	{
		return classes;
	}
}