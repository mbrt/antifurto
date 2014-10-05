#pragma once

// fwd reference
class CCamera;

CCamera* StartCamera(int width, int height, int framerate, int num_levels, bool do_argb_conversion);
void StopCamera(CCamera* camera);

bool BeginReadFrame(CCamera* camera, int level, const void* &out_buffer, int& out_buffer_size);
void EndReadFrame(CCamera* camera, int level);
