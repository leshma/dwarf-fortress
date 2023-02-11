#pragma once
#include <windows.h>
#include <boost/signals2.hpp>

// This class will serve as a medium between the users input through keyboard and the game logic.
class KeyboardController
{
public:
    void ListenToInputs()
    {
        const HANDLE handle = GetStdHandle(STD_INPUT_HANDLE);
        DWORD events;
        INPUT_RECORD buffer;
        while (true)
        {
            PeekConsoleInput( handle, &buffer, 1, &events );
            if(events > 0)
            {
                ReadConsoleInput(handle, &buffer, 1, &events);
                if (buffer.Event.KeyEvent.bKeyDown)
                {
                    if (buffer.Event.KeyEvent.wVirtualKeyCode == VK_ESCAPE)
                    {
                        return;
                    }
                    
                    SignalKeyboardPress(buffer.Event.KeyEvent.wVirtualKeyCode);
                }
            }
        }
    }
    
    boost::signals2::signal<void (WORD)> SignalKeyboardPress;
};
