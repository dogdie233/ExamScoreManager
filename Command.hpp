#pragma once

#ifndef ESM_COMMAND_HPP
#define ESM_COMMAND_HPP

#include <string>

namespace esm
{
	class Command
	{
	public:
		std::string title;

		Command(const std::string& title) : title(title) {}
		Command(std::string&& title) : title(std::move(title)) {}

		virtual void Invoke() = 0;
	};
}
#endif

