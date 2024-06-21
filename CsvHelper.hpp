#pragma once
#ifndef ESM_CSV_HELPER_HPP
#define ESM_CSV_HELPER_HPP

#include <iostream>
#include <string>
#include <fstream>

namespace csv
{
    class CsvHelper
    {
    public:
        /// <summary>
        /// дcsv�ַ�����Ϊ��͵��ֱ��ȫ����˫������
        /// </summary>
        /// <param name="output">�����</param>
        /// <param name="str">ԭ�ַ���</param>
        static void writeCsvString(std::ostream& output, const std::string& str);

        /// <summary>
        /// ����nm�ĵ���csv�ַ���
        /// </summary>
        /// <param name="input">������</param>
        /// <param name="dest">Ŀ���ַ���</param>
        static void readCsvString(std::istream& input, std::string& dest);

        static void solveDeliver(std::istream& input);
    private:
    };

    class CsvReader
    {
    public:
        std::shared_ptr<std::istream> input;

        CsvReader(std::shared_ptr<std::istream> pStream);

        bool hasNext();

        CsvReader& operator>>(std::string& str);
        CsvReader& operator>>(int& num);
        CsvReader& operator>>(float& num);
    };

    class CsvWriter
    {
    public:
        std::shared_ptr<std::ostream> output;
        bool isNewLine = true;

        CsvWriter(std::shared_ptr<std::ostream> pStream);

        CsvWriter& operator<<(const std::string& str);
        CsvWriter& operator<<(int num);
        CsvWriter& operator<<(float num);
        CsvWriter& operator<<(CsvWriter& (*func)(CsvWriter&));
    };

    CsvWriter& flush(CsvWriter& writer);
    CsvWriter& endl(CsvWriter& writer);
}

#endif
