#include <format>
#include "ExamTable.hpp"
#include "Utils.hpp"
#include "StudentManager.hpp"
#include "ExamManager.hpp"
#include "SubjectManager.hpp"

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
			std::string line;
			*reader.input >> line;

			int start = 0;
			while (start < line.size())
			{
				float score;
				int end = line.find(',', start);
				if (end == std::string::npos)
					end = line.size();
				score = std::stof(line.substr(start, end - start));
				scores.push_back(score);
				start = end + 1;
			}

			auto pStu = StudentManager::getInstance().getStudentById(id);
			table.push_back(std::make_pair(std::move(pStu), scores));
		}
		return true;
	}

	void ExamTable::setStudentScore(const std::shared_ptr<StudentInfo>& pStudent, int subjectId, float score)
	{
		if (pStudent == nullptr || !SubjectManager::getInstance().isSubjectExist(subjectId) || (score < 0 && score != -1))
			return;

		std::vector<float>* pScores = nullptr;
		for (auto& pair : table)
		{
			if (pair.first == pStudent)
			{
				pScores = &pair.second;
				break;
			}
		}
		if (pScores == nullptr)
		{
			table.push_back(std::make_pair(pStudent, std::vector<float>(SubjectManager::getInstance().getSubjectCount())));
			pScores = &table.rbegin()->second;
			for (auto& score : *pScores)
				score = -1;
		}
		while (pScores->size() <= subjectId)
			pScores->push_back(-1);
		(*pScores)[subjectId] = score;
	}

	float ExamTable::getStudentScore(const std::shared_ptr<StudentInfo>& pStudent, int subjectId) const
	{
		if (pStudent == nullptr || !SubjectManager::getInstance().isSubjectExist(subjectId))
			return -1;

		for (auto& pair : table)
		{
			if (pair.first == pStudent)
			{
				if (pair.second.size() <= subjectId)
					return -1;
				return pair.second[subjectId];
			}
		}
		return -1;
	}

	bool ExamTable::isStudentInTable(const std::shared_ptr<StudentInfo>& student) const
	{
		if (student == nullptr)
			return false;

		for (auto& pair : table)
		{
			if (pair.first == student)
				return true;
		}
		return false;
	}

	void ExamTable::removeSubject(int subjectId)
	{
		for (auto& pair : table)
		{
			if (pair.second.size() > subjectId)
				pair.second[subjectId] = -1;
		}
	}

	std::vector<std::pair<std::shared_ptr<StudentInfo>, std::vector<float>>>& ExamTable::getTable() noexcept
	{
		return table;
	}
}
