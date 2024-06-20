#include "Navigator.hpp"
#include "Breadcrumb.hpp"
#include <list>

namespace esm
{
	Navigator& Navigator::getInstance()
	{
		static Navigator instance;
		return instance;
	}

	void Navigator::push(Breadcrumb&& breadcrumb)
	{
		navigator.push_back(std::move(breadcrumb));
	}

	void Navigator::pop()
	{
		navigator.pop_back();
	}

	std::list<Breadcrumb>& Navigator::getAll()
	{
		return navigator;
	}
}