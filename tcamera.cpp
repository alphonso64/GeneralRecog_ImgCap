#include "tcamera.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include <malloc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>
#include "sdf.h"

//Tcamera::Tcamera()
//{
//    cam = new TcameraInfo();
//}

#define VIDEOPATH "/dev/video0"
#define	FPS		4

#define CLEAR(x) memset(&(x), 0, sizeof(x))


Tcamera::Tcamera(int previewWidth, int previewHeight)
{
    cam = new TcameraInfo();
    cam->width = previewWidth;
    cam->height = previewHeight;
    initFlag = false;
    strcpy(cam->device_name,VIDEOPATH);
}

Tcamera::~Tcamera()
{
    delete cam;
}

//open camera
int Tcamera::open_camera()
{
    struct stat st;

    //query the file is exist
    if (-1 == stat(cam->device_name, &st))
    {
        printf("Cann't identify %s\n", cam->device_name);
        return BASICERROR;
    }

    //test the file is a character device
    if (!S_ISCHR(st.st_mode))
    {
        printf("%s is not a device file\n", cam->device_name);
        return BASICERROR;
    }

    //open the camera device
    cam->camera_fd = open(cam->device_name, O_RDWR, 0);

    //test whether open successfully or not
    if (-1 == cam->camera_fd)
    {
        printf("can't open %s\n", cam->device_name);
        return BASICERROR;
    }
    return SUCCESS;
}


//close camera
int  Tcamera::close_camera()
{
    //close the camera device and test the result
    if (-1 == close(cam->camera_fd))
    {
        printf("close device failure\n");
        return BASICERROR;
    }
    return SUCCESS;
}


//initialize the camera device
int Tcamera::init_camera()
{
    struct v4l2_capability 	*cap = &(cam->v4l2_cap);
    struct v4l2_format 		*fmt = &(cam->v4l2_fmt);
//    struct v4l2_crop 		*crop = &(cam->crop);
    unsigned int min;

    //get the video device capability
    if (-1 == ioctl(cam->camera_fd, VIDIOC_QUERYCAP, cap))
    {
        if (EINVAL == errno)
        {
            printf("%s is not a v4l2 device\n", cam->device_name);
            return BASICERROR;
        }
        else
        {
            printf("query cap failure\n");
            return BASICERROR;
        }
    }

    //judge the device whether is a video capture device or not
    if (!(cap->capabilities & V4L2_CAP_VIDEO_CAPTURE))
    {
        printf("%s is not a video capture device\n", cam->device_name);
        return BASICERROR;
    }

    //test the device support video streaming or not
    if (!(cap->capabilities & V4L2_CAP_STREAMING))
    {
        printf("%s is not support streaming\n", cam->device_name);
        return BASICERROR;
    }

    CLEAR(*fmt);

    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    struct v4l2_fmtdesc fmt__;
    struct v4l2_frmsizeenum frmsize;
    fmt__.index = 0;
    fmt__.type = type;
    while (ioctl(cam->camera_fd, VIDIOC_ENUM_FMT, &fmt__) >= 0)
    {
        frmsize.pixel_format = fmt__.pixelformat;
        frmsize.index = 0;
        printf("pixel_format %d\n",frmsize.pixel_format);
        while (ioctl(cam->camera_fd, VIDIOC_ENUM_FRAMESIZES, &frmsize) >= 0)
        {
            if (frmsize.type == V4L2_FRMSIZE_TYPE_DISCRETE)
            {
                printf("V4L2_FRMSIZE_TYPE_DISCRETE %dx%d\n",
                    frmsize.discrete.width,
                    frmsize.discrete.height);
            } else if (frmsize.type == V4L2_FRMSIZE_TYPE_CONTINUOUS)
            {
                printf("V4L2_FRMSIZE_TYPE_STEPWISE %dx%d\n",
                    frmsize.stepwise.max_width,
                    frmsize.stepwise.max_height);
            }
            frmsize.index++;
        }
        fmt__.index++;
    }

    fmt->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt->fmt.pix.width = cam->width;
    fmt->fmt.pix.height = cam->height;
    fmt->fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    fmt->fmt.pix.field = V4L2_FIELD_NONE;
    //set the video format
    if (-1 == ioctl(cam->camera_fd, VIDIOC_S_FMT, fmt))
    {
        printf("can't set video format\n");
        return BASICERROR;
    }
    //set image size
    cam->image_size = fmt->fmt.pix.width * fmt->fmt.pix.height * 2;

    min = fmt->fmt.pix.width * 2;
    if (fmt->fmt.pix.bytesperline < min)
    {
        fmt->fmt.pix.bytesperline = min;
    }

    min = fmt->fmt.pix.bytesperline * fmt->fmt.pix.height;
    if(fmt->fmt.pix.sizeimage < min)
    {
        fmt->fmt.pix.sizeimage = min;
    }

    cam->bytesperrow = fmt->fmt.pix.bytesperline;

    //initialize the memory map from kernel space to user space
    if(SUCCESS!=init_mmap())
        return BASICERROR;

    return SUCCESS;
}


//map the driver memory from kernel space to user space
int Tcamera::init_mmap()
{
    struct v4l2_requestbuffers req;
    unsigned int 	i = 0;

    CLEAR(req);

    req.count = FPS;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;

    //get capture memory
    if (-1 == ioctl(cam->camera_fd, VIDIOC_REQBUFS, &req))
    {
        if (EINVAL == errno)
        {
            printf("%s does not support memory map\n", cam->device_name);
            return BASICERROR;
        }
        else
        {
            printf("get memory failure!\n");
            return BASICERROR;
        }
    }

    if (req.count < 2)
    {
        printf("can't get enough memory\n");
        return BASICERROR;
    }

    cam->buffers = (buffer *)calloc(req.count, sizeof(*(cam->buffers)));

    if (cam->buffers == NULL)
    {
        printf("out of memory!\n");
        return BASICERROR;
    }

    for (i = 0; i < req.count; ++i)
    {
        struct v4l2_buffer buf;

        CLEAR(buf);

        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;

        if (-1 == ioctl(cam->camera_fd, VIDIOC_QUERYBUF, &buf))
        {
            printf("can't get info of video query buffer\n");
            return BASICERROR;
        }

        cam->buffers[i].length = buf.length;
        cam->buffers[i].start = mmap(NULL,
                buf.length, PROT_READ|PROT_WRITE,
                MAP_SHARED, cam->camera_fd, buf.m.offset);

        //test memory map is useful
        if (MAP_FAILED == cam->buffers[i].start)
        {
            printf("map memory failure\n");
            return BASICERROR;
        }
    }
    return SUCCESS;
}


//free the camera
int Tcamera::uninit_camera()
{
    int i = 0;

    for (i = 0; i < FPS; ++i)
    {
        if (-1 == munmap(cam->buffers[i].start, cam->buffers[i].length))
        {
            printf("can't free the mapped memory\n");
            return BASICERROR;
        }
    }

    free(cam->buffers);
    cam->buffers = NULL;
    return SUCCESS;
}


//set and get video capture FPS
int Tcamera::set_capture_fps(int *fps)
{
    struct v4l2_streamparm setfps={0};

    setfps.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    setfps.parm.capture.timeperframe.numerator = 1;
    setfps.parm.capture.timeperframe.denominator = *fps;

    if (-1 == ioctl(cam->camera_fd, VIDIOC_S_PARM, &setfps))
    {
        printf("set video param error in set_capture_fps function\n");
        return BASICERROR;
    }

    *fps = (int)setfps.parm.capture.timeperframe.denominator;

    printf("now the video FPS is %d\n", *fps);
    return SUCCESS;
}


//start capture the picture from camera
int Tcamera::start_capturing()
{
    int i = 0;
    enum v4l2_buf_type type;

    for (i = 0; i < FPS; ++i)
    {
        struct v4l2_buffer buf;

        CLEAR(buf);

        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;

        if (-1 == ioctl(cam->camera_fd, VIDIOC_QBUF, &buf))
        {
            printf("video query buffer failure\n");
            return BASICERROR;
        }
    }

    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    //start capture the video
    if (-1 == ioctl(cam->camera_fd, VIDIOC_STREAMON, &type))
    {
        printf("video stream open failure\n");
        return BASICERROR;
    }
    return SUCCESS;
}


//stop capture the picture
int Tcamera::stop_capturing()
{
    enum v4l2_buf_type type;

    type = V4L2_BUF_TYPE_VBI_CAPTURE;

    //stop capturing picture
    if (-1 == ioctl(cam->camera_fd, VIDIOC_STREAMOFF, &type))
    {
        printf("stop capture the video device\n");
        return BASICERROR;
    }
    return SUCCESS;
}

//initialize video driver for linux system
int Tcamera::v4l2_init()
{
    if(SUCCESS!=open_camera())
    {
        return BASICERROR;
    }
    if(SUCCESS!=init_camera())
    {
        return BASICERROR;
    }
    if(SUCCESS!=start_capturing())
    {
        return BASICERROR;
    }
    initFlag = true;
    return SUCCESS;
}


//close the video driver for linux system
int Tcamera::v4l2_close()
{
    if(SUCCESS!=stop_capturing())
    {
        return BASICERROR;
    }
    if(SUCCESS!=uninit_camera())
    {
        return BASICERROR;
    }
    if(SUCCESS!=close_camera())
    {
        return BASICERROR;
    }
    initFlag = false;
    return SUCCESS;
}

//read a frame from the opened camera device
int Tcamera::read_frame_from_camera(uint8 *frame_buf, int *frame_length)
{
    struct v4l2_buffer buf;
    CLEAR(buf);

    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;

    if (-1 == ioctl(cam->camera_fd, VIDIOC_DQBUF, &buf))
    {
        switch(errno)
        {
        case EAGAIN:
            return 0;
        case EIO:
        default:
            printf("read frame from camera wrong\n");
            return BASICERROR;
        }
    }

    memcpy(frame_buf, cam->buffers[buf.index].start, cam->buffers[buf.index].length);
    *frame_length = cam->buffers[buf.index].length;

    if (-1 == ioctl(cam->camera_fd, VIDIOC_QBUF, &buf))
    {
        printf("read frame frame camera put buf into the frame queue error");
        return BASICERROR;
    }

    return SUCCESS;
}


