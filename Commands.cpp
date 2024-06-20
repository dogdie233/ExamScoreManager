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
		return std::make_unique<BackCommand>("����");
	}

	LambdaCommand::LambdaCommand(const std::string& title, std::function<void(void)>) : Command(title), callback{ callback } {}
	void LambdaCommand::Invoke()
	{
		callback();
	}

	TestCommand::TestCommand(const std::string& title, int depth) : Command(std::format("{}{}", title, depth)), depth(depth) {}
	void TestCommand::Invoke()
	{
		auto breadcrumb = Breadcrumb(std::format("����{}", depth));
		breadcrumb.addCommand(makeBackCommandPtr());
		for (int i = 1; i <= depth + 1; i++)
			breadcrumb.addCommand(std::make_unique<TestCommand>("�������", i));
		Navigator::getInstance().push(std::move(breadcrumb));
	}

	SubjectNewCommand::SubjectNewCommand() : Command("��ӿγ�") {}
	void SubjectNewCommand::Invoke()
	{
		std::cout << "������γ����ƣ�";
		std::string name;
		std::cin >> name;
		int id = SubjectManager::getInstance().getSubjectId(name);
		if (id != -1)
		{
			std::cout << con::textYellow << "! �γ��Ѵ��ڣ�idΪ" << id << con::textColorDefault << std::endl;
			waitEnterPressed();
			return;
		}
		id = SubjectManager::getInstance().addSubject(std::move(name));
		std::cout << con::textGreen << "�� �γ���ӳɹ���IDΪ" << id << con::textColorDefault << std::endl;
		waitAnyKeyPressed();
	}

	ListSubjectCommand::ListSubjectCommand() : Command("�г����пγ�") {}
	void ListSubjectCommand::Invoke()
	{
		clearConsole();
		auto& subjects = SubjectManager::getInstance().getSubjects();
		std::cout << "��ǰһ���� " << subjects.size() << " ���γ�" << std::endl;
		if (subjects.size() == 0)
		{
			waitAnyKeyPressed();
			return;
		}
		std::cout << "ID" << con::cha(4) << "�γ�����" << std::endl;
		for (int i = 0; i < subjects.size(); i++)
		{
			if (subjects[i].empty())
				continue;
			std::cout << i << con::cha(4) << subjects[i] << std::endl;
		}
		waitAnyKeyPressed();
	}

	SubjectSelectionCommand::SubjectSelectionCommand(std::function<void(int)> callback, int selecting)
		: callback{ callback }, selecting(selecting), Command("ѡ��һ���γ�") {}
	void SubjectSelectionCommand::Invoke()
	{
		if (SubjectManager::getInstance().getSubjects().size() == 0)
		{
			std::cout << con::textRed << "! ������ӿγ�" << con::textColorDefault << std::endl;
			callback(-1);
			waitAnyKeyPressed();
			return;
		}
		auto breadcrumb = Breadcrumb("ѡ��γ�");
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
			this->title = "ѡ��һ���γ�";
		else
			this->title = std::format("ѡ��һ���γ̣���ǰѡ�У�{}��", SubjectManager::getInstance().getSubjectName(selecting));
	}

	SubjectSelectionCommand::OptionCommand::OptionCommand(std::string&& optionTitle, SubjectSelectionCommand& parent, int index)
		: Command(optionTitle), parent{ parent }, index(index) {}
	void SubjectSelectionCommand::OptionCommand::Invoke()
	{
		parent.UpdateSelecting(index);
		Navigator::getInstance().pop();
	}

	SubjectRenameCommand::SubjectRenameCommand(ExistingSubjectManagementCommand& management) : management{ management }, Command("�������γ�") {}
	void SubjectRenameCommand::Invoke()
	{
		if (management.selectedSubjectId == -1)
		{
			std::cout << con::textRed << "! ����ѡ��һ���γ�" << con::textColorDefault << std::endl;
			waitAnyKeyPressed();
			return;
		}
		std::cout << "�������µĿγ����ƣ�";
		std::string name;
		std::cin >> name;
		bool succeed = SubjectManager::getInstance().renameSubject(management.selectedSubjectId, std::move(name));
		if (succeed)
		{
			std::cout << con::textGreen << "�� �������γ̳ɹ�" << con::textColorDefault << std::endl;
			waitAnyKeyPressed();
			Navigator::getInstance().pop();
		}
		else
			std::cout << con::textRed << "! �������γ�ʧ�ܣ�����������ظ���" << con::textColorDefault << std::endl;
		waitAnyKeyPressed();
	}

	SubjectDeleteCommand::SubjectDeleteCommand(ExistingSubjectManagementCommand& management) : management{ management }, Command("ɾ���γ�") {}
	void SubjectDeleteCommand::Invoke()
	{
		if (management.selectedSubjectId == -1)
		{
			std::cout << con::textRed << "! ����ѡ��һ���γ�" << con::textColorDefault << std::endl;
			waitAnyKeyPressed();
			return;
		}
		auto& name = SubjectManager::getInstance().getSubjectName(management.selectedSubjectId);
		std::cout << con::textYellow << "? ��ȷ��Ҫɾ���γ�\"" << name << "\"��" << con::textColorDefault << std::endl;
		waitEnterPressed();
		std::cout << con::textYellow << "? �����Ҫɾ���γ�\"" << name << "\"��" << con::textColorDefault << std::endl;
		waitEnterPressed();
		std::cout << "���ȷ��ɾ��������\"" << name << "\"��";
		std::string input;
		std::cin >> input;
		if (input != name)
		{
			std::cout << con::textRed << "! ɾ��ʧ�ܣ����벻ƥ��" << con::textColorDefault << std::endl;
			waitAnyKeyPressed();
			return;
		}

		std::cout << con::textGreen << "�� ɾ���ɹ�" << con::textColorDefault << std::endl;
		SubjectManager::getInstance().removeSubject(management.selectedSubjectId);
		waitAnyKeyPressed();
		Navigator::getInstance().pop();
	}

	ExistingSubjectManagementCommand::ExistingSubjectManagementCommand() : Command("�������пγ�") {}
	void ExistingSubjectManagementCommand::Invoke()
	{
		if (SubjectManager::getInstance().getSubjects().size() == 0)
		{
			std::cout << con::textRed << "! ������ӿγ�" << con::textColorDefault << std::endl;
			waitAnyKeyPressed();
			return;
		}
		auto breadcrumb = Breadcrumb("���пγ̹���");
		breadcrumb.addCommand(makeBackCommandPtr());
		breadcrumb.addCommand(std::make_unique<SubjectSelectionCommand>([this](int index) { this->selectedSubjectId = index; }));
		breadcrumb.addCommand(std::make_unique<SubjectRenameCommand>(*this));
		breadcrumb.addCommand(std::make_unique<SubjectDeleteCommand>(*this));
		Navigator::getInstance().push(std::move(breadcrumb));
		Navigator::getInstance().getAll().back().getCommands()[1]->Invoke();
	}

	SubjectManagementCommand::SubjectManagementCommand() : Command("����γ���Ϣ") {}
	void SubjectManagementCommand::Invoke()
	{
		auto breadcrumb = Breadcrumb("�γ̹���");
		breadcrumb.addCommand(makeBackCommandPtr());
		breadcrumb.addCommand(std::make_unique<SubjectNewCommand>());
		breadcrumb.addCommand(std::make_unique<ListSubjectCommand>());
		breadcrumb.addCommand(std::make_unique<ExistingSubjectManagementCommand>());
		Navigator::getInstance().push(std::move(breadcrumb));
	}

	StudentAddCommand::StudentAddCommand() : Command("���ѧ��") {}
	void StudentAddCommand::Invoke()
	{
		if (StudentManager::getInstance().getClasses().size() == 0)
		{
			std::cout << con::textRed << "! ������Ӱ༶" << con::textColorDefault << std::endl;
			waitAnyKeyPressed();
			return;
		}
		StudentInfo stu;
		std::cout << "������ѧ��������";
		std::cin >> stu.name;
		std::cout << "������ѧ��ѧ�ţ�";
		std::cin >> stu.id;
		std::cout << "��ѡ��ѧ���༶��";
		int x, y;
		con::getCursorPosition(x, y);
		std::cout << std::endl;
		stu.classId = selectOption(StudentManager::getInstance().getClasses(), 0);
		con::setCursorPosition(x, y);
		std::cout << StudentManager::getInstance().getClasses()[stu.classId] << '\n';
		std::cout << con::textGreen << "�� ѧ����ӳɹ�" << con::textColorDefault << std::endl;
		waitAnyKeyPressed();
	}

	ClassAddCommand::ClassAddCommand() : Command("��Ӱ༶") {}
	void ClassAddCommand::Invoke()
	{
		std::cout << "������༶���ƣ�";
		std::string name;
		std::cin >> name;
		int id = StudentManager::getInstance().addClass(std::move(name));
		std::cout << con::textGreen << "�� �༶��ӳɹ���IDΪ" << id << con::textColorDefault << std::endl;
		waitAnyKeyPressed();
	}

	StudentManagementCommand::StudentManagementCommand() : Command("����ѧ����Ϣ") {}
	void StudentManagementCommand::Invoke()
	{
		auto breadcrumb = Breadcrumb("ѧ������");
		breadcrumb.addCommand(makeBackCommandPtr());
		breadcrumb.addCommand(std::make_unique<StudentAddCommand>());
		breadcrumb.addCommand(std::make_unique<ClassAddCommand>());
		Navigator::getInstance().push(std::move(breadcrumb));
	}

	ExamManagerCommand::ExamManagerCommand() : Command("��������Ϣ") {}
	void ExamManagerCommand::Invoke()
	{
		auto breadcrumb = Breadcrumb("���Թ���");
		breadcrumb.addCommand(makeBackCommandPtr());
		Navigator::getInstance().push(std::move(breadcrumb));
	}
}