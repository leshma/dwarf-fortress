#pragma once
#include <windows.h>
#include <boost/signals2.hpp>

// This class will serve as a medium between the users input through keyboard and the game logic.
class KeyboardController
{
public:
    /**
     * \brief A method that indefinitely listens to the player input.
     */
    WORD ListenToInputs();

    boost::signals2::signal<void (WORD)> SignalKeyboardPress;
    boost::signals2::signal<void (std::string)> SignalActionTextChanged;
};
