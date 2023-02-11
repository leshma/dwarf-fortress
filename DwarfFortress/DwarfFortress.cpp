#include <boost/signals2.hpp>
#include <boost/bind.hpp>

#include "Controllers/KeyboardController.h"
#include "Externals/json/json.h"
#include "Models/Environment.h"
#include "Utilities/LevelLoader.h"
#include "Views/View.h"

using namespace boost;
using namespace std;

int main()
{
    LevelLoader levelLoader("Data/Level1.json");
    std::shared_ptr<Environment> environment = std::make_shared<Environment>(Coordinates {119, 29}, levelLoader.LoadedObjects, levelLoader.LoadedRooms);
    View view(environment);
    KeyboardController keyboardController;

    view.Draw();
    environment->SignalEnvironmentChanged.connect(bind(&View::DrawChanges, &view, _1));
    keyboardController.SignalKeyboardPress.connect(bind(&Environment::MoveTick, environment, _1));
    keyboardController.ListenToInputs();
    
    return 0;
}
