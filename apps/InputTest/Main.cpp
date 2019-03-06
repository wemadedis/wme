#include "PlayerController.h"
int main()
{
    PlayerController* pc = new PlayerController();
    pc->Init();

    int runLoops = 10;

    for(int runIndex = 1; runIndex <= runLoops; runIndex++)
    {
        pc->OnUpdate(runIndex);
    }
    
    return 0;
}
