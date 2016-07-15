#ifndef TCAMERA_H
#define TCAMERA_H
#include "sdf.h"

struct buffer
{
    void	*start;
    size_t	length;
};

class TcameraInfo
{
 public:
    char	device_name[20];
    int		camera_fd;
    int		width;
    int		height;
    int 	image_size;
    int     bytesperrow;
    int 	frame_number;
    struct	v4l2_capability v4l2_cap;
    struct 	v4l2_format		v4l2_fmt;
    struct	v4l2_crop 		crop;
    struct	buffer			*buffers;
};

class Tcamera
{
public:
//    Tcamera();
    Tcamera(int previewWidth,int previewHeight);
    ~Tcamera();
    int v4l2_init();
    int v4l2_close();
    int read_frame_from_camera(uint8 *frame_buf, int *length);

    bool initFlag;

private:
    int open_camera();
    int close_camera();
    int start_capturing( );
    int stop_capturing();
    int init_camera();
    int uninit_camera();
    int init_mmap();
    int set_capture_fps(int *fps);

private:
    TcameraInfo *cam;
};

#endif // TCAMERA_H
