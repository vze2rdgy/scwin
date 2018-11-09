#include "pch.h"
#include "XamlHelper.h"

std::vector<DependencyObject^> scwin::XamlHelper::GetAncesters(DependencyObject ^ start)
{
	std::vector<DependencyObject^> parents;
	auto par = VisualTreeHelper::GetParent(start);
	while (par != nullptr)
	{
		parents.emplace_back(par);
		par = VisualTreeHelper::GetParent(par);
	}
	return std::move(parents);
}

bool scwin::XamlHelper::GetChildFromTree(DependencyObject ^ startParent, String^ Name, DependencyObject^* child)
{
	if (startParent == nullptr || StringHelpers::IsNullOrEmpty(Name))
		return false;

	auto currCtl = dynamic_cast<Control^>(startParent);
	if (currCtl != nullptr && currCtl->Name == Name)
	{
		*child = currCtl;
		return true;
	}

	for (int i = 0; i < VisualTreeHelper::GetChildrenCount(startParent); i++)
	{
		auto uie = VisualTreeHelper::GetChild(startParent, i);
		auto ctl = dynamic_cast<Control^>(uie);
		if (ctl != nullptr && ctl->Name == Name)
		{
			*child = ctl;
			return true;
		}
		if (GetChildFromTree(uie, Name, child))
		{
			return true;
		}
	}
	return false;
}
