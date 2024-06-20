#pragma once

#ifndef ESM_NAVIGATOR_HPP
#define ESM_NAVIGATOR_HPP

#include <list>

namespace esm
{
	class Breadcrumb;

	class Navigator
	{
	public:
		static Navigator& getInstance();

		void push(Breadcrumb&& breadcrumb);
		void pop();
		std::list<Breadcrumb>& getAll();

	private:
		std::list<Breadcrumb> navigator;
		Navigator() = default;
		~Navigator() = default;

		Navigator(const Navigator&) = delete;
		const Navigator& operator=(const Navigator&) = delete;
	};
}

#endif
