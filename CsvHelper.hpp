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
        /// 写csv字符串，为了偷懒直接全部用双引号了
        /// </summary>
        /// <param name="output">输出流</param>
        /// <param name="str">原字符串</param>
        static void writeCsvString(std::ostream& output, const std::string& str);

        /// <summary>
        /// 解析nm的单个csv字符串
        /// </summary>
        /// <param name="input">输入流</param>
        /// <param name="dest">目标字符串</param>
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
