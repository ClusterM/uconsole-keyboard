#include "uconsolekeyboardplugin.h"
#include "keyboardstate.h"
#include <qqml.h>

void UConsoleKeyboardPlugin::registerTypes(const char *uri)
{
    qmlRegisterType<KeyboardState>(uri, 1, 0, "KeyboardState");
}
