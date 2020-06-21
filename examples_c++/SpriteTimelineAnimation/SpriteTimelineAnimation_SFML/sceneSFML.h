#ifndef API_DEMO_TEST__SFML_H
#define API_DEMO_TEST__SFML_H


#include "jugimapSFML/jmSFML.h"
#include "engineIndependent/scene.h"



class DemoSceneSFML : public DemoScene
{
public:

    bool Init() override;
    void Draw() override;

};






#endif
