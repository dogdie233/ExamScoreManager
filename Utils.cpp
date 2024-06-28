#include "Utils.hpp"

#include "Breadcrumb.hpp"
#include "Command.hpp"
#include "ConsoleUtils.hpp"
#include "Navigator.hpp"
#include "Student.hpp"

#include <iostream>
#include <list>
#include <conio.h>
#include <algorithm>

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
			std::cout << con::lineClear << std::flush;
			con::fillCharToEnd('=');
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
			std::cout << con::lineClear << std::flush;
			con::fillCharToEnd('=');
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

	int selectMultiOptions(const std::vector<std::string>& options, std::vector<bool>& selections, int focusing)
	{
		if (options.size() != selections.size())
			throw std::runtime_error("Size of options and selections not match.");

		std::cout << con::cha(1) << std::flush;
		int x, y;
		con::getCursorPosition(x, y);
		con::hideCursor();
		bool confirm = false;
		int optionsCount = options.size() + 1;
		while (!confirm)
		{
			for (int i = 0; i < options.size(); i++)
			{
				std::cout << con::lineClear;
				if (focusing == i)
					std::cout << con::textGreen;
				std::cout << (selections[i] ? "√ " : "  ") << options[i];
				if (focusing == i)
					std::cout << con::textColorDefault;
				std::cout << '\n';
			}
			if (focusing == optionsCount - 1)
				std::cout << con::textGreen << "确认" << con::textColorDefault << std::endl;
			else
				std::cout << "确认" << std::endl;
			con::setCursorPosition(x, y);

			while (1)
			{
				int key = _getch();
				if (key == 13)
				{
					if (focusing == optionsCount - 1)
						confirm = true;
					else
						selections[focusing] = !selections[focusing];
					break;
				}
				if (key != 224)
					continue;
				key = _getch();
				if (key == 72)
				{
					focusing = (focusing - 1 + optionsCount) % optionsCount;
					break;
				}
				else if (key == 80)
				{
					focusing = (focusing + 1) % optionsCount;
					break;
				}
			}
		}

		for (int i = 0; i < optionsCount; i++)
			std::cout << con::lineClear << '\n';
		con::setCursorPosition(x, y);
		con::showCursor();
		return focusing;
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
	
	bool CreateCsvWriterSafe(const PersistentData& persistentData, csv::CsvWriter& writer)
	{
		csv::CsvWriter newWriter(nullptr);
		try
		{
			newWriter = persistentData.CreateWriter();
			writer = newWriter;
			return true;
		}
		catch (const std::exception& e)
		{
			std::cerr << "Error when opening subject save data: " << e.what() << std::endl;
			return false;
		}
	}

	bool CreateCsvReaderSafe(const PersistentData& persistentData, csv::CsvReader& reader)
	{
		csv::CsvReader newReader(nullptr);
		try
		{
			newReader = persistentData.CreateReader();
			reader = newReader;
			return true;
		}
		catch (const std::exception& e)
		{
			std::cerr << "Error when opening save data: " << e.what() << std::endl;
			return false;
		}
	}

	bool ConfirmDeleteAction(const std::string& itemType, const std::string& itemName)
	{
		char otp = rand() % 26 + 'a';

		std::cout << con::textYellow << "你正在执行删除" << itemType << "操作";
		if (!itemName.empty())
			std::cout << "，对象为\"" << itemName << "\"";
		std::cout << std::endl;
		std::cout << "这项操作十分重要，会造成重大影响且不可恢复，确定要执行吗？" << std::endl;
		std::cout << "确定请输入 " << con::textBold << otp << con::textColorDefault << ": ";
		char input;
		std::cin >> input;
		std::cin.clear();

		if (input == otp)
			std::cout << con::textGreen << "√ 操作已确认，正在执行删除操作..." << con::textColorDefault << std::endl;
		else
			std::cout << con::textRed << "× 已取消删除操作" << con::textColorDefault << std::endl;
		return input == otp;
	}

	bool defaultStudentPtrComparator(const std::shared_ptr<StudentInfo>& lhs, const std::shared_ptr<StudentInfo>& rhs)
	{
		return lhs->classId < rhs->classId || (lhs->classId == rhs->classId && lhs->id < rhs->id);
	}

	bool defaultStudentComparator(const std::shared_ptr<StudentInfo>& lhs, const StudentInfo& rhs)
	{
		return lhs->classId < rhs.classId || (lhs->classId == rhs.classId && lhs->id < rhs.id);
	}
}