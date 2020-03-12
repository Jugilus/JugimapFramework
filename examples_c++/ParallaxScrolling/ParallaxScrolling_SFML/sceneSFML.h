#ifndef API_DEMO_TEST__SFML_H
#define API_DEMO_TEST__SFML_H


#include "jugimapSFML/jmSFML.h"
#include "engineIndependent/parallaxScene.h".h"



class ParallaxSceneSFML : public ParallaxScene
{
public:

    bool Init() override;
    void Draw() override;

};






#endif
