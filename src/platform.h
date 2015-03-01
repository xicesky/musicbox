/***************************************************************************************************
    Our "platform" library.
    Currently uses SDL for everything.
***************************************************************************************************/

#ifndef PLATFORM_H
#define PLATFORM_H

namespace MusicBox {

class Platform_Private;

class Platform {

protected:
    Platform_Private* p;

public:
    Platform();
    virtual ~Platform();

    void run();

};

}

#endif
