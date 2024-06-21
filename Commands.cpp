#include "Commands.hpp"

#include <format>
#include "Breadcrumb.hpp"
#include "Command.hpp"
#include "Utils.hpp"
#include "ConsoleUtils.hpp"
#include "SubjectManager.hpp"
#include "StudentManager.hpp"
#include "Navigator.hpp"

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
		std::cout << "当前一共有 " << subjects.size() << " 个课程" << std::endl;
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
		if (SubjectManager::getInstance().getSubjects().size() == 0)
		{
			std::cout << con::textRed << "! 请先添加课程" << con::textColorDefault << std::endl;
			callback(-1);
			waitAnyKeyPressed();
			return;
		}
		auto breadcrumb = Breadcrumb("选择课程");
		auto& subjects = SubjectManager::getInstance().getSubjects();
		for (int i = 0; i < subjects.size(); i++)
			breadcrumb.addCommand(std::make_unique<OptionCommand>(std::format("{}\033[4G{}", i, subjects[i]), *this, i));
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
		if (management.selectedSubjectId == -1)
		{
			std::cout << con::textRed << "! 请先选择一个课程" << con::textColorDefault << std::endl;
			waitAnyKeyPressed();
			return;
		}
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
		if (management.selectedSubjectId == -1)
		{
			std::cout << con::textRed << "! 请先选择一个课程" << con::textColorDefault << std::endl;
			waitAnyKeyPressed();
			return;
		}
		auto& name = SubjectManager::getInstance().getSubjectName(management.selectedSubjectId);
		std::cout << con::textYellow << "? 你确定要删除课程\"" << name << "\"吗" << con::textColorDefault << std::endl;
		waitEnterPressed();
		std::cout << con::textYellow << "? 你真的要删除课程\"" << name << "\"吗" << con::textColorDefault << std::endl;
		waitEnterPressed();
		std::cout << "如果确认删除请输入\"" << name << "\"：";
		std::string input;
		std::cin >> input;
		if (input != name)
		{
			std::cout << con::textRed << "! 删除操作取消，输入不匹配" << con::textColorDefault << std::endl;
		}
		else
		{
			std::cout << con::textGreen << "√ 删除成功" << con::textColorDefault << std::endl;
			SubjectManager::getInstance().removeSubject(management.selectedSubjectId);
			Navigator::getInstance().pop();
		}
		waitAnyKeyPressed();
	}

	ExistingSubjectManagementCommand::ExistingSubjectManagementCommand() : Command("管理已有课程") {}
	void ExistingSubjectManagementCommand::Invoke()
	{
		if (SubjectManager::getInstance().getSubjects().size() == 0)
		{
			std::cout << con::textRed << "! 请先添加课程" << con::textColorDefault << std::endl;
			waitAnyKeyPressed();
			return;
		}
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
		if (StudentManager::getInstance().getClasses().size() == 0)
		{
			std::cout << con::textRed << "! 请先添加班级" << con::textColorDefault << std::endl;
			waitAnyKeyPressed();
			return;
		}
		StudentInfo stu;
		std::cout << "请输入学生姓名：";
		std::cin >> stu.name;
		std::cout << "请输入学生学号：";
		std::cin >> stu.id;
		std::cout << "请选择学生班级：";
		int x, y;
		con::getCursorPosition(x, y);
		std::cout << std::endl;
		stu.classId = selectOption(StudentManager::getInstance().getClasses(), 0);
		con::setCursorPosition(x, y);
		std::cout << StudentManager::getInstance().getClasses()[stu.classId] << '\n';
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

		nameLongest = max(nameLongest, 4);
		idLongest = max(idLongest, 4);

		int idPos = 1 + nameLongest + 2;
		int classPos = idPos + idLongest + 2;
		auto& classes = StudentManager::getInstance().getClasses();
		std::cout << "姓名" << con::cha(idPos) << "学号" << con::cha(classPos) << "班级" << std::endl;
		for (auto& stu : students)
			std::cout << stu->name << con::cha(idPos) << stu->id << con::cha(classPos) << classes[stu->classId] << std::endl;
		waitEnterPressed();
	}

	ExistingStudentManagementCommand::ExistingStudentManagementCommand()
		: Command("管理已有学生"), selectedStudentPtr(nullptr) {}
	void ExistingStudentManagementCommand::Invoke()
	{
		if (StudentManager::getInstance().getStudents().size() == 0)
		{
			std::cout << con::textRed << "! 请先添加学生" << con::textColorDefault << std::endl;
			waitAnyKeyPressed();
			return;
		}

		auto breadcrumb = Breadcrumb("已有学生管理");
		breadcrumb.addCommand(makeBackCommandPtr());
		breadcrumb.addCommand(std::make_unique<StudentSelectionCommand>(*this));
		breadcrumb.addCommand(std::make_unique<StudentRenameCommand>(*this));
		Navigator::getInstance().push(std::move(breadcrumb));
		Navigator::getInstance().getAll().back().getCommands()[1]->Invoke();
	}

	StudentSelectionCommand::StudentSelectionCommand(ExistingStudentManagementCommand& management)
		: Command("选择一个学生"), management{ management } {}
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
		management.selectedStudentPtr = studentPtr;
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
			parent.UpdateSelecting(pStu);
			std::cout << con::textGreen << "√ 已选中学生 " << pStu->name << "（" << pStu->id << "）" << con::textColorDefault << std::endl;
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
		idLongest = max(idLongest, 8);
		for (int i = 0; i < students.size(); i++)
			names[i] = students[i]->id + con::chaCmd(1 + idLongest + 2) + students[i]->name;

		std::cout << "请选择一个学生：" << std::flush;
		int x, y;
		con::getCursorPosition(x, y);
		std::cout << '\n';
		int selected = selectOption(names, 0);
		auto& pStu = students[selected];
		parent.management.selectedStudentPtr = pStu;
		con::setCursorPosition(x, y);
		std::cout << pStu->name << "（" << pStu->id << "）" << '\n';
		std::cout << con::textGreen << "√ 选择完成" << con::textColorDefault << std::endl;
		Navigator::getInstance().pop();
		students.clear();
		waitAnyKeyPressed();
	}

	StudentRenameCommand::StudentRenameCommand(ExistingStudentManagementCommand& management)
		: Command("重命名学生"), management{ management } {}
	void StudentRenameCommand::Invoke()
	{
		if (management.selectedStudentPtr == nullptr)
		{
			std::cout << con::textRed << "! 请先选择一个学生" << con::textColorDefault << std::endl;
			waitAnyKeyPressed();
			return;
		}
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
		if (management.selectedStudentPtr == nullptr)
		{
			std::cout << con::textRed << "! 请先选择一个学生" << con::textColorDefault << std::endl;
			waitAnyKeyPressed();
			return;
		}
		auto& name = management.selectedStudentPtr->name;
		std::cout << con::textYellow << "? 你确定要删除学生\"" << name << "\"吗" << con::textColorDefault << std::endl;
		waitEnterPressed();
		std::cout << con::textYellow << "? 你真的要删除学生\"" << name << "\"吗" << con::textColorDefault << std::endl;
		waitEnterPressed();
		std::cout << "如果确认删除请输入\"" << name << "\"：";
		std::string input;
		std::cin >> input;
		if (input != name)
			std::cout << con::textRed << "! 删除操作取消，输入不匹配" << con::textColorDefault << std::endl;
		else
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
		int id = StudentManager::getInstance().addClass(std::move(name));
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

	ExamManagerCommand::ExamManagerCommand() : Command("管理考试信息") {}
	void ExamManagerCommand::Invoke()
	{
		auto breadcrumb = Breadcrumb("考试管理");
		breadcrumb.addCommand(makeBackCommandPtr());
		Navigator::getInstance().push(std::move(breadcrumb));
	}
}