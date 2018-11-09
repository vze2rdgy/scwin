#pragma once

namespace scwin 
{
	ref class scMainViewModel;

	[Bindable]
	public ref class scSignInViewModel sealed : public INotifyPropertyChanged
	{

	private:
		String^ email;
		String^ pwd;
		bool remMe;
		Command^ cmdSignIn;
		Command^ cmdSignUp;

	protected:
		void NotifyPropertyChanged(String^ propName);

	public:
		virtual event PropertyChangedEventHandler^ PropertyChanged;

	public:
		scSignInViewModel(scMainViewModel^ mainvm);

		virtual ~scSignInViewModel();

		property scMainViewModel^ Parent;

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

		property bool RememberMe
		{
			bool get();
			void set(bool val);
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