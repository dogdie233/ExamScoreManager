#include <format>
#include "ExamTable.hpp"
#include "Utils.hpp"
#include "StudentManager.hpp"
#include "ExamManager.hpp"

namespace esm
{
	ExamTable::ExamTable() : id(-1), PersistentDataSavable("") {}
	ExamTable::ExamTable(const int id) : ExamTable(id, std::format("未命名考试{}", id)) {}
	ExamTable::ExamTable(const int id, const std::string& title)
		: id(id), title(title), PersistentDataSavable(std::format("data/exam_{}.csv", id)) {}

	bool ExamTable::save()
	{
		auto writer = csv::CsvWriter(nullptr);
		if (!CreateCsvWriterSafe(this->persistentData, writer))
			return false;

		for (auto& pair : table)
		{
			writer << pair.first->id;
			for (auto& score : pair.second)
				writer << score;
			writer << csv::endl;
		}
		return true;
	}

	bool ExamTable::load()
	{
		auto reader = csv::CsvReader(nullptr);
		if (!CreateCsvReaderSafe(this->persistentData, reader))
		{
			// 无法加载考试数据，删除考试信息
			ExamManager::getInstance().removeExam(id);
			return false;
		}

		table.clear();
		while (reader.hasNext())
		{
			std::string id;
			reader >> id;
			std::vector<float> scores;
			while (reader.hasNext())
			{
				float score;
				reader >> score;
				scores.push_back(score);
			}
			auto pStu = StudentManager::getInstance().getStudentById(id);
			table.push_back(std::make_pair(std::move(pStu), scores));
		}
		return true;
	}

	void ExamTable::AddScoreInfo(std::pair<std::shared_ptr<StudentInfo>, std::vector<float>>&& pair)
	{
		table.push_back(std::move(pair));
	}

	std::vector<std::pair<std::shared_ptr<StudentInfo>, std::vector<float>>>& ExamTable::getTable()
	{
		return table;
	}
}
