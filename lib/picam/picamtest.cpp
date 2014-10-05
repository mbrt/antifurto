#include <stdio.h>
#include <unistd.h>
#include "camera.h"

#define MAIN_TEXTURE_WIDTH 512
#define MAIN_TEXTURE_HEIGHT 512

char tmpbuff[MAIN_TEXTURE_WIDTH*MAIN_TEXTURE_HEIGHT*4];

//entry point
void test()
{
	//should the camera convert frame data from yuv to argb automatically?
	bool do_argb_conversion = false;

	//how many detail levels (1 = just the capture res, > 1 goes down by half each level, 4 max)
	int num_levels = 4;

	//init graphics and the camera
	CCamera* cam = StartCamera(MAIN_TEXTURE_WIDTH, MAIN_TEXTURE_HEIGHT,30,num_levels,do_argb_conversion);

	printf("Running frame loop\n");
	for(int i = 0; i < 3000; i++)
	{
		//pick a level to read based on current frame (flicking through them every 30 frames)
		int texidx = (i / 30)%num_levels;

		//lock the chosen frame buffer, and copy it directly into the corresponding open gl texture
		const void* frame_data; int frame_sz;
		if(cam->BeginReadFrame(texidx,frame_data,frame_sz))
		{
			if(do_argb_conversion)
			{
				//if doing argb conversion the frame data will be exactly the right size so just set directly
				//textures[texidx].SetPixels(frame_data);
			}
			else
			{
				//if not converting argb the data will be the wrong size and look weird, put copy it in
				//via a temporary buffer just so we can observe something happening!
				memcpy(tmpbuff,frame_data,frame_sz);
				//textures[texidx].SetPixels(tmpbuff);
			}
			cam->EndReadFrame(texidx);
		}
	}

	printf("Stopping camera...\n");
	StopCamera();
	printf("Stopped\n");
}

int main(int argc, const char **argv)
{
    printf("First test\n");
    test();
    sleep(5);
    printf("Second test\n");
    test();
    return 0;
}
