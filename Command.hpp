#pragma once

#include <string>

namespace esm
{
	class Command
	{
	public:
		std::string title;

		Command(const std::string& title) : title(title) {}
		Command(std::string&& title)
		{
			this->title = title;
		}

		virtual void Invoke() = 0;
	};
}