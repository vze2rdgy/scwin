#pragma once

namespace scwin 
{
	ref class scMainViewModel;

	[Bindable]
	public ref class scSignupViewModel sealed : INotifyPropertyChanged
	{
	private:
		String^ fullname;
		String^ email;
		String^ pwd;
		String^ company;
		Command^ cmdSignIn;
		Command^ cmdSignUp;

	protected:
		void NotifyPropertyChanged(String^ propName);

	public:
		virtual event PropertyChangedEventHandler^ PropertyChanged;

	public:
		scSignupViewModel(scMainViewModel^ mainvm);
		virtual ~scSignupViewModel();

		property scMainViewModel^ Parent;

		property String^ FullName
		{
			String^ get();
			void set(String^ val);
		}

		property String^ Email
		{
			String^ get();
			void set(String^ val);
		}

		property String^ Password
		{
			String^ get();
			void set(String^ val);
		}

		property String^ Company
		{
			String^ get();
			void set(String^ val);
		}

		property ICommand^ SignInCommand
		{
			ICommand^ get();
		}
		
		property ICommand^ SignUpCommand
		{
			ICommand^ get();
		}
	};
}