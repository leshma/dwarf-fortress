#pragma once
#include <iostream>
#include <tuple>
#include <vector>
#include <windows.h>

#include "../Models/Coordinates.h"
#include "../Models/Environment.h"

// This class is used to draw onto the console.
class View
{
public:
    // Dimensions of the View.
    Coordinates Dimensions;

    // The Environment from which the View pulls the game data.
    std::shared_ptr<Environment> Environment;

    // Objects in the View represented by a character and a foreground/background colour.
    std::vector<std::vector<std::tuple<char, int>>> Objects;

    // Console screen handle.
    HANDLE ConsoleScreenHandle;

    // Console info.
    CONSOLE_SCREEN_BUFFER_INFO ConsoleScreenInfo;
    
    View(std::shared_ptr<class Environment> environment);

    // This is used to initially draw the game grid.
    void InitialDraw();

    // Draws the changes that happened to the Environment and to the Player status.
    void DrawChanges(std::vector<EnvironmentChange> environmentChanges);
    
    // Draws the changes that happened to the Environment.
    void DrawEnvironmentChanges(std::vector<EnvironmentChange> changes);

    // Draws the changes that happened to the Player status.
    void DrawStatusChanges();

    // Draws the action text that shows up when the Player interacts with the world.
    void DrawActionText(std::string newActionText);

    // Clears the View.
    void Clear();

    // Clears a row in the View.
    void ClearRow(COORD rowStart, COORD rowEnd = {-1, -1});

private:
    // This represents the starting coordinates of the Player status printout.
    COORD _statusPosition {0, 29};

    // The length of the Player status printout.
    short _statusLength = 40;

    // The maximum length of the action text.
    short _actionTextLength = 80;

    // The action text.
    std::string _actionText;

    // Appends the new action text to the current one.
    void AppendToActionText(std::string newActionText);
};
