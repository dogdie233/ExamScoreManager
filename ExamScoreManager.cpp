#pragma once
#include <iostream>

#include "Utils.hpp"
#include "Commands.hpp"
#include "Navigator.hpp"
#include "Breadcrumb.hpp"
#include "ConsoleUtils.hpp"
#include "SubjectManager.hpp"
#include "ClassManager.hpp"
#include "StudentManager.hpp"
#include "ExamManager.hpp"

namespace esm
{
	static void addMain()
	{
		auto breadcrumb = Breadcrumb("主页");
		breadcrumb.addCommand(std::make_unique<BackCommand>("退出程序"));
		breadcrumb.addCommand(std::make_unique<SubjectManagementCommand>());
		breadcrumb.addCommand(std::make_unique<StudentManagementCommand>());
		breadcrumb.addCommand(std::make_unique<ExamManagementCommand>());
		breadcrumb.addCommand(std::make_unique<TestCommand>("测试", 1));
		Navigator::getInstance().push(std::move(breadcrumb));
	}
}

using namespace esm;

int main()
{
	con::initConsole();

	std::cout << "科目信息加载" << (SubjectManager::getInstance().load() ? "成功" : "失败") << std::endl;
	std::cout << "班级信息加载" << (ClassManager::getInstance().load() ? "成功" : "失败") << std::endl;
	std::cout << "学生信息加载" << (StudentManager::getInstance().load() ? "成功" : "失败") << std::endl;
	std::cout << "考试信息加载" << (ExamManager::getInstance().load() ? "成功" : "失败") << std::endl;
	for (auto& exam : ExamManager::getInstance().getExams())
		exam.second->load();
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
	waitAnyKeyPressed();
	return 0;
}