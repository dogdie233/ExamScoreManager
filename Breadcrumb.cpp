#pragma once
#include "Breadcrumb.hpp"

#include <string>
#include <vector>
#include <memory>
#include "Command.hpp"

namespace esm
{
	Breadcrumb::Breadcrumb(const std::string&& title) : title(title), selecting(0) {}

	void Breadcrumb::addCommand(std::unique_ptr<Command>&& pCommand) noexcept
	{
		this->commands.push_back(std::move(pCommand));
	}

	const std::vector<std::unique_ptr<Command>>& Breadcrumb::getCommands() const noexcept
	{
		return this->commands;
	}
}