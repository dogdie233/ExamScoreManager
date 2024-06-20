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

	std::unique_ptr<BackCommand> makeBackCommandPtr()
	{
		return std::make_unique<BackCommand>("返回");
	}

	LambdaCommand::LambdaCommand(const std::string& title, std::function<void(void)>) : Command(title), callback{ callback } {}
	void LambdaCommand::Invoke()
	{
		callback();
	}

	TestCommand::TestCommand(const std::string& title, int depth) : Command(std::format("{}{}", title, depth)), depth(depth) {}
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
		: callback{ callback }, selecting(selecting), Command("选择一个课程") {}
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

	SubjectRenameCommand::SubjectRenameCommand(ExistingSubjectManagementCommand& management) : management{ management }, Command("重命名课程") {}
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

	SubjectDeleteCommand::SubjectDeleteCommand(ExistingSubjectManagementCommand& management) : management{ management }, Command("删除课程") {}
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
			std::cout << con::textRed << "! 删除失败，输入不匹配" << con::textColorDefault << std::endl;
			waitAnyKeyPressed();
			return;
		}

		std::cout << con::textGreen << "√ 删除成功" << con::textColorDefault << std::endl;
		SubjectManager::getInstance().removeSubject(management.selectedSubjectId);
		waitAnyKeyPressed();
		Navigator::getInstance().pop();
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
		std::cout << con::textGreen << "√ 学生添加成功" << con::textColorDefault << std::endl;
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