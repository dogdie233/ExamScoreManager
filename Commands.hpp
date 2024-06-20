#pragma once

#ifndef ESM_COMMANDS_HPP
#define ESM_COMMANDS_HPP

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

	class ExamManagerCommand : public Command
	{
	public:
		ExamManagerCommand();

		void Invoke() override;
	};
}

#endif