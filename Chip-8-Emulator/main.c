
#include <main.h>

int main()
{
    initAudio();
    //startAudio();
    
    cpuLoop();

    /* todo later close everything */
    closeAudio();
    return 0;
}