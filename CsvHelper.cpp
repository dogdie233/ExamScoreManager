#include "CsvHelper.hpp"

#include <iostream>

namespace csv
{
    void CsvHelper::writeCsvString(std::ostream& output, const std::string& str)
    {
        output << '"';
        for (char ch : str) {
            output << ch;
            if (ch == '"')  // csvת��涨
                output << '"';
        }
        output << '"';
    }

    void CsvHelper::readCsvString(std::istream& input, std::string& dest)
    {
        char c;
        dest.clear(); // ȷ��Ŀ���ַ����ǿյ�

        input >> c;
        bool isInQuote = c == '"';
        if (isInQuote)
            input >> c;

        do
        {
            if ((!isInQuote && (c == ',' || c == '\n')) || (isInQuote && c == '"' && (!input.get(c) || c != '"')))
            {
                if (c == '\n' && input.peek() == '\r')
                    input.get(); // ����Windows���Ļ��з��е�'\r'
                break;
            }
            dest += c;
        } while (input.get(c));

        // ����������ڲ���������Ҫ������β������
        if (isInQuote && c == '"') {
            // ���������ﴦ���β�����ţ������Ҫ�Ļ�
        }
    }

    void CsvHelper::solveDeliver(std::istream& input)
    {
        if (input.eof())
            return;

        char c;
        input.get(c);
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

        *input >> num;
        CsvHelper::solveDeliver(*input);
        return *this;
    }

    CsvReader& CsvReader::operator>>(float& num)
    {
        if (input->peek() == ',')
        {
            input->get();
            return *this;
        }

        *input >> num;
        CsvHelper::solveDeliver(*input);
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