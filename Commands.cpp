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
		return std::make_unique<BackCommand>("����");
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
		: Command("ѡ��һ���γ�"), callback{ callback }, selecting(selecting) {}
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

	SubjectRenameCommand::SubjectRenameCommand(ExistingSubjectManagementCommand& management)
		: Command("�������γ�"), management{ management } {}
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

	SubjectDeleteCommand::SubjectDeleteCommand(ExistingSubjectManagementCommand& management)
		: Command("ɾ���γ�"), management{ management } {}
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
			std::cout << con::textRed << "! ɾ������ȡ�������벻ƥ��" << con::textColorDefault << std::endl;
		}
		else
		{
			std::cout << con::textGreen << "�� ɾ���ɹ�" << con::textColorDefault << std::endl;
			SubjectManager::getInstance().removeSubject(management.selectedSubjectId);
			Navigator::getInstance().pop();
		}
		waitAnyKeyPressed();
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
		if (StudentManager::getInstance().getStudentById(stu.id) != nullptr)
			std::cout << con::textRed << "! ��ѧ���Ѵ��ڣ����ʧ��" << con::textColorDefault << std::endl;
		else
		{
			StudentManager::getInstance().addStudent(stu);
			std::cout << con::textGreen << "�� ѧ����ӳɹ�" << con::textColorDefault << std::endl;
		}
		waitAnyKeyPressed();
	}

	StudentListCommand::StudentListCommand() : Command("�г�����ѧ��") {}
	void StudentListCommand::Invoke()
	{
		auto& students = StudentManager::getInstance().getStudents();
		clearConsole();
		std::cout << "��ǰһ���� " << students.size() << " ��ѧ��" << std::endl;
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
		std::cout << "����" << con::cha(idPos) << "ѧ��" << con::cha(classPos) << "�༶" << std::endl;
		for (auto& stu : students)
			std::cout << stu->name << con::cha(idPos) << stu->id << con::cha(classPos) << classes[stu->classId] << std::endl;
		waitEnterPressed();
	}

	ExistingStudentManagementCommand::ExistingStudentManagementCommand()
		: Command("��������ѧ��"), selectedStudentPtr(nullptr) {}
	void ExistingStudentManagementCommand::Invoke()
	{
		if (StudentManager::getInstance().getStudents().size() == 0)
		{
			std::cout << con::textRed << "! �������ѧ��" << con::textColorDefault << std::endl;
			waitAnyKeyPressed();
			return;
		}

		auto breadcrumb = Breadcrumb("����ѧ������");
		breadcrumb.addCommand(makeBackCommandPtr());
		breadcrumb.addCommand(std::make_unique<StudentSelectionCommand>(*this));
		breadcrumb.addCommand(std::make_unique<StudentRenameCommand>(*this));
		Navigator::getInstance().push(std::move(breadcrumb));
		Navigator::getInstance().getAll().back().getCommands()[1]->Invoke();
	}

	StudentSelectionCommand::StudentSelectionCommand(ExistingStudentManagementCommand& management)
		: Command("ѡ��һ��ѧ��"), management{ management } {}
	void StudentSelectionCommand::Invoke()
	{
		auto breadcrumb = Breadcrumb("ѡ��ѧ��");
		breadcrumb.addCommand(makeBackCommandPtr());
		breadcrumb.addCommand(std::make_unique<StudentSelectionCommand::ByIdCommand>(*this));
		breadcrumb.addCommand(std::make_unique<StudentSelectionCommand::ByNameCommand>(*this));
		Navigator::getInstance().push(std::move(breadcrumb));
	}
	void StudentSelectionCommand::UpdateSelecting(std::shared_ptr<StudentInfo> studentPtr)
	{
		if (studentPtr == nullptr)
			this->title = "ѡ��һ��ѧ��";
		else
			this->title = std::format("ѡ��һ��ѧ������ǰѡ�У�{}��", studentPtr->name);
		management.selectedStudentPtr = studentPtr;
	}

	StudentSelectionCommand::ByIdCommand::ByIdCommand(StudentSelectionCommand& parent)
		: Command("��ID����"), parent{ parent } {}
	void StudentSelectionCommand::ByIdCommand::Invoke()
	{
		std::string id;
		std::cout << "������ѧ��ѧ�ţ�";
		std::cin >> id;
		auto pStu = StudentManager::getInstance().getStudentById(id);
		if (pStu == nullptr)
			std::cout << con::textRed << "! δ�ҵ�ѧ��Ϊ" << id << "��ѧ��" << con::textColorDefault << std::endl;
		else
		{
			parent.UpdateSelecting(pStu);
			std::cout << con::textGreen << "�� ��ѡ��ѧ�� " << pStu->name << "��" << pStu->id << "��" << con::textColorDefault << std::endl;
			Navigator::getInstance().pop();
		}
		waitAnyKeyPressed();
	}

	StudentSelectionCommand::ByNameCommand::ByNameCommand(StudentSelectionCommand& parent)
		: Command("������ģ������"), parent{ parent } {}
	void StudentSelectionCommand::ByNameCommand::Invoke()
	{
		std::string name;
		std::cout << "������ѧ��������";
		std::cin >> name;
		auto students = StudentManager::getInstance().getStudentsByNameRegex(name);
		if (students.size() == 0)
		{
			std::cout << con::textRed << "! δ�ҵ������а���\"" << name << "\"��ѧ��" << con::textColorDefault << std::endl;
			waitAnyKeyPressed();
			students.clear();
			return;
		}

		if (students.size() > 20)
		{
			std::cout << con::textYellow << "! ƥ�䵽����20��ѧ����ֻ��ʾǰ20��" << con::textColorDefault << std::endl;
			students.erase(students.begin() + 20, students.end());
		}
		std::vector<std::string> names(students.size());
		auto ids = students | std::views::transform([](const auto& p) -> int { return dummyStrLenCalc(p->id); });
		int idLongest = *std::max_element(ids.begin(), ids.end());
		idLongest = max(idLongest, 8);
		for (int i = 0; i < students.size(); i++)
			names[i] = students[i]->id + con::chaCmd(1 + idLongest + 2) + students[i]->name;

		std::cout << "��ѡ��һ��ѧ����" << std::flush;
		int x, y;
		con::getCursorPosition(x, y);
		std::cout << '\n';
		int selected = selectOption(names, 0);
		auto& pStu = students[selected];
		parent.management.selectedStudentPtr = pStu;
		con::setCursorPosition(x, y);
		std::cout << pStu->name << "��" << pStu->id << "��" << '\n';
		std::cout << con::textGreen << "�� ѡ�����" << con::textColorDefault << std::endl;
		Navigator::getInstance().pop();
		students.clear();
		waitAnyKeyPressed();
	}

	StudentRenameCommand::StudentRenameCommand(ExistingStudentManagementCommand& management)
		: Command("������ѧ��"), management{ management } {}
	void StudentRenameCommand::Invoke()
	{
		if (management.selectedStudentPtr == nullptr)
		{
			std::cout << con::textRed << "! ����ѡ��һ��ѧ��" << con::textColorDefault << std::endl;
			waitAnyKeyPressed();
			return;
		}
		std::cout << "�������µ�������";
		std::string name;
		std::cin >> name;
		management.selectedStudentPtr->name = name;
		std::cout << con::textGreen << "�� �������ɹ�" << con::textColorDefault << std::endl;
		Navigator::getInstance().pop();
		waitAnyKeyPressed();
	}

	StudentDeleteCommand::StudentDeleteCommand(ExistingStudentManagementCommand& management)
		: Command("ɾ��ѧ��"), management{ management } {}
	void StudentDeleteCommand::Invoke()
	{
		if (management.selectedStudentPtr == nullptr)
		{
			std::cout << con::textRed << "! ����ѡ��һ��ѧ��" << con::textColorDefault << std::endl;
			waitAnyKeyPressed();
			return;
		}
		auto& name = management.selectedStudentPtr->name;
		std::cout << con::textYellow << "? ��ȷ��Ҫɾ��ѧ��\"" << name << "\"��" << con::textColorDefault << std::endl;
		waitEnterPressed();
		std::cout << con::textYellow << "? �����Ҫɾ��ѧ��\"" << name << "\"��" << con::textColorDefault << std::endl;
		waitEnterPressed();
		std::cout << "���ȷ��ɾ��������\"" << name << "\"��";
		std::string input;
		std::cin >> input;
		if (input != name)
			std::cout << con::textRed << "! ɾ������ȡ�������벻ƥ��" << con::textColorDefault << std::endl;
		else
		{
			StudentManager::getInstance().removeStudent(std::move(management.selectedStudentPtr));
			management.selectedStudentPtr = nullptr;
			Navigator::getInstance().pop();
		}
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
		breadcrumb.addCommand(std::make_unique<StudentListCommand>());
		breadcrumb.addCommand(std::make_unique<ExistingStudentManagementCommand>());
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