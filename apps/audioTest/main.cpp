
#include "rte/AudioComponent.hpp"
#include "rte/TransformComponent.hpp"

#include "rte/Emitter.hpp"
#include "glm/glm.hpp"


int main()
{
    const std::string& path = std::string(ENGINE_ASSET_DIR) + std::string("audio\\mono.wav");
    
    // Emitter TEST

    // RTE::Audio::Emitter emitter;
    // emitter.LoadSoundFromFile(path);
    // emitter.Play();
    // while(emitter.GetState() == RTE::Audio::State::Playing){}


    // AudioComponentTest

    using namespace RTE::StdComponents;
    AudioComponent ac;
    TransformComponent tc;
    tc.Initialize(glm::vec3(0.f), glm::vec3(0.f), glm::vec3(0.f));
    ac.Initialize(path, &tc);
    ac.Play();
    while(ac.GetState() == RTE::Audio::State::Playing){}
    
    return 0;
}