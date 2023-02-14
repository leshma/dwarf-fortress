#include "KeyboardController.h"

WORD KeyboardController::ListenToInputs()
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
                switch (buffer.Event.KeyEvent.wVirtualKeyCode)
                {
                case VK_ESCAPE: // Exit the game
                    SignalActionTextChanged("Exiting the game...");
                    return buffer.Event.KeyEvent.wVirtualKeyCode;
                case VK_F1: // Save
                    SignalActionTextChanged("Saved the game.");
                    return buffer.Event.KeyEvent.wVirtualKeyCode;
                case VK_F2: // Load
                    SignalActionTextChanged("Loaded the game.");
                    return buffer.Event.KeyEvent.wVirtualKeyCode;
                default:
                    break;
                }
                    
                SignalKeyboardPress(buffer.Event.KeyEvent.wVirtualKeyCode);
            }
        }
    }
}
