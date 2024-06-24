#include <format>
#include <limits>
#include <iomanip>

#include "Commands.hpp"
#include "Breadcrumb.hpp"
#include "Command.hpp"
#include "Utils.hpp"
#include "ConsoleUtils.hpp"
#include "SubjectManager.hpp"
#include "StudentManager.hpp"
#include "ClassManager.hpp"
#include "ExamManager.hpp"
#include "Navigator.hpp"

#define ENSURE(expr, output) do { \
	if (!(expr)) \
	{ \
		std::cout << con::textRed << (output) << con::textColorDefault << std::endl; \
		waitAnyKeyPressed(); \
		return; \
	} \
} while(false)

namespace esm
{
	BackCommand::BackCommand(const std::string& title) : Command(title) {}
	void BackCommand::Invoke()
	{
		Navigator::getInstance().pop();
	}

	inline std::unique_ptr<BackCommand> makeBackCommandPtr()
	{
		return std::make_unique<BackCommand>("返回");
	}

	LambdaCommand::LambdaCommand(const std::string& title, std::function<void(void)>)
		: Command(title), callback{ callback } {}
	void LambdaCommand::Invoke()
	{
		callback();
	}

	TestCommand::TestCommand(const std::string& title, int depth)
		: Command(std::format("{}{}", title, depth)), depth(depth) {}
	void TestCommand::Invoke()
	{
		auto breadcrumb = Breadcrumb(std::format("测试{}", depth));
		breadcrumb.addCommand(makeBackCommandPtr());
		for (int i = 1; i <= depth + 1; i++)
			breadcrumb.addCommand(std::make_unique<TestCommand>("进入测试", i));
		Navigator::getInstance().push(std::move(breadcrumb));
	}

	SubjectNewCommand::SubjectNewCommand() : Command("添加课程") {}
	void SubjectNewCommand::Invoke()
	{
		std::cout << "请输入课程名称：";
		std::string name;
		std::cin >> name;
		int id = SubjectManager::getInstance().getSubjectId(name);
		if (id != -1)
		{
			std::cout << con::textYellow << "! 课程已存在，id为" << id << con::textColorDefault << std::endl;
			waitEnterPressed();
			return;
		}
		id = SubjectManager::getInstance().addSubject(std::move(name));
		std::cout << con::textGreen << "√ 课程添加成功，ID为" << id << con::textColorDefault << std::endl;
		waitAnyKeyPressed();
	}

	ListSubjectCommand::ListSubjectCommand() : Command("列出所有课程") {}
	void ListSubjectCommand::Invoke()
	{
		clearConsole();
		auto& subjects = SubjectManager::getInstance().getSubjects();
		std::cout << "当前一共有 " << SubjectManager::getInstance().getSubjectCount() << " 个课程" << std::endl;
		if (subjects.size() == 0)
		{
			waitAnyKeyPressed();
			return;
		}
		std::cout << "ID" << con::cha(4) << "课程名称" << std::endl;
		for (int i = 0; i < subjects.size(); i++)
		{
			if (subjects[i].empty())
				continue;
			std::cout << i << con::cha(4) << subjects[i] << std::endl;
		}
		waitAnyKeyPressed();
	}

	SubjectSelectionCommand::SubjectSelectionCommand(std::function<void(int)> callback, int selecting)
		: Command("选择一个课程"), callback{ callback }, selecting(selecting) {}
	void SubjectSelectionCommand::Invoke()
	{
		ENSURE(SubjectManager::getInstance().getSubjectCount() != 0, "! 请先添加课程");
		auto breadcrumb = Breadcrumb("选择课程");
		auto& subjects = SubjectManager::getInstance().getSubjects();
		for (int i = 0; i < subjects.size(); i++)
		{
			if (subjects[i].empty())
				continue;
			breadcrumb.addCommand(std::make_unique<OptionCommand>(std::format("{}\033[4G{}", i, subjects[i]), *this, i));
		}
		breadcrumb.selecting = selecting;
		Navigator::getInstance().push(std::move(breadcrumb));
	}
	void SubjectSelectionCommand::UpdateSelecting(int selecting)
	{
		this->selecting = selecting;
		if (this->callback != nullptr)
			this->callback(selecting);
		if (selecting == -1)
			this->title = "选择一个课程";
		else
			this->title = std::format("选择一个课程（当前选中：{}）", SubjectManager::getInstance().getSubjectName(selecting));
	}

	SubjectSelectionCommand::OptionCommand::OptionCommand(std::string&& optionTitle, SubjectSelectionCommand& parent, int index)
		: Command(optionTitle), parent{ parent }, index(index) {}
	void SubjectSelectionCommand::OptionCommand::Invoke()
	{
		parent.UpdateSelecting(index);
		Navigator::getInstance().pop();
	}

	SubjectRenameCommand::SubjectRenameCommand(ExistingSubjectManagementCommand& management)
		: Command("重命名课程"), management{ management } {}
	void SubjectRenameCommand::Invoke()
	{
		ENSURE(management.selectedSubjectId != -1, "! 请先选择一个课程");
		std::cout << "请输入新的课程名称：";
		std::string name;
		std::cin >> name;
		bool succeed = SubjectManager::getInstance().renameSubject(management.selectedSubjectId, std::move(name));
		if (succeed)
		{
			std::cout << con::textGreen << "√ 重命名课程成功" << con::textColorDefault << std::endl;
			waitAnyKeyPressed();
			Navigator::getInstance().pop();
		}
		else
			std::cout << con::textRed << "! 重命名课程失败，大概是名字重复了" << con::textColorDefault << std::endl;
		waitAnyKeyPressed();
	}

	SubjectDeleteCommand::SubjectDeleteCommand(ExistingSubjectManagementCommand& management)
		: Command("删除课程"), management{ management } {}
	void SubjectDeleteCommand::Invoke()
	{
		ENSURE(management.selectedSubjectId != -1, "! 请先选择一个课程");
		auto& name = SubjectManager::getInstance().getSubjectName(management.selectedSubjectId);
		if (ConfirmDeleteAction("课程", name))
		{
			ExamManager::getInstance().removeSubject(management.selectedSubjectId);
			SubjectManager::getInstance().removeSubject(management.selectedSubjectId);
			Navigator::getInstance().pop();
		}
		waitAnyKeyPressed();
	}

	ExistingSubjectManagementCommand::ExistingSubjectManagementCommand() : Command("管理已有课程") {}
	void ExistingSubjectManagementCommand::Invoke()
	{
		ENSURE(SubjectManager::getInstance().getSubjectCount() != 0, "! 请先添加课程");
		auto breadcrumb = Breadcrumb("已有课程管理");
		breadcrumb.addCommand(makeBackCommandPtr());
		breadcrumb.addCommand(std::make_unique<SubjectSelectionCommand>([this](int index) { this->selectedSubjectId = index; }));
		breadcrumb.addCommand(std::make_unique<SubjectRenameCommand>(*this));
		breadcrumb.addCommand(std::make_unique<SubjectDeleteCommand>(*this));
		Navigator::getInstance().push(std::move(breadcrumb));
		Navigator::getInstance().getAll().back().getCommands()[1]->Invoke();
	}

	SubjectManagementCommand::SubjectManagementCommand() : Command("管理课程信息") {}
	void SubjectManagementCommand::Invoke()
	{
		auto breadcrumb = Breadcrumb("课程管理");
		breadcrumb.addCommand(makeBackCommandPtr());
		breadcrumb.addCommand(std::make_unique<SubjectNewCommand>());
		breadcrumb.addCommand(std::make_unique<ListSubjectCommand>());
		breadcrumb.addCommand(std::make_unique<ExistingSubjectManagementCommand>());
		Navigator::getInstance().push(std::move(breadcrumb));
	}

	StudentAddCommand::StudentAddCommand() : Command("添加学生") {}
	void StudentAddCommand::Invoke()
	{
		ENSURE(ClassManager::getInstance().getClasses().size() != 0, "! 请先添加班级");
		StudentInfo stu;
		std::cout << "请输入学生姓名：";
		std::cin >> stu.name;
		std::cout << "请输入学生学号：";
		std::cin >> stu.id;
		std::cout << "请选择学生班级：";
		int x, y;
		con::getCursorPosition(x, y);
		std::cout << std::endl;
		stu.classId = selectOption(ClassManager::getInstance().getClasses(), 0);
		con::setCursorPosition(x, y);
		std::cout << ClassManager::getInstance().getClasses()[stu.classId] << '\n';
		if (StudentManager::getInstance().getStudentById(stu.id) != nullptr)
			std::cout << con::textRed << "! 该学号已存在，添加失败" << con::textColorDefault << std::endl;
		else
		{
			StudentManager::getInstance().addStudent(stu);
			std::cout << con::textGreen << "√ 学生添加成功" << con::textColorDefault << std::endl;
		}
		waitAnyKeyPressed();
	}

	StudentListCommand::StudentListCommand() : Command("列出所有学生") {}
	void StudentListCommand::Invoke()
	{
		auto& students = StudentManager::getInstance().getStudents();
		clearConsole();
		std::cout << "当前一共有 " << students.size() << " 个学生" << std::endl;
		if (students.size() == 0)
		{
			waitAnyKeyPressed();
			return;
		}
		auto names = students | std::views::transform([](const auto& p) -> int { return dummyStrLenCalc(p->name); });
		int nameLongest = *std::max_element(names.begin(), names.end());
		auto ids = students | std::views::transform([](const auto& p) -> int { return p->id.size(); });
		int idLongest = *std::max_element(ids.begin(), ids.end());

		nameLongest = std::max(nameLongest, 4);
		idLongest = std::max(idLongest, 4);

		int idPos = 1 + nameLongest + 2;
		int classPos = idPos + idLongest + 2;
		auto& classes = ClassManager::getInstance().getClasses();
		std::cout << "姓名" << con::cha(idPos) << "学号" << con::cha(classPos) << "班级" << std::endl;
		for (auto& stu : students)
			std::cout << stu->name << con::cha(idPos) << stu->id << con::cha(classPos) << classes[stu->classId] << std::endl;
		waitEnterPressed();
	}

	ExistingStudentManagementCommand::ExistingStudentManagementCommand()
		: Command("管理已有学生"), selectedStudentPtr(nullptr) {}
	void ExistingStudentManagementCommand::Invoke()
	{
		ENSURE(StudentManager::getInstance().getStudents().size() != 0, "! 请先添加学生");
		auto breadcrumb = Breadcrumb("已有学生管理");
		breadcrumb.addCommand(makeBackCommandPtr());
		breadcrumb.addCommand(std::make_unique<StudentSelectionCommand>([this](auto pStu) { this->selectedStudentPtr = pStu; }, nullptr));
		breadcrumb.addCommand(std::make_unique<StudentRenameCommand>(*this));
		breadcrumb.addCommand(std::make_unique<StudentDeleteCommand>(*this));
		Navigator::getInstance().push(std::move(breadcrumb));
		Navigator::getInstance().getAll().back().getCommands()[1]->Invoke();
	}

	StudentSelectionCommand::StudentSelectionCommand(std::function<void(std::shared_ptr<StudentInfo>)> callback, std::shared_ptr<StudentInfo> pStu)
		: Command("选择一个学生"), callback{ callback } {}
	void StudentSelectionCommand::Invoke()
	{
		auto breadcrumb = Breadcrumb("选择学生");
		breadcrumb.addCommand(makeBackCommandPtr());
		breadcrumb.addCommand(std::make_unique<StudentSelectionCommand::ByIdCommand>(*this));
		breadcrumb.addCommand(std::make_unique<StudentSelectionCommand::ByNameCommand>(*this));
		Navigator::getInstance().push(std::move(breadcrumb));
	}
	void StudentSelectionCommand::UpdateSelecting(std::shared_ptr<StudentInfo> studentPtr)
	{
		if (studentPtr == nullptr)
			this->title = "选择一个学生";
		else
			this->title = std::format("选择一个学生（当前选中：{}）", studentPtr->name);
		pStu = studentPtr;
		callback(pStu);
	}

	StudentSelectionCommand::ByIdCommand::ByIdCommand(StudentSelectionCommand& parent)
		: Command("按ID查找"), parent{ parent } {}
	void StudentSelectionCommand::ByIdCommand::Invoke()
	{
		std::string id;
		std::cout << "请输入学生学号：";
		std::cin >> id;
		auto pStu = StudentManager::getInstance().getStudentById(id);
		if (pStu == nullptr)
			std::cout << con::textRed << "! 未找到学号为" << id << "的学生" << con::textColorDefault << std::endl;
		else
		{
			std::cout << con::textGreen << "√ 已选中学生 " << pStu->name << "（" << pStu->id << "）" << con::textColorDefault << std::endl;
			parent.UpdateSelecting(pStu);
			Navigator::getInstance().pop();
		}
		waitAnyKeyPressed();
	}

	StudentSelectionCommand::ByNameCommand::ByNameCommand(StudentSelectionCommand& parent)
		: Command("按姓名模糊查找"), parent{ parent } {}
	void StudentSelectionCommand::ByNameCommand::Invoke()
	{
		std::string name;
		std::cout << "请输入学生姓名：";
		std::cin >> name;
		auto students = StudentManager::getInstance().getStudentsByNameRegex(name);
		if (students.size() == 0)
		{
			std::cout << con::textRed << "! 未找到姓名中包含\"" << name << "\"的学生" << con::textColorDefault << std::endl;
			waitAnyKeyPressed();
			students.clear();
			return;
		}

		if (students.size() > 20)
		{
			std::cout << con::textYellow << "! 匹配到超过20个学生，只显示前20个" << con::textColorDefault << std::endl;
			students.erase(students.begin() + 20, students.end());
		}
		std::vector<std::string> names(students.size());
		auto ids = students | std::views::transform([](const auto& p) -> int { return dummyStrLenCalc(p->id); });
		int idLongest = *std::max_element(ids.begin(), ids.end());
		idLongest = std::max(idLongest, 8);
		for (int i = 0; i < students.size(); i++)
			names[i] = students[i]->id + con::chaCmd(1 + idLongest + 2) + students[i]->name;

		std::cout << "请选择一个学生：" << std::flush;
		int x, y;
		con::getCursorPosition(x, y);
		std::cout << '\n';
		int selected = selectOption(names, 0);
		auto& pStu = students[selected];
		con::setCursorPosition(x, y);
		std::cout << pStu->name << "（" << pStu->id << "）" << '\n';
		std::cout << con::textGreen << "√ 选择完成" << con::textColorDefault << std::endl;
		students.clear();
		parent.UpdateSelecting(pStu);
		Navigator::getInstance().pop();
		waitAnyKeyPressed();
	}

	StudentRenameCommand::StudentRenameCommand(ExistingStudentManagementCommand& management)
		: Command("重命名学生"), management{ management } {}
	void StudentRenameCommand::Invoke()
	{
		ENSURE(management.selectedStudentPtr != nullptr, "! 请先选择一个学生");
		std::cout << "请输入新的姓名：";
		std::string name;
		std::cin >> name;
		management.selectedStudentPtr->name = name;
		std::cout << con::textGreen << "√ 重命名成功" << con::textColorDefault << std::endl;
		Navigator::getInstance().pop();
		waitAnyKeyPressed();
	}

	StudentDeleteCommand::StudentDeleteCommand(ExistingStudentManagementCommand& management)
		: Command("删除学生"), management{ management } {}
	void StudentDeleteCommand::Invoke()
	{
		ENSURE(management.selectedStudentPtr != nullptr, "! 请先选择一个学生");
		auto& name = management.selectedStudentPtr->name;
		if (ConfirmDeleteAction("学生", name))
		{
			StudentManager::getInstance().removeStudent(std::move(management.selectedStudentPtr));
			management.selectedStudentPtr = nullptr;
			Navigator::getInstance().pop();
		}
		waitAnyKeyPressed();
	}

	ClassAddCommand::ClassAddCommand() : Command("添加班级") {}
	void ClassAddCommand::Invoke()
	{
		std::cout << "请输入班级名称：";
		std::string name;
		std::cin >> name;
		int id = ClassManager::getInstance().addClass(std::move(name));
		std::cout << con::textGreen << "√ 班级添加成功，ID为" << id << con::textColorDefault << std::endl;
		waitAnyKeyPressed();
	}

	StudentManagementCommand::StudentManagementCommand() : Command("管理学生信息") {}
	void StudentManagementCommand::Invoke()
	{
		auto breadcrumb = Breadcrumb("学生管理");
		breadcrumb.addCommand(makeBackCommandPtr());
		breadcrumb.addCommand(std::make_unique<StudentAddCommand>());
		breadcrumb.addCommand(std::make_unique<StudentListCommand>());
		breadcrumb.addCommand(std::make_unique<ExistingStudentManagementCommand>());
		breadcrumb.addCommand(std::make_unique<ClassAddCommand>());
		Navigator::getInstance().push(std::move(breadcrumb));
	}

	ExamNewCommand::ExamNewCommand() : Command("新增考试") {}
	void ExamNewCommand::Invoke()
	{
		std::cout << "请输入考试名称：";
		std::string name;
		std::cin >> name;
		auto pExam = ExamManager::getInstance().newExam(ExamManager::getInstance().nextAvailableId(), name);
		std::cout << con::textGreen << "√ 考试记录创建成功，ID为" << pExam->id << con::textColorDefault << std::endl;
		waitAnyKeyPressed();
	}

	ExamListCommand::ExamListCommand() : Command("列出所有考试") {}
	void ExamListCommand::Invoke()
	{
		auto exams = ExamManager::getInstance().getExams();
		clearConsole();
		std::cout << "当前一共有 " << exams.size() << " 个考试记录" << std::endl;
		if (exams.size() == 0)
		{
			waitAnyKeyPressed();
			return;
		}
		std::cout << "ID" << con::cha(6) << "考试名称" << std::endl;
		for (auto& exam : exams)
			std::cout << exam.second->id << con::cha(6) << exam.second->title << std::endl;
		waitAnyKeyPressed();
	}

	ExamSelectionCommand::ExamSelectionCommand(std::function<void(std::shared_ptr<ExamTable>)> callback, std::shared_ptr<ExamTable> pExam)
		: Command("选择一场考试"), callback{ callback }, pExam{ pExam } {}
	void ExamSelectionCommand::Invoke()
	{
		auto breadcrumb = Breadcrumb("选择考试");
		auto exams = ExamManager::getInstance().getExams();
		breadcrumb.addCommand(makeBackCommandPtr());
		for (auto& kvp : exams)
			breadcrumb.addCommand(std::make_unique<OptionCommand>(std::format("{}\033[4G{}", kvp.second->id, kvp.second->title), *this, kvp.second));
		Navigator::getInstance().push(std::move(breadcrumb));
	}
	void ExamSelectionCommand::UpdateSelecting(std::shared_ptr<ExamTable> pExam)
	{
		this->pExam = pExam;
		if (pExam == nullptr)
			this->title = "选择一场考试";
		else
			this->title = std::format("选择一场考试（当前选中：{}）", pExam->title);
		callback(pExam);
	}

	ExamSelectionCommand::OptionCommand::OptionCommand(std::string&& optionTitle, ExamSelectionCommand& parent, std::shared_ptr<ExamTable> pExam)
		: Command(optionTitle), parent{ parent }, pExam{ pExam } {}
	void ExamSelectionCommand::OptionCommand::Invoke()
	{
		parent.UpdateSelecting(pExam);
		Navigator::getInstance().pop();
	}

	ExistingExamManagementCommand::ExistingExamManagementCommand() : Command("管理单次考试成绩与信息"), pExam(nullptr) {}
	void ExistingExamManagementCommand::Invoke()
	{
		ENSURE(ExamManager::getInstance().getExams().size() != 0, "! 请先添加考试");
		auto breadcrumb = Breadcrumb("已有考试管理");
		breadcrumb.addCommand(makeBackCommandPtr());
		breadcrumb.addCommand(std::make_unique<ExamSelectionCommand>([this](auto pExam) { this->pExam = pExam; }, nullptr));
		breadcrumb.addCommand(std::make_unique<ExamDeleteCommand>(*this));
		breadcrumb.addCommand(std::make_unique<ExamRenameCommand>(*this));
		breadcrumb.addCommand(std::make_unique<ExamScorePrintCommand>(*this));
		breadcrumb.addCommand(std::make_unique<ExamScoreAddCommand>(*this));
		breadcrumb.addCommand(std::make_unique<ExamScoreUpdateCommand>(*this));
		Navigator::getInstance().push(std::move(breadcrumb));
		Navigator::getInstance().getAll().back().getCommands()[1]->Invoke();
	}

	ExamRenameCommand::ExamRenameCommand(ExistingExamManagementCommand& management)
		: Command("重命名考试"), management{ management } {}
	void ExamRenameCommand::Invoke()
	{
		ENSURE(management.pExam != nullptr, "! 请先选择一个考试");
		std::cout << "请输入新的考试名称：";
		std::string name;
		std::cin >> name;
		management.pExam->title = name;
		ExamManager::getInstance().save();
		std::cout << con::textGreen << "√ 重命名成功" << con::textColorDefault << std::endl;
		Navigator::getInstance().pop();
		waitAnyKeyPressed();
	}

	ExamDeleteCommand::ExamDeleteCommand(ExistingExamManagementCommand& management)
		: Command("删除考试"), management{ management } {}
	void ExamDeleteCommand::Invoke()
	{
		ENSURE(management.pExam != nullptr, "! 请先选择一个考试");
		auto& title = management.pExam->title;
		if (ConfirmDeleteAction("考试", title))
		{
			ExamManager::getInstance().removeExam(management.pExam->id);
			management.pExam = nullptr;
			Navigator::getInstance().pop();
		}
		waitAnyKeyPressed();
	}

	ExamScorePrintCommand::ExamScorePrintCommand(ExistingExamManagementCommand& management)
		: Command("打印本次考试所有成绩"), management{ management } {}
	void ExamScorePrintCommand::Invoke()
	{
		const int subjectWidth = 16;

		ENSURE(management.pExam != nullptr, "! 请先选择一次考试");
		clearConsole();
		std::cout << "考试名称：" << management.pExam->title << "    ID：" << management.pExam->id << std::endl;
		std::cout << "----------------------------------------" << std::endl;

		auto& table = management.pExam->getTable();
		auto names = table | std::views::transform([](const auto& p) -> int { return dummyStrLenCalc(p.first->name); });
		int nameLongest = *std::max_element(names.begin(), names.end());
		auto ids = table | std::views::transform([](const auto& p) -> int { return p.first->id.size(); });
		int idLongest = *std::max_element(ids.begin(), ids.end());

		nameLongest = std::max(nameLongest, 4);
		idLongest = std::max(idLongest, 4);

		int idPos = 1 + nameLongest + 2;
		int subjectPos = idPos + idLongest + 2;
		auto& classes = ClassManager::getInstance().getClasses();
		std::cout << "姓名" << con::cha(idPos) << "学号";

		auto subjects = SubjectManager::getInstance().getSubjects();
		for (auto& subject : subjects)
		{
			if (subject.empty())
				continue;
			std::cout << con::cha(subjectPos) << subject << ' ';
			subjectPos += subjectWidth;
		}
		std::cout << std::endl;
		
		for (auto& pair : table)
		{
			std::cout << pair.first->name << con::cha(idPos) << pair.first->id;
			subjectPos = idPos + idLongest + 2;
			for (auto& score : pair.second)
			{
				if (subjects[&score - pair.second.data()].empty())
					continue;

				std::cout << con::cha(subjectPos);
				if (score == -1.0f)
					std::cout << "未参加";
				else
					std::cout << std::fixed << std::setprecision(2) << score;
				subjectPos += subjectWidth;
			}
			std::cout << std::endl;
		}

		int subjectCnt = SubjectManager::getInstance().getSubjectCount();
		std::vector<float> maxx(subjectCnt, std::numeric_limits<float>::min());
		std::vector<float> minn(subjectCnt, std::numeric_limits<float>::max());
		std::vector<float> avg(subjectCnt);
		std::vector<int> validCount(subjectCnt);
		for (auto& pair : table)
		{
			auto& scores = pair.second;
			int idx = 0;
			for (int j = 0; j < scores.size(); j++)
			{
				if (subjects[j].empty())
					continue;
				auto score = scores[j];
				if (score == -1.0f)
				{
					idx++;
					continue;
				}
				maxx[idx] = std::max(maxx[idx], score);
				minn[idx] = std::min(minn[idx], score);
				avg[idx] += score;
				validCount[idx]++;
				idx++;
			}
		}

		auto printLine = [&](const std::string& name, const std::vector<int>& counts, const std::vector<float>& scores) -> void
			{
				int subjectPos = idPos + idLongest + 2;
				std::cout << name;
				for (int i = 0; i < subjectCnt; i++)
				{
					std::cout << con::cha(subjectPos);
					if (counts[i] == 0)
						std::cout << "无效";
					else
						std::cout << std::fixed << std::setprecision(2) << scores[i];
					subjectPos += subjectWidth;
				}
				std::cout << std::endl;
			};

		std::cout << "----------------------------------------" << std::endl;
		for (int i = 0; i < subjectCnt; i++)
			avg[i] /= validCount[i];
		printLine("平均分", validCount, avg);
		printLine("最高分", validCount, maxx);
		printLine("最低分", validCount, minn);

		waitEnterPressed();
	}

	ExamScoreAddCommand::ExamScoreAddCommand(ExistingExamManagementCommand& management)
		: Command("添加学生成绩"), management{ management }, studentSelectionCmd([this](auto pStu) { OnStudentSelected(pStu); }, nullptr) {}
	void ExamScoreAddCommand::Invoke()
	{
		ENSURE(SubjectManager::getInstance().getSubjectCount() != 0, "! 请先添加课程");
		ENSURE(management.pExam != nullptr, "! 请先选择一次考试");
		studentSelectionCmd.Invoke();
	}
	void ExamScoreAddCommand::OnStudentSelected(std::shared_ptr<StudentInfo> pStu)
	{
		if (pStu == nullptr)
			return;
		ENSURE(!management.pExam->isStudentInTable(pStu), "! 该学生已存在考试记录，无法添加，请使用修改功能");

		clearConsole();
		std::cout << "当前选中学生：" << pStu->name << "（" << pStu->id << "）" << std::endl;
		std::cout << "请输入学生该科目的成绩，输入-1表示未参加考试" << std::endl;
		auto& subjects = SubjectManager::getInstance().getSubjects();
		for (int i = 0; i < subjects.size(); i++)
		{
			if (subjects[i].empty())
				continue;
			std::string input;
			float score = -1.0f;
			while (true)
			{
				std::cout << con::textYellow << subjects[i] << con::textColorDefault << "成绩：";
				std::cin >> input;
				try
				{
					score = std::stof(input);
				}
				catch (std::exception&)
				{
					std::cout << con::textRed << "! 请输入一个合法的数字" << con::textColorDefault << std::endl;
					continue;
				}
				break;
			}
			management.pExam->setStudentScore(pStu, i, score);
		}
		std::cout << con::textGreen << "√ 成绩添加成功" << con::textColorDefault << std::endl;
		management.pExam->save();
	}

	ExamScoreUpdateCommand::ExamScoreUpdateCommand(ExistingExamManagementCommand& management)
		: Command("更新学生成绩"), management{ management }, studentSelectionCmd([this](auto pStu) { OnStudentSelected(pStu); }, nullptr) {}
	void ExamScoreUpdateCommand::Invoke()
	{
		ENSURE(SubjectManager::getInstance().getSubjectCount() != 0, "! 请先添加课程");
		ENSURE(management.pExam != nullptr, "! 请先选择一次考试");
		studentSelectionCmd.Invoke();
	}
	void ExamScoreUpdateCommand::OnStudentSelected(std::shared_ptr<StudentInfo> pStu)
	{
		auto buildOption = [](const std::string& subject, const float score) -> std::string
		{
			if (score == -1.0f)
				return std::format("{}：未参加", subject);
			else
				return std::format("{}：{:.2f}", subject, score);
		};

		if (pStu == nullptr)
			return;
		ENSURE(management.pExam->isStudentInTable(pStu), "! 该学生不存在考试记录，无法更新，请使用添加功能");

		clearConsole();
		auto& subjects = SubjectManager::getInstance().getSubjects();
		std::vector<std::string> options;
		std::vector<int> idMapping;
		options.push_back("返回");
		idMapping.push_back(-1);
		for (int i = 0; i < subjects.size(); i++)
		{
			if (subjects[i].empty())
				continue;
			options.push_back(buildOption(subjects[i], management.pExam->getStudentScore(pStu, i)));
			idMapping.push_back(i);
		}
		std::cout << "当前选中学生：" << pStu->name << "（" << pStu->id << "）" << std::endl;
		std::cout << "请选中要修改的科目，按下回车后输入分数再按回车（-1也可以用来表示未参加考试）" << std::endl;
		std::cout << "========================================" << std::endl;
		int selecting = 0;
		while (true)
		{
			int x, y;
			con::getCursorPosition(x, y);
			selecting = selectOption(options, selecting);
			if (selecting == 0)
				break;
			con::setCursorPosition(x, y);

			int x1 = 1, y1 = 1;
			for (int i = 0; i < options.size(); i++)
			{
				if (i == selecting)
				{
					std::cout << con::textYellow << subjects[idMapping[i]] << "：" << con::textColorDefault;
					con::getCursorPosition(x1, y1);
					std::cout << std::endl;
				}
				else
					std::cout << options[i] << std::endl;
			}
			con::setCursorPosition(x1, y1);

			std::string input;
			std::cin >> input;
			con::setCursorPosition(x, y);

			if (input == "-1" || input == "未参加")
			{
				management.pExam->setStudentScore(pStu, idMapping[selecting], -1.0f);
				options[selecting] = buildOption(subjects[idMapping[selecting]], -1.0f);
			}
			else
			{
				float score = -1.0f;
				try
				{
					score = std::stof(input);
					management.pExam->setStudentScore(pStu, idMapping[selecting], score);
					options[selecting] = buildOption(subjects[idMapping[selecting]], score);
				}
				catch (std::exception&)
				{
					continue;
				}
			}
		}

		management.pExam->save();
		std::cout << con::textGreen << "√ 成绩更新成功" << con::textColorDefault << std::endl;
	}

	ExamManagementCommand::ExamManagementCommand() : Command("管理考试信息") {}
	void ExamManagementCommand::Invoke()
	{
		auto breadcrumb = Breadcrumb("考试管理");
		breadcrumb.addCommand(makeBackCommandPtr());
		breadcrumb.addCommand(std::make_unique<ExamNewCommand>());
		breadcrumb.addCommand(std::make_unique<ExamListCommand>());
		breadcrumb.addCommand(std::make_unique<ExistingExamManagementCommand>());
		Navigator::getInstance().push(std::move(breadcrumb));
	}
}