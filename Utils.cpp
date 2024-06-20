#include "Utils.hpp"
#include "Breadcrumb.hpp"
#include "Command.hpp"
#include "ConsoleUtils.hpp"
#include "Navigator.hpp"
#include <iostream>
#include <list>
#include <conio.h>

namespace esm
{
	void waitEnterPressed()
	{
		while (_getch() != 13);
	}

	void waitAnyKeyPressed()
	{
		int _ = _getch();
	}

	void printNavigator()
	{
		std::cout << "当前位置：";
		auto& navigator = Navigator::getInstance().getAll();
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

	void printCommands(Breadcrumb& breadcrumb)
	{
		if (breadcrumb.getCommands().size() == 0)
			throw std::runtime_error("No commands available.");

		if (breadcrumb.selecting < 0 || breadcrumb.selecting >= breadcrumb.getCommands().size())
			breadcrumb.selecting = 0;
		int x, y;
		con::getCursorPosition(x, y);
		con::hideCursor();
		bool confirm = false;
		while (!confirm)
		{
			std::cout << con::lineClear << "================\n";
			auto iter = breadcrumb.getCommands().begin();
			int id = 0;
			while (iter != breadcrumb.getCommands().end())
			{
				auto& pCmd = *iter;
				std::cout << con::lineClear;
				if (id == breadcrumb.selecting)
					std::cout << con::textGreen << pCmd->title << con::textColorDefault;
				else
					std::cout << pCmd->title;
				std::cout << '\n';
				++iter;
				++id;
			}
			std::cout << con::lineClear << "================\n";
			std::cout << con::lineClear << "通过↑↓方向键选择命令，回车键执行当前选中命令（绿色）" << std::endl;

			while (1)
			{
				int key = _getch();
				if (key == 13)
				{
					confirm = true;
					break;
				}
				if (key != 224)
					continue;
				key = _getch();
				if (key == 72)
				{
					breadcrumb.selecting = (breadcrumb.selecting - 1 + breadcrumb.getCommands().size()) % breadcrumb.getCommands().size();
					break;
				}
				else if (key == 80)
				{
					breadcrumb.selecting = (breadcrumb.selecting + 1) % breadcrumb.getCommands().size();
					break;
				}
			}
			if (!confirm)
				con::setCursorPosition(x, y);
		}

		con::showCursor();
		breadcrumb.getCommands()[breadcrumb.selecting]->Invoke();
	}

	int selectOption(const std::vector<std::string>& options, int selecting)
	{
		std::cout << con::cha(1) << std::flush;
		int x, y;
		con::getCursorPosition(x, y);
		con::hideCursor();
		bool confirm = false;
		while (!confirm)
		{
			for (int i = 0; i < options.size(); i++)
			{
				std::cout << con::lineClear;
				if (i == selecting)
					std::cout << con::textGreen << options[i] << con::textColorDefault;
				else
					std::cout << options[i];
				std::cout << '\n';
			}
			con::setCursorPosition(x, y);

			while (1)
			{
				int key = _getch();
				if (key == 13)
				{
					confirm = true;
					break;
				}
				if (key != 224)
					continue;
				key = _getch();
				if (key == 72)
				{
					selecting = (selecting - 1 + options.size()) % options.size();
					break;
				}
				else if (key == 80)
				{
					selecting = (selecting + 1) % options.size();
					break;
				}
			}
		}

		for (int i = 0; i < options.size(); i++)
			std::cout << con::lineClear << '\n';
		con::setCursorPosition(x, y);
		con::showCursor();
		return selecting;
	}

	void clearConsole()
	{
		system("cls");
	}
}