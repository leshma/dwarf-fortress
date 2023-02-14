#include <boost/signals2.hpp>
#include <boost/bind.hpp>

#include "Controllers/KeyboardController.h"
#include "Externals/json/json.h"
#include "Models/Environment.h"
#include "Utilities/LevelManager.h"
#include "Views/View.h"

using namespace boost;
using namespace std;

int main()
{
    // Set the randomizer
    srand(static_cast<unsigned int>(time(nullptr)));

    string levelFilePath = "Data/Level1.json";
    WORD lastInput;
    do
    {
        const std::unique_ptr<LevelManager> levelManager(new LevelManager(levelFilePath));
        const std::shared_ptr<Environment> environment(new Environment(Coordinates {119, 29}, levelManager->LoadedObjects, levelManager->LoadedRooms));
        const std::shared_ptr<View> view(new View(environment));
        const std::unique_ptr<KeyboardController> keyboardController(new KeyboardController());

        view->InitialDraw();

        environment->SignalEnvironmentChanged.connect(bind(&View::DrawEnvironmentChanges, view, _1));
        environment->SignalStatusChanged.connect(bind(&View::DrawStatusChanges, view));
        environment->SignalActionTextChanged.connect(bind(&View::DrawActionText, view, _1));
        
        keyboardController->SignalKeyboardPress.connect(bind(&Environment::MoveTick, environment, _1));
        keyboardController->SignalActionTextChanged.connect(bind(&View::DrawActionText, view, _1));

        do
        {
            lastInput = keyboardController->ListenToInputs();

            // Save the game state
            if (lastInput == VK_F1)
            {
                levelManager->SaveFile(LevelManager::QuickSaveFilePath, environment);
            }
        }
        while (lastInput != VK_F2 && lastInput != VK_ESCAPE);

        // Load the game state by setting the path towards the saved file
        if (lastInput == VK_F2)
        {
            levelFilePath = LevelManager::QuickSaveFilePath;
        }
    }
    while (lastInput != VK_ESCAPE);

    return 0;
}
