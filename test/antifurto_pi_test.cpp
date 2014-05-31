#include "Picture.hpp"
#include "PictureArchive.hpp"
#include "PiCamera.hpp"

#include <thread>


using namespace antifurto;

void pictureArchiveTest()
{
    PictureArchive archive("/tmp", 3);
    PiCamera camera;
    Picture p;
    for (int i = 0; i < 5; ++i)
    {
        camera.takePicture(p);
        archive.addPicture(p);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    archive.startSaving();
    camera.takePicture(p);
    archive.addPicture(p);
    archive.stopSaving();
}

int main()
{
    pictureArchiveTest();
    return 0;
}
