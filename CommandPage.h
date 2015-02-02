#pragma once
#include "Command.h"

using CommandButton = Command;

struct CommandPage
{
   std::wstring                name;
   std::vector<CommandButton>  buttons;
};


