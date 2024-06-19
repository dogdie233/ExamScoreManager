#include "Utils.cpp"
#include "Commands.cpp"

namespace esm
{
	static void addMain()
	{
		auto breadcrumb = Breadcrumb("主页");
		breadcrumb.addCommand(std::make_unique<BackCommand>("退出"));
		navigator.push_back(std::move(breadcrumb));
	}
}

using namespace esm;

int main()
{
	addMain();
	while (!navigator.empty())
	{
		clearConsole();
		printNavigator();
		auto& tail = *navigator.rbegin();

		printCommands(tail);
		askAndInvokeCommands(tail);
	}
	std::cout << "程序已退出" << std::endl;
	return 0;
}