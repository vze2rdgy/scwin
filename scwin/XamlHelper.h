#pragma once

#include "cpplinq.hpp"

using namespace Windows::UI;
using namespace Windows::UI::Xaml;

namespace scwin
{
	class XamlHelper
	{
	public:
		template<typename T>
		static bool FindAncestor(FrameworkElement^ ofElement, T^* ancestor)
		{
			if (ofElement == nullptr)
			{
				(*ancestor) = nullptr;
				return false;
			}
			FrameworkElement^ ans = dynamic_cast<FrameworkElement^>(ofElement->Parent);
			while (true)
			{
				if (ans == nullptr)
				{
					(*ancestor) = nullptr;
					return false;
				}
				OutputDebugString(ans->GetType()->FullName->Data());
				OutputDebugString(L"\n");
				(*ancestor) = dynamic_cast<T^>(ans);
				if ((*ancestor) != nullptr)
				{
					return true;
				}
				//if (ans->Parent != nullptr)
				//{
				//	OutputDebugString(ans->Parent->GetType()->FullName->Data());
				//	OutputDebugString(L"\n");
				//}
				ans = dynamic_cast<FrameworkElement^>(ans->Parent);
			}
			return false;
		}


		static std::vector<DependencyObject^> GetAncesters(DependencyObject^ start);

		template<typename T>
		static bool GetAncester(DependencyObject^ start, T^* ancester)
		{
			using namespace cpplinq;
			auto parents = GetAncesters(start);
			if (!parents.size())
				return false;
			auto ans = from(parents) >> where([=](DependencyObject^ dp)
			{
				return dynamic_cast<T^>(dp) != nullptr;
			}) >> first_or_default();
			*ancester = (T^)ans;
			return (ans != nullptr);
		}

		static bool GetChildFromTree(DependencyObject^ startParent, String^ Name, DependencyObject^* child);

		template<typename T>
		static bool GetChildFromTree(DependencyObject^ startParent, T^* child);

		template<typename T>
		static bool GetChildrenOfType(FrameworkElement^ root, IVector<T^>^& children);
	};


	template<typename T>
	inline bool XamlHelper::GetChildFromTree(DependencyObject ^ startParent, T ^* child)
	{
		if (startParent == nullptr)
			return false;

		for (int i = 0; i < VisualTreeHelper::GetChildrenCount(startParent); i++)
		{
			auto uie = VisualTreeHelper::GetChild(startParent, i);
			auto ctl = dynamic_cast<T^>(uie);
			if (ctl != nullptr)
			{
				*child = ctl;
				return true;
			}
			if (GetChildFromTree(uie, child))
			{
				return true;
			}
		}
		return false;
	}

	template<typename T>
	inline bool XamlHelper::GetChildrenOfType(FrameworkElement ^ root, IVector<T^>^& children)
	{
		if (root == nullptr)
			return false;
		
		if (children == nullptr)
			children = ref new Vector<T^>();

		for (int i = 0; i < VisualTreeHelper::GetChildrenCount(root); i++)
		{
			auto uie = dynamic_cast<FrameworkElement^>(VisualTreeHelper::GetChild(root, i));
			if (uie == nullptr)
				continue;

			auto ctl = dynamic_cast<T^>(uie);
			if (ctl != nullptr)
			{
				children->Append(ctl);
			}

			if (GetChildrenOfType(uie, children))
			{
				return true;
			}
		}
		return true;
	}
}
