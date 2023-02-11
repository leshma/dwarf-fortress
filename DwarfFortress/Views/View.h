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
    Coordinates Dimensions;
    std::shared_ptr<Environment> Environment;
    std::vector<std::vector<std::tuple<char, int>>> Objects;

    HANDLE ConsoleScreenHandle;
    CONSOLE_SCREEN_BUFFER_INFO ConsoleScreenInfo;
    
    View(std::shared_ptr<class Environment> environment) : Environment(std::move(environment)), ConsoleScreenHandle(GetStdHandle(STD_OUTPUT_HANDLE))
    {
        GetConsoleScreenBufferInfo(ConsoleScreenHandle, &ConsoleScreenInfo);

        Dimensions.X = ConsoleScreenInfo.srWindow.Right;
        Dimensions.Y = ConsoleScreenInfo.srWindow.Bottom;

        Objects = std::vector<std::vector<std::tuple<char, int>>>(Dimensions.Y, std::vector<std::tuple<char, int>>(Dimensions.X, std::tuple<char, int>(' ', BACKGROUND_GREEN)));
        for (int y = 0; y < Environment->Objects.size(); ++y)
            for (int x = 0; x < Environment->Objects[y].size(); ++x)
            {
                if (Environment->Objects[y][x] != nullptr)
                    switch (Environment->Objects[y][x]->GetType())
                    {
                        case TWall:
                            Objects[y][x] = std::tuple<char, int>(' ', FOREGROUND_GREEN | BACKGROUND_RED | BACKGROUND_INTENSITY);
                            break;
                        case TPlayer:
                            Objects[y][x] = std::tuple<char, int>('P', FOREGROUND_RED | BACKGROUND_RED | FOREGROUND_INTENSITY);
                            break;
                        case TDoor:
                            Objects[y][x] = std::tuple<char, int>(' ', BACKGROUND_GREEN);
                            break;
                        default:
                            break;
                    }
            }
    }

    void Draw()
    {
        for (int y = 0; y < Environment->Objects.size(); ++y)
            for (int x = 0; x < Environment->Objects[y].size(); ++x)
            {
                if (Environment->Objects[y][x] != nullptr)
                    switch (Environment->Objects[y][x]->GetType())
                    {
                        case TWall:
                            Objects[y][x] = std::tuple<char, int>(' ', FOREGROUND_GREEN | BACKGROUND_RED | BACKGROUND_INTENSITY);
                                break;
                        case TPlayer:
                            Objects[y][x] = std::tuple<char, int>('P', FOREGROUND_RED | BACKGROUND_RED | FOREGROUND_INTENSITY);
                                break;
                        case TDoor:
                            Objects[y][x] = std::tuple<char, int>(' ', BACKGROUND_GREEN);
                            break;
                        default:
                            Objects[y][x] = std::tuple<char, int>(' ', BACKGROUND_GREEN);
                            break;
                    }
            }
        
        DWORD written;
        for (byte y = 0; y < Objects.size(); ++y)
        {
            for (byte x = 0; x < Objects[y].size(); ++x)
            {
                FillConsoleOutputCharacterA(
                    ConsoleScreenHandle, std::get<0>(Objects[y][x]), 1, COORD {x, y}, &written
                );
                FillConsoleOutputAttribute(
                    ConsoleScreenHandle, std::get<1>(Objects[y][x]),
                    1, COORD {x, y}, &written
                );
            }
        }
    }
    
    void Clear() {
        COORD topLeft  = { 0, 0 };
        DWORD written;

        GetConsoleScreenBufferInfo(ConsoleScreenHandle, &ConsoleScreenInfo);
        FillConsoleOutputCharacterA(
            ConsoleScreenHandle, ' ', ConsoleScreenInfo.dwSize.X * ConsoleScreenInfo.dwSize.Y, topLeft, &written
        );
        FillConsoleOutputAttribute(
            ConsoleScreenHandle, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE,
            ConsoleScreenInfo.dwSize.X * ConsoleScreenInfo.dwSize.Y, topLeft, &written
        );
        SetConsoleCursorPosition(ConsoleScreenHandle, topLeft);
    }
};
