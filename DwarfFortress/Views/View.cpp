#include "View.h"

View::View(std::shared_ptr<::Environment> environment): Environment(std::move(environment)), ConsoleScreenHandle(GetStdHandle(STD_OUTPUT_HANDLE))
{
    GetConsoleScreenBufferInfo(ConsoleScreenHandle, &ConsoleScreenInfo);

    Dimensions.X = ConsoleScreenInfo.srWindow.Right;
    Dimensions.Y = ConsoleScreenInfo.srWindow.Bottom;

    Objects = std::vector<std::vector<std::tuple<char, int>>>(Dimensions.Y, std::vector<std::tuple<char, int>>(Dimensions.X, std::tuple<char, int>(' ', BACKGROUND_GREEN)));
    for (int y = 0; y < Environment->Objects.size(); ++y)
    {
        for (int x = 0; x < Environment->Objects[y].size(); ++x)
        {
            if (Environment->Objects[y][x] != nullptr)
                switch (Environment->Objects[y][x]->GetType())
                {
                case TWall:
                    Objects[y][x] = std::tuple<char, int>(' ', BACKGROUND_RED | BACKGROUND_GREEN);
                    break;
                case TPlayer:
                    Objects[y][x] = std::tuple<char, int>('P', FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_BLUE | FOREGROUND_INTENSITY);
                    break;
                case TDoor:
                    Objects[y][x] = std::tuple<char, int>(' ', BACKGROUND_GREEN);
                    break;
                case TItem:
                    {
                        char printCharacter = 'i';
                        std::shared_ptr<Item> item = std::dynamic_pointer_cast<Item>(Environment->Objects[y][x]);
                        if (item->PersistanceType == TEquippable)
                        {
                            switch (item->ItemType)
                            {
                            case TWeapon:
                                printCharacter = 'w';
                                break;
                            case TArmor:
                                printCharacter = 'a';
                                break;
                            default:
                                break;
                            }
                        }
                        else
                        {
                            printCharacter = 'c';
                        }

                        Objects[y][x] = std::tuple<char, int>(printCharacter, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | BACKGROUND_BLUE);
                    }
                    break;
                case TEnemy:
                    Objects[y][x] = std::tuple<char, int>('!', FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | BACKGROUND_RED);
                    break;
                default:
                    break;
                }
        }
    }
}

void View::InitialDraw()
{
    // Draw the game
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

    DrawStatusChanges();
    DrawActionText("1-Weapon Change, 2-Armor Change, 3-Drink Consumable, F1-Save, F2-Load");
}

void View::DrawChanges(std::vector<EnvironmentChange> environmentChanges)
{
    DrawEnvironmentChanges(environmentChanges);
    DrawStatusChanges();
}

void View::DrawEnvironmentChanges(std::vector<EnvironmentChange> changes)
{
    DWORD written;
    for (const auto change : changes)
    {
        if (change.FromObjectDisappears) Objects[change.From.Y][change.From.X] = std::tuple<char, int>(' ', BACKGROUND_GREEN);
        if (change.ToObjectDisappears) Objects[change.To.Y][change.To.X] = std::tuple<char, int>(' ', BACKGROUND_GREEN);
            
        const std::tuple<char, int> temp = Objects[change.From.Y][change.From.X];
        Objects[change.From.Y][change.From.X] = Objects[change.To.Y][change.To.X];
        Objects[change.To.Y][change.To.X] = temp;

        FillConsoleOutputCharacterA(
            ConsoleScreenHandle, std::get<0>(Objects[change.From.Y][change.From.X]), 1,
            COORD{static_cast<short>(change.From.X), static_cast<short>(change.From.Y)}, &written
        );
        FillConsoleOutputAttribute(
            ConsoleScreenHandle, std::get<1>(Objects[change.From.Y][change.From.X]),
            1, COORD {static_cast<short>(change.From.X), static_cast<short>(change.From.Y)}, &written
        );

        FillConsoleOutputCharacterA(
            ConsoleScreenHandle, std::get<0>(Objects[change.To.Y][change.To.X]), 1,
            COORD{static_cast<short>(change.To.X), static_cast<short>(change.To.Y)}, &written
        );
        FillConsoleOutputAttribute(
            ConsoleScreenHandle, std::get<1>(Objects[change.To.Y][change.To.X]),
            1, COORD {static_cast<short>(change.To.X), static_cast<short>(change.To.Y)}, &written
        );
    }
}

void View::DrawStatusChanges()
{
    Coordinates playerPosition = Environment->PlayerPosition;
    std::shared_ptr<Player> player = std::dynamic_pointer_cast<Player>(Environment->Objects[playerPosition.Y][playerPosition.X]);
        
    double health = player->Health, damage = 0.00, armor = 0.00;
    if (player->Inventory->Weapon != nullptr)
    {
        for (const auto weaponStatistic : player->Inventory->Weapon->Statistics)
        {
            switch(weaponStatistic->Type)
            {
            case Health:
                health += weaponStatistic->Value;
                break;
            case Damage:
                damage += weaponStatistic->Value;
                break;
            case Armor:
                armor += weaponStatistic->Value;
                break;
            }                
        }
    }

    if (player->Inventory->Armor != nullptr)
    {
        for (const auto armorStatistic : player->Inventory->Armor->Statistics)
        {
            switch(armorStatistic->Type)
            {
            case Health:
                health += armorStatistic->Value;
                break;
            case Damage:
                damage += armorStatistic->Value;
                break;
            case Armor:
                armor += armorStatistic->Value;
                break;
            }                
        }
    }
            
    ClearRow(_statusPosition, {_statusLength, 0});
    SetConsoleCursorPosition(ConsoleScreenHandle, _statusPosition);
    std::cout << "Health: " << health << " | Damage: " << damage << " | Armor: " << armor;
}

void View::DrawActionText(std::string newActionText)
{
    AppendToActionText(newActionText);
        
    const COORD actionTextPosition { static_cast<short>(Dimensions.X - _actionText.size()), static_cast<short>(Dimensions.Y) };
    SetConsoleCursorPosition(ConsoleScreenHandle, actionTextPosition);
    std::cout << _actionText;
}

void View::Clear()
{
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

void View::ClearRow(COORD rowStart, COORD rowEnd)
{
    if (rowEnd.X == -1)
        rowEnd.X = ConsoleScreenInfo.dwSize.X;
        
    DWORD written;
    GetConsoleScreenBufferInfo(ConsoleScreenHandle, &ConsoleScreenInfo);
    FillConsoleOutputCharacterA(
        ConsoleScreenHandle, ' ',
        ConsoleScreenInfo.dwSize.X - (ConsoleScreenInfo.dwSize.X - rowEnd.X), rowStart, &written
    );
    FillConsoleOutputAttribute(
        ConsoleScreenHandle, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE,
        ConsoleScreenInfo.dwSize.X - (ConsoleScreenInfo.dwSize.X - rowEnd.X), rowStart, &written
    );
}

void View::AppendToActionText(std::string newActionText)
{
    _actionText.append(" " + newActionText);

    if (_actionText.size() > _actionTextLength)
    {
        _actionText = _actionText.substr(_actionText.size() - _actionTextLength, _actionText.size());
        _actionText[0] = _actionText[1] = _actionText[2] = '.';
    }
}
