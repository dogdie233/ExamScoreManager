#include "ExamTable.hpp"

#include "Utils.hpp"
#include "StudentManager.hpp"
#include "ExamManager.hpp"
#include "SubjectManager.hpp"

#include <format>

namespace esm
{
	ExamTable::ExamTable() : id(-1), PersistentDataSavable("") {}
	ExamTable::ExamTable(const int id) : ExamTable(id, std::format("未命名考试{}", id)) {}
	ExamTable::ExamTable(const int id, const std::string& title)
		: id(id), title(title), PersistentDataSavable(std::format("data/exam_{}.csv", id)) {}
	ExamTable::ExamTable(const int id, std::vector<int>&& subjects, const std::string& title)
		: id(id), title(title), subjects{ std::move(subjects) }, PersistentDataSavable(std::format("data/exam_{}.csv", id)) {}

	bool ExamTable::save()
	{
		auto writer = csv::CsvWriter(nullptr);
		if (!CreateCsvWriterSafe(this->persistentData, writer))
			return false;

		for (auto& pair : table)
		{
			writer << pair.pStudent->id;
			for (auto& score : pair.scores)
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
			std::vector<float> scores(subjects.size());
			for (int i = 0; i < subjects.size(); i++)
				reader >> scores[i];
			auto pStu = StudentManager::getInstance().getStudentById(id);
			table.push_back(ExamScoreRecord(pStu, std::move(scores)));
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
			if (pair.pStudent == pStudent)
			{
				pScores = &pair.scores;
				break;
			}
		}
		if (pScores == nullptr)
		{
			table.push_back(ExamScoreRecord(pStudent, std::vector<float>(subjects.size(), -1)));
			pScores = &table.rbegin()->scores;
		}
		auto subjectIndex = find(subjects.begin(), subjects.end(), subjectId) - subjects.begin();
		(*pScores)[subjectIndex] = score;
	}

	float ExamTable::getStudentScore(const std::shared_ptr<StudentInfo>& pStudent, int subjectId) const
	{
		if (pStudent == nullptr)
			return -1;

		int subjectIndex = find(subjects.begin(), subjects.end(), subjectId) - subjects.begin();
		if (subjectIndex >= subjects.size())
			return -1;
		for (auto& pair : table)
		{
			if (pair.pStudent == pStudent)
				return pair.scores[subjectIndex];
		}
		return -1;
	}

	bool ExamTable::isStudentInTable(const std::shared_ptr<StudentInfo>& student) const
	{
		if (student == nullptr)
			return false;

		for (auto& pair : table)
		{
			if (pair.pStudent == student)
				return true;
		}
		return false;
	}

	void ExamTable::addSubject(int subjectId)
	{
		if (!SubjectManager::getInstance().isSubjectExist(subjectId))
			return;

		subjects.push_back(subjectId);
		for (auto& record : table)
			record.scores.push_back(-1);
	}

	void ExamTable::removeSubject(int subjectId)
	{
		int index = find(subjects.begin(), subjects.end(), subjectId) - subjects.begin();
		if (index >= subjects.size())
			return;

		subjects.erase(subjects.begin() + index);
		for (auto& record : table)
			record.scores.erase(record.scores.begin() + index);
	}

	bool ExamTable::removeStudent(std::shared_ptr<StudentInfo> pStu) noexcept
	{
		if (pStu == nullptr)
			return false;

		for (auto it = table.cbegin(); it < table.cend(); it++)
		{
			if (it->pStudent != pStu)
				continue;

			table.erase(it);
			return true;
		}
		return false;
	}

	std::vector<ExamScoreRecord>& ExamTable::getTable() noexcept
	{
		return table;
	}
}
