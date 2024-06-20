#pragma once

#ifndef ESM_UTILS_HPP
#define ESM_UTILS_HPP

#include <vector>
#include <string>
#include <list>

namespace esm
{
	class Breadcrumb;

	void waitEnterPressed();

	void waitAnyKeyPressed();

	void printNavigator();

	void printCommands(Breadcrumb& breadcrumb);

	int selectOption(const std::vector<std::string>& options, int selecting = 0);

	void clearConsole();
}
#endif
