#pragma once

#ifndef ESM_COMMANDS_HPP
#define ESM_COMMANDS_HPP

#include "Student.hpp"
#include "ExamTable.hpp"
#include "Command.hpp"
#include <memory>
#include <functional>

namespace esm
{
	class BackCommand : public Command
	{
	public:
		BackCommand(const std::string& title);

		void Invoke() override;
	};

	std::unique_ptr<BackCommand> makeBackCommandPtr();

	class LambdaCommand : public Command
	{
	public:
		LambdaCommand(const std::string& title, std::function<void(void)>);

		void Invoke() override;

	private:
		std::function<void(void)> callback;
	};

	class TestCommand : public Command
	{
	public:
		TestCommand(const std::string& title, int depth);

		void Invoke() override;

	private:
		int depth;
	};

	class SubjectNewCommand : public Command
	{
	public:
		SubjectNewCommand();

		void Invoke() override;
	};

	class ListSubjectCommand : public Command
	{
	public:
		ListSubjectCommand();

		void Invoke() override;
	};

	class SubjectSelectionCommand : public Command
	{
	public:
		int selecting;

		SubjectSelectionCommand(std::function<void(int)> callback, int selecting = -1);

		void Invoke() override;

	protected:
		void UpdateSelecting(int selecting);

		class OptionCommand : public Command
		{
		public:
			int index;

			OptionCommand(std::string&& optionTitle, SubjectSelectionCommand& parent, int index);

			void Invoke() override;

		private:
			SubjectSelectionCommand& parent;
		};

	private:
		std::function<void(int)> callback;
	};

	class ExistingSubjectManagementCommand : public Command
	{
	public:
		int selectedSubjectId = -1;

		ExistingSubjectManagementCommand();

		void Invoke() override;
	};

	class SubjectRenameCommand : public Command
	{
	public:
		SubjectRenameCommand(ExistingSubjectManagementCommand& management);

		void Invoke() override;
	private:
		ExistingSubjectManagementCommand& management;
	};

	class SubjectDeleteCommand : public Command
	{
	public:
		SubjectDeleteCommand(ExistingSubjectManagementCommand& management);

		void Invoke() override;
	private:
		ExistingSubjectManagementCommand& management;
	};

	class SubjectManagementCommand : public Command
	{
	public:
		SubjectManagementCommand();

		void Invoke() override;
	};

	class StudentAddCommand : public Command
	{
	public:
		StudentAddCommand();

		void Invoke() override;
	};

	class StudentListCommand : public Command
	{
	public:
		StudentListCommand();

		void Invoke() override;
	};

	class ExistingStudentManagementCommand : public Command
	{
	public:
		std::shared_ptr<StudentInfo> selectedStudentPtr = nullptr;

		ExistingStudentManagementCommand();

		void Invoke() override;
	};

	class StudentSelectionCommand : public Command
	{
	public:
		std::shared_ptr<StudentInfo> pStu;

		StudentSelectionCommand(std::function<void(std::shared_ptr<StudentInfo>)> callback, std::shared_ptr<StudentInfo> pStu);

		void Invoke() override;

		void UpdateSelecting(std::shared_ptr<StudentInfo> studentPtr);

	protected:
		class ByIdCommand : public Command
		{
		public:
			StudentSelectionCommand& parent;

			ByIdCommand(StudentSelectionCommand& parent);

			void Invoke() override;
		};

		class ByNameCommand : public Command
		{
		public:
			StudentSelectionCommand& parent;

			ByNameCommand(StudentSelectionCommand& parent);

			void Invoke() override;
		};

	private:
		std::function<void(std::shared_ptr<StudentInfo>)> callback;
	};

	class StudentRenameCommand : public Command
	{
	public:
		ExistingStudentManagementCommand& management;

		StudentRenameCommand(ExistingStudentManagementCommand& management);

		void Invoke() override;
	};

	class StudentDeleteCommand : public Command
	{
	public:
		ExistingStudentManagementCommand& management;

		StudentDeleteCommand(ExistingStudentManagementCommand& management);

		void Invoke() override;
	};

	class ClassAddCommand : public Command
	{
	public:
		ClassAddCommand();
	
		void Invoke() override;
	};

	class StudentManagementCommand : public Command
	{
	public:
		StudentManagementCommand();

		void Invoke() override;
	};

	class ExamNewCommand : public Command
	{
	public:
		ExamNewCommand();

		void Invoke() override;
	};

	class ExamListCommand : public Command
	{
	public:
		ExamListCommand();

		void Invoke() override;
	};

	class ExamSelectionCommand : public Command
	{
	public:
		std::shared_ptr<ExamTable> pExam;

		ExamSelectionCommand(std::function<void(std::shared_ptr<ExamTable>)> callback, std::shared_ptr<ExamTable> pExam);

		void Invoke() override;

		void UpdateSelecting(std::shared_ptr<ExamTable> pExam);

	private:
		std::function<void(std::shared_ptr<ExamTable>)> callback;

	protected:
		class OptionCommand : public Command
		{
		public:
			std::shared_ptr<ExamTable> pExam;

			OptionCommand(std::string&& optionTitle, ExamSelectionCommand& parent, std::shared_ptr<ExamTable> pExam);

			void Invoke() override;

		private:
			ExamSelectionCommand& parent;
		};
	};

	class ExistingExamManagementCommand : public Command
	{
	public:
		std::shared_ptr<ExamTable> pExam;

		ExistingExamManagementCommand();

		void Invoke() override;
	};

	class ExamRenameCommand : public Command
	{
	public:
		ExistingExamManagementCommand& management;

		ExamRenameCommand(ExistingExamManagementCommand& management);

		void Invoke() override;
	};

	class ExamDeleteCommand : public Command
	{
	public:
		ExamDeleteCommand(ExistingExamManagementCommand& management);

		void Invoke() override;

	private:
		ExistingExamManagementCommand& management;
	};

	class ExamSubjectUpdateCommand : public Command
	{
	public:
		ExamSubjectUpdateCommand(ExistingExamManagementCommand& management);

		void Invoke() override;

	private:
		ExistingExamManagementCommand& management;
	};

	class ExamScorePrintCommand : public Command
	{
	public:
		ExamScorePrintCommand(ExistingExamManagementCommand& management);

		void Invoke() override;

	private:
		ExistingExamManagementCommand& management;
	};

	class ExamScoreAddBatchCommand : public Command
	{
	public:
		ExamScoreAddBatchCommand(ExistingExamManagementCommand& management);

		void Invoke() override;
		
	private:
		ExistingExamManagementCommand& management;
	};

	class ExamScoreUpdateOrCreateCommand : public Command
	{
	public:
		ExamScoreUpdateOrCreateCommand(ExistingExamManagementCommand& management);

		void Invoke() override;

	private:
		ExistingExamManagementCommand& management;
		StudentSelectionCommand studentSelectionCmd;

		void OnStudentSelected(std::shared_ptr<StudentInfo> pStu);
	};

	class ExamScoreDeleteCommand : public Command
	{
	public:
		ExamScoreDeleteCommand(ExistingExamManagementCommand& management);

		void Invoke() override;

	private:
		ExistingExamManagementCommand& management;
		StudentSelectionCommand studentSelectionCmd;

		void OnStudentSelected(std::shared_ptr<StudentInfo> pStu);
	};

	class ExamManagementCommand : public Command
	{
	public:
		ExamManagementCommand();

		void Invoke() override;
	};
}

#endif