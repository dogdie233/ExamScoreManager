#pragma once

#ifndef ESM_CONSOLEUTILS_HPP
#define ESM_CONSOLEUTILS_HPP

#include <Windows.h>
#include <iostream>

namespace con
{
	static HANDLE stdinHandle = GetStdHandle(STD_INPUT_HANDLE);

	void initConsole();

	void getCursorPosition(int& x, int& y);

	void setCursorPosition(const int x, const int y);

	void hideCursor();

	void showCursor();

	std::ostream& lineClear(std::ostream& _Ostr);

	std::ostream& textGreen(std::ostream& _Ostr);

	std::ostream& textRed(std::ostream& _Ostr);

	std::ostream& textYellow(std::ostream& _Ostr);

	std::ostream& textBold(std::ostream& _Ostr);

	std::ostream& textColorDefault(std::ostream& _Ostr);

	struct CHA
	{
		int pos;
		friend std::ostream& operator<<(std::ostream& os, const CHA& cha)
		{
			os << "\033[" << cha.pos << "G";
			return os;
		}

		friend std::wostream& operator<<(std::wostream& os, const CHA& cha)
		{
			os << L"\033[" << cha.pos << L"G";
			return os;
		}
	};

	/// <summary>
	/// 设置光标行内绝对位置
	/// </summary>
	CHA cha(int pos);

	std::string chaCmd(int pos);
}
#endif
