
#include <main.h>

int main()
{
    initAudio();
    //startAudio();
    
    p_libsys_init();
    
    cpuLoop();

    /* todo later close everything */
    closeAudio();
    return 0;
}