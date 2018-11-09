#include "pch.h"
#include "StartupEventArgs.h"

using namespace scwin;

StartupEventArgs::StartupEventArgs()
{

}

scwin::StartupEventArgs::StartupEventArgs(LandingContentStage From, LandingContentStage To, Windows::Security::Credentials::KeyCredentialStatus Status)
{
	this->From = From;
	this->To = To;
	this->Status = Status;
}
