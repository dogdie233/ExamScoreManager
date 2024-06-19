#pragma once

#include <iostream>
#include <list>
#include "Breadcrumb.hpp"

namespace esm
{
	static std::list<Breadcrumb> navigator;

	static void printNavigator()
	{
		std::cout << "��ǰλ�ã�";
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
		std::cout << "�������" << '\n';
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
			std::cout << "�����������ţ�";
			std::cin >> id;
			if (id < 1 || id > breadcrumb.getCommands().size())
			{
				std::cout << "��Ч�������ţ�����������" << std::endl;
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