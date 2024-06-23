#pragma once

#ifndef ESM_STUDENT_HPP
#define ESM_STUDENT_HPP

#include <string>

namespace esm
{
	struct StudentInfo
	{
	public:
		std::string name;
		std::string id;
		int gender = 0;
		int classId = -1;
	};

	struct ClassInfo
	{
	public:
		std::string title;
	};
}
#endif