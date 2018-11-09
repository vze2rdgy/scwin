#pragma once

namespace scwin
{

	[Bindable]
	ref class scViewModelBase : DependencyObject
	{
	protected private:
		scViewModelBase();

	protected private:
		template<typename T> void SetProperty(String^ propName, T& storage, T newVal)
		{
			if (storage != newVal)
			{
				storage = newVal;
			}
		}

	public:
		virtual ~scViewModelBase();

		property Exception^ LastException
		{
		public:
			Exception^ get();
		private:
			void set(Exception^ ex);
		}


	};

}

