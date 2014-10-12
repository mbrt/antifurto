#pragma once

enum picam_error_code_t
{
    PICAM_NO_ERROR = 0,
    PICAM_ALREADY_CREATED = 1,
    PICAM_CAM_COMP_FAILED = 2,
    PICAM_NO_OUTPUT_PORTS = 3,
    PICAM_CANNOT_ENABLE_PORT = 4,
    PICAM_CANNOT_SET_PREVIEW_PORT = 5,
    PICAM_CANNOT_SET_VIDEO_PORT = 6,
    PICAM_CANNOT_SET_STILL_PORT = 7,
    PICAM_CANNOT_ENABLE_CAMERA = 8,
    PICAM_FAILED_SPLITTER_CREATE = 9,
    PICAM_WRONG_SPLITTER_PORTS = 10,
    PICAM_CANNOT_SET_RESIZER_PORT_FORMAT = 11,
    PICAM_FAILED_CREATE_CONNECTION = 12,
    PICAM_FAILED_ENABLE_CONNECTION = 13,
    PICAM_FAILED_INITIALIZE_OUTPUT = 14,
    PICAM_FAILED_START_CAPTURE = 15,
    PICAM_FAILED_CREATE_OUTPUT_QUEUE = 16,
    PICAM_FAILED_RETURN_BUFFER_TO_PORT = 17,
    PICAM_FAILED_CREATE_RESIZE_COMP = 18,
    PICAM_WRONG_RESIZER_PORTS = 19,
    PICAM_CANNOT_CREATE_VIDEO_BUFFER_POOL = 20,
    PICAM_FAILED_SET_VIDEO_BUFFER_CB = 21,
    PICAM_CANNOT_GET_REQUIRED_BUFFER = 22,
    PICAM_UNABLE_SEND_BUFFER_TO_PORT = 23
};

// fwd reference
class CCamera;

CCamera* picam_start_camera(int width, int height, int framerate, int num_levels, bool do_argb_conversion);
void picam_stop_camera(CCamera* camera);

int picam_begin_read_frame(CCamera* camera, int level, const void* &out_buffer, int& out_buffer_size);
int picam_end_read_frame(CCamera* camera, int level);

picam_error_code_t picam_get_last_error();
