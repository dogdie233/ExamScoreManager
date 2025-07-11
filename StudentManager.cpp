#pragma once
#include "StudentManager.hpp"

#include <algorithm>
#include <regex>
#include <vector>
#include <memory>
#include "Student.hpp"
#include "Utils.hpp"

namespace esm
{
	StudentManager& StudentManager::getInstance() noexcept
	{
		static StudentManager instance;
		return instance;
	}

	std::shared_ptr<StudentInfo> StudentManager::getStudentById(const std::string& id)
	{
		for (auto& student : students)
		{
			if (student->id == id)
				return student;
		}
		return nullptr;
	}

	std::vector<std::shared_ptr<StudentInfo>> StudentManager::getStudentsByNameRegex(const std::string& regex)
	{
		std::vector<std::shared_ptr<StudentInfo>> result;
		std::regex re(regex);
		for (auto& student : students)
		{
			if (std::regex_search(student->name, re))
				result.push_back(student);
		}
		return result;
	}

	void StudentManager::addStudent(const StudentInfo& student)
	{
		auto pStudent = std::make_shared<StudentInfo>(StudentInfo(student));
		auto next = std::upper_bound(students.begin(), students.end(), pStudent, defaultStudentPtrComparator);
		students.insert(next, std::move(pStudent));
		save();
	}

	std::vector<std::shared_ptr<StudentInfo>>& StudentManager::getStudents() noexcept
	{
		return students;
	}

	void StudentManager::removeStudent(std::shared_ptr<StudentInfo>&& pStudent)
	{
		students.erase(std::remove(students.begin(), students.end(), pStudent), students.end());
		save();
	}

	bool StudentManager::save()
	{
		csv::CsvWriter writer(nullptr);
		if (!CreateCsvWriterSafe(this->persistentData, writer))
			return false;

		for (auto& stu : students)
			writer << stu->id << stu->name << stu->gender << stu->classId << csv::endl;
		return true;
	}

	bool StudentManager::load()
	{
		csv::CsvReader reader(nullptr);
		if (!CreateCsvReaderSafe(this->persistentData, reader))
			return false;

		students.clear();
		while (reader.hasNext())
		{
			StudentInfo stu;
			reader >> stu.id >> stu.name >> stu.gender >> stu.classId;
			students.push_back(std::make_shared<StudentInfo>(stu));
		}
		std::sort(students.begin(), students.end(), defaultStudentPtrComparator);
		return true;
	}
}