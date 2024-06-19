#pragma once
#include <string>

namespace esm
{
	struct StudentInfo
	{
	public:
		std::string name;
		std::string id;
		int classId;
	};

	struct ClassInfo
	{
	public:
		std::string title;
	};

	struct ExamModel
	{
	public:
		int id;
		std::string title;
	};
}