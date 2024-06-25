#include "ConsoleUtils.hpp"
#include <locale>
#include <Windows.h>
#include <fcntl.h>
#include <iostream>
#include <streambuf>
#include <iomanip>
#include <string>
#include <corecrt_io.h>
#include <filesystem>

namespace con
{
	static HANDLE stdinHandle = GetStdHandle(STD_INPUT_HANDLE);
	std::unique_ptr<WinUtf8StreamBuf> globalWinUtf8Streambuf;

	WinUtf8StreamBuf::WinUtf8StreamBuf(unsigned read_buf_size, bool handle_console_eof)
		: _read_buf_size(read_buf_size), _u8_buf_size(read_buf_size * 6), _wide_buf_size(read_buf_size * 2),
		_buffer(std::unique_ptr<char[]>(new char[_u8_buf_size])),
		_wide_buffer(std::unique_ptr<wchar_t[]>(new wchar_t[read_buf_size * 2])),
		_handle_eof(handle_console_eof)
	{
	}

	std::streambuf::int_type WinUtf8StreamBuf::underflow()
	{
		if (_eof)
		{
			setg(nullptr, nullptr, nullptr);
			return traits_type::eof();
		}

		if (gptr() < egptr())
			return traits_type::to_int_type(*this->gptr());

		unsigned long read_len;
		auto ret = ReadConsoleW(
			GetStdHandle(STD_INPUT_HANDLE),
			_wide_buffer.get(),
			_wide_buf_size,
			&read_len, NULL);

		if (!ret || read_len == 0)
		{
			_eof = true;
			setg(nullptr, nullptr, nullptr);
			return traits_type::eof();
		}

		if (_handle_eof)
		{
			auto buf = _wide_buffer.get();
			for (unsigned long i = 0; i < read_len; i++)
			{
				// ReadConsole will read Ctrl+Z as-is, so special handling is needed here
				if (buf[i] == 0x001A)
				{
					read_len = i;
					_eof = true;
				}
			}
		}

		if (read_len == 0)
		{
			_eof = true;
			setg(nullptr, nullptr, nullptr);
			return traits_type::eof();
		}

		int size = WideCharToMultiByte(
			CP_UTF8,
			0,
			_wide_buffer.get(),
			read_len,
			_buffer.get(),
			_u8_buf_size,
			NULL, NULL);
		setg(_buffer.get(), _buffer.get(), _buffer.get() + size);
		if (size == 0)
			return traits_type::eof();
		return traits_type::to_int_type(*this->gptr());
	}

	void initConsole()
	{
		DWORD mode;
		SetConsoleOutputCP(CP_UTF8);
		SetConsoleCP(CP_UTF8);
		GetConsoleMode(stdinHandle, &mode);
		SetConsoleMode(stdinHandle, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);

#ifdef WIN32
		if (!globalWinUtf8Streambuf)
			globalWinUtf8Streambuf = std::make_unique<WinUtf8StreamBuf>(2048, true);
		std::cin.rdbuf(globalWinUtf8Streambuf.get());
#endif

		TCHAR exePath[1024] = { 0 };
		GetModuleFileName(NULL, exePath, sizeof(exePath) / sizeof(TCHAR));

		std::filesystem::path exeFilePath(exePath);
		std::filesystem::path exeDirPath = exeFilePath.parent_path();

		SetCurrentDirectory(exeDirPath.c_str());
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