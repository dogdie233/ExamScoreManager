#pragma once

#ifndef ESM_BREADCRUMB_HPP
#define ESM_BREADCRUMB_HPP

#include "Command.hpp"
#include <string>
#include <vector>
#include <memory>

namespace esm
{
	class Breadcrumb
	{
	public:
		std::string title;
		int selecting;

		Breadcrumb(const std::string&& title);

		void addCommand(std::unique_ptr<Command>&& pCommand) noexcept;

		const std::vector<std::unique_ptr<Command>>& getCommands() const noexcept;
	private:
		std::vector<std::unique_ptr<Command>> commands;
	};
}
#endif