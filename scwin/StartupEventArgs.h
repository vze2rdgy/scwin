#pragma once

using namespace Platform;

namespace scwin 
{
	

	public ref class StartupEventArgs sealed
	{
	public:
		StartupEventArgs();
		StartupEventArgs(LandingContentStage From, LandingContentStage To, Windows::Security::Credentials::KeyCredentialStatus Status);
		property LandingContentStage From;
		property LandingContentStage To;
		property Windows::Security::Credentials::KeyCredentialStatus Status;
	};

	public delegate void StartupEventHandler(Object^ sender, StartupEventArgs^ ergs);

}