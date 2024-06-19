#pragma once

#include <string>
#include <vector>
#include <memory>
#include "Command.hpp"

namespace esm
{
	class Breadcrumb
	{
	public:
		std::string title;

		Breadcrumb(const std::string&& title) : title(title), commands(0) {}

		void addCommand(std::unique_ptr<Command>&& pCommand)
		{
			this->commands.push_back(std::move(pCommand));
		}

		const std::vector<std::unique_ptr<Command>>& getCommands() const
		{
			return this->commands;
		}
	private:
		std::vector<std::unique_ptr<Command>> commands;
	};
}