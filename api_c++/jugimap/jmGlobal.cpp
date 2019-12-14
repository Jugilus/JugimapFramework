#include <string>
#include <sstream>
#include "jmObjectFactory.h"
#include "jmSceneManager.h"
#include "jmGlobal.h"



namespace jugimap {


Settings settings;
Time time;


void DeleteGlobalObjects()
{
    if(objectFactory) delete objectFactory;
    if(sceneManager) delete sceneManager;

}


}
