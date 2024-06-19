#pragma once

#include <iostream>
#include <list>
#include "Breadcrumb.hpp"

namespace esm
{
	static std::list<Breadcrumb> navigator;

	static void printNavigator()
	{
		std::cout << "当前位置：";
		auto iter = navigator.begin();
		while (iter != navigator.end())
		{
			auto& bread = *iter;
			if (iter != navigator.begin())
				std::cout << " > ";
			std::cout << bread.title;
			++iter;
		}
		std::cout << std::endl;
	}

	static void printCommands(const Breadcrumb& breadcrumb)
	{
		std::cout << "================" << '\n';
		auto iter = breadcrumb.getCommands().begin();
		std::cout << "可用命令：" << '\n';
		int id = 1;
		while (iter != breadcrumb.getCommands().end())
		{
			auto& pCmd = *iter;
			std::cout << id << ". " << pCmd->title << '\n';
			++iter;
			++id;
		}
		std::cout << "================" << '\n';
	}

	static void askAndInvokeCommands(const Breadcrumb& breadcrumb)
	{
		int id;
		while (true)
		{
			std::cout << "请输入命令编号：";
			std::cin >> id;
			if (id < 1 || id > breadcrumb.getCommands().size())
			{
				std::cout << "无效的命令编号，请重新输入" << std::endl;
				continue;
			}

			auto& pCmd = breadcrumb.getCommands()[id - 1];
			pCmd->Invoke();
			break;
		}
	}

	static void clearConsole()
	{
		system("cls");
	}
}