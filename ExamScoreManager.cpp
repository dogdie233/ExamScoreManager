#pragma once
#include <iostream>

#include "Utils.hpp"
#include "Commands.hpp"
#include "Navigator.hpp"
#include "Breadcrumb.hpp"
#include "ConsoleUtils.hpp"
#include "SubjectManager.hpp"

namespace esm
{
	static void addMain()
	{
		auto breadcrumb = Breadcrumb("主页");
		breadcrumb.addCommand(std::make_unique<BackCommand>("退出程序"));
		breadcrumb.addCommand(std::make_unique<SubjectManagementCommand>());
		breadcrumb.addCommand(std::make_unique<StudentManagementCommand>());
		breadcrumb.addCommand(std::make_unique<ExamManagerCommand>());
		breadcrumb.addCommand(std::make_unique<TestCommand>("测试", 1));
		Navigator::getInstance().push(std::move(breadcrumb));
	}
}

using namespace esm;

int main()
{
	con::initConsole();

	std::cout << "科目信息加载" << (SubjectManager::getInstance().load() ? "成功" : "失败");
	waitAnyKeyPressed();

	addMain();
	while (!Navigator::getInstance().getAll().empty())
	{
		clearConsole();
		printNavigator();
		auto& tail = *Navigator::getInstance().getAll().rbegin();

		printCommands(tail);
	}
	std::cout << "程序已退出" << std::endl;
	return 0;
}