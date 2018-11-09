//
// AppDataTemplates.xaml.h
// Declaration of the AppDataTemplates class
//

#pragma once


namespace scwin
{

	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class AppDataTemplates sealed : public Windows::UI::Xaml::ResourceDictionary,
		public ::Windows::UI::Xaml::Markup::IComponentConnector,
		public ::Windows::UI::Xaml::Markup::IComponentConnector2
	{
	public:
		AppDataTemplates();
		void InitializeComponent();
		virtual void Connect(int connectionId, ::Platform::Object^ target);
		virtual ::Windows::UI::Xaml::Markup::IComponentConnector^ GetBindingConnector(int connectionId, ::Platform::Object^ target);

	private:
		void UnloadObject(::Windows::UI::Xaml::DependencyObject^ dependencyObject);
		void DisconnectUnloadedObject(int connectionId);

	private:
		bool _contentLoaded;
	};
}
