#include "CsvHelper.hpp"

#include <iostream>

namespace csv
{
    void CsvHelper::writeCsvString(std::ostream& output, const std::string& str)
    {
        output << '"';
        for (char ch : str) {
            output << ch;
            if (ch == '"')  // csv转义规定
                output << '"';
        }
        output << '"';
    }

    void CsvHelper::readCsvString(std::istream& input, std::string& dest)
    {
        char c;
        dest.clear(); // 确保目标字符串是空的

        input >> c;
        bool isInQuote = c == '"';
        if (isInQuote)
            input >> c;

        do
        {
            if ((!isInQuote && (c == ',' || c == '\n')) || (isInQuote && c == '"' && (!input.get(c) || c != '"')))
            {
                if (c == '\n' && input.peek() == '\r')
                    input.get(); // 跳过Windows风格的换行符中的'\r'
                break;
            }
            dest += c;
        } while (input.get(c));

        // 如果在引号内部结束，需要跳过结尾的引号
        if (isInQuote && c == '"') {
            // 可以在这里处理结尾的引号，如果需要的话
        }
    }

    void CsvHelper::solveDeliver(std::istream& input)
    {
        if (input.eof())
            return;

        int c = input.get();
        if (c == ',')
            return;
        if (c == '\n' && input.peek() == '\r')
            input.get();
    }

    CsvReader::CsvReader(std::shared_ptr<std::istream> stream) : input{ stream } {}

    bool CsvReader::hasNext()
    {
        char c;
        while (!input->eof() && (input->peek() == '\n' || input->peek() == '\r'))
            input->get(c);
        return !input->eof();
    }

    CsvReader& CsvReader::operator>>(std::string& str)
    {
        if (input->peek() == ',')
        {
            input->get();
            return *this;
        }

        CsvHelper::readCsvString(*input, str);
        return *this;
    }

    CsvReader& CsvReader::operator>>(int& num)
    {
        if (input->peek() == ',')
        {
            input->get();
            return *this;
        }

        std::string str;
        CsvHelper::readCsvString(*input, str);
        num = std::stoi(str);
        return *this;
    }

    CsvReader& CsvReader::operator>>(float& num)
    {
        if (input->peek() == ',')
        {
            input->get();
            return *this;
        }

        std::string str;
        CsvHelper::readCsvString(*input, str);
        num = std::stof(str);
        return *this;
    }

    CsvWriter::CsvWriter(std::shared_ptr<std::ostream> output) : output{ output }, isNewLine(true) {}

    CsvWriter& CsvWriter::operator<<(const std::string& str)
    {
        if (!isNewLine)
            *output << ',';
        CsvHelper::writeCsvString(*output, str);
        isNewLine = false;
        return *this;
    }

    CsvWriter& CsvWriter::operator<<(const int num)
    {
        if (!isNewLine)
            *output << ',';
        *output << num;
        isNewLine = false;
        return *this;
    }

    CsvWriter& CsvWriter::operator<<(const float num)
    {
        if (!isNewLine)
            *output << ',';
        *output << num;
        isNewLine = false;
        return *this;
    }

    CsvWriter& CsvWriter::operator<<(CsvWriter& (*func)(CsvWriter&))
    {
        return func(*this);
    }

    CsvWriter& flush(CsvWriter& writer)
    {
        *writer.output << std::flush;
        return writer;
    }

    CsvWriter& endl(CsvWriter& writer)
	{
        *writer.output << std::endl;
        writer.isNewLine = true;
		return writer;
	}
}