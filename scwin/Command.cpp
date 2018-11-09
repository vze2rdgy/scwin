#include "pch.h"
#include "Command.h"

using namespace scwin;



Command::Command(ExecuteAction^ executeAction, CanExecuteFunc^ canExecFunc)
{
	this->canExecFunc = canExecFunc;
	this->executeAction = executeAction;
}


Command::~Command()
{
}

bool Command::CanExecute(Platform::Object ^parameter)
{
	return canExecFunc(parameter);
}

void Command::Execute(Platform::Object ^parameter)
{
	executeAction(parameter);
}

void Command::Refresh(Object^ parameter)
{
	this->CanExecuteChanged(this, parameter);
}

void scwin::Command::Refresh()
{
	this->Refresh(nullptr);
}
