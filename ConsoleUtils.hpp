// 在控制台中读取utf8参考了 https://chariri.moe/archives/408/windows-cin-read-utf8/

#pragma once

#ifndef ESM_CONSOLEUTILS_HPP
#define ESM_CONSOLEUTILS_HPP

#include <iostream>

namespace con
{
	constexpr unsigned default_win_utf8_cin_buf_size = 2048;

	class WinUtf8StreamBuf : public std::streambuf
	{
	public:
		WinUtf8StreamBuf(unsigned read_buf_size = default_win_utf8_cin_buf_size, bool handle_console_eof = false);

	protected:
		int_type underflow() override;

	private:
		unsigned _read_buf_size, _u8_buf_size, _wide_buf_size;
		std::unique_ptr<char[]> _buffer;
		std::unique_ptr<wchar_t[]> _wide_buffer;

		bool _handle_eof, _eof = false;
	};

	void initConsole();

	void getCursorPosition(int& x, int& y);

	void setCursorPosition(const int x, const int y);

	void hideCursor();

	void showCursor();

	void fillCharToEnd(char c);

	std::ostream& lineClear(std::ostream& _Ostr);

	std::ostream& textGreen(std::ostream& _Ostr);

	std::ostream& textRed(std::ostream& _Ostr);

	std::ostream& textYellow(std::ostream& _Ostr);

	std::ostream& textOrange(std::ostream& _Ostr);

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
