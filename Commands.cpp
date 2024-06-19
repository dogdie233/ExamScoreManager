#include "Utils.cpp";

namespace esm
{
	class BackCommand : public Command
	{
	public:
		BackCommand(const std::string title) : Command(title) {}

		void Invoke() override
		{
			navigator.pop_back();
		}
	};

	static std::unique_ptr<BackCommand> MakeBackCommand()
	{
		return std::make_unique<BackCommand>("·µ»Ø");
	}
}