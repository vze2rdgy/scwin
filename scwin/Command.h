#pragma once

namespace fmeta = Windows::Foundation::Metadata;

namespace scwin
{
	public delegate void ExecuteAction(Platform::Object^ p);
	public delegate bool CanExecuteFunc(Platform::Object^ p);

	[fmeta::WebHostHidden]
	[Bindable]
	public ref class Command sealed : ICommand
	{
	private:
		ExecuteAction^ executeAction;
		CanExecuteFunc^ canExecFunc;

	public:
		Command(ExecuteAction^ executeAction, CanExecuteFunc^ canExecFunc);
		virtual ~Command();

		// Inherited via ICommand
		virtual event Windows::Foundation::EventHandler<Platform::Object ^> ^ CanExecuteChanged;

		virtual bool CanExecute(Platform::Object ^parameter);

		virtual void Execute(Platform::Object ^parameter);

		// Update command state.
		void Refresh(Platform::Object^ parameter);
		void Refresh();
	};

}