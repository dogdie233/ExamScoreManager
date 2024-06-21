#include "ConsoleUtils.hpp"
#include <locale>
#include <Windows.h>
#include <fcntl.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <corecrt_io.h>

namespace con
{
	void initConsole()
	{
		DWORD mode;
		SetConsoleCP(CP_UTF8);
		GetConsoleMode(stdinHandle, &mode);
	    SetConsoleMode(stdinHandle, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
		std::locale::global(std::locale("en_US.UTF-8"));
		std::cin.imbue(std::locale("en_US.UTF-8"));
		std::cout.imbue(std::locale("en_US.UTF-8"));
	}

	void getCursorPosition(int& x, int& y)
	{
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
		x = csbi.dwCursorPosition.X;
		y = csbi.dwCursorPosition.Y;
	}

	void setCursorPosition(const int x, const int y)
	{
		COORD coord = { x, y };
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
	}

	void hideCursor()
	{
		std::cout << "\033[?25l" << std::flush;
	}

	void showCursor()
	{
		std::cout << "\033[?25h" << std::flush;
	}

	std::ostream& lineClear(std::ostream& _Ostr)
	{
		_Ostr << "\033[2K";
		return _Ostr;
	}

	std::ostream& textGreen(std::ostream& _Ostr)
	{
		_Ostr << "\033[32m";
		return _Ostr;
	}

	std::ostream& textRed(std::ostream& _Ostr)
	{
		_Ostr << "\033[31m";
		return _Ostr;
	}

	std::ostream& textYellow(std::ostream& _Ostr)
	{
		_Ostr << "\033[33m";
		return _Ostr;
	}

	std::ostream& textBold(std::ostream& _Ostr)
	{
		_Ostr << "\033[1m";
		return _Ostr;
	}

	std::ostream& textColorDefault(std::ostream& _Ostr)
	{
		_Ostr << "\033[39m";
		return _Ostr;
	}

	CHA cha(int pos)
	{
		return CHA(pos);
	}

	std::string chaCmd(int pos)
	{
		return "\033[" + std::to_string(pos) + "G";
	}
}