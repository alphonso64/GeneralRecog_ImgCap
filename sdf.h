#ifndef SDF_H
#define SDF_H

#include <linux/videodev2.h>
#include <string.h>
#include "stdint.h"
#include <time.h>
typedef unsigned char		uint8;
typedef unsigned short int 	uint16;
typedef	unsigned int 		uint32;
typedef unsigned long long	uint64;
typedef char				int8;
typedef short int			int16;
typedef int					int32;
typedef long long			int64;

//define the function execute status
#define BASICERROR			1
#define SUCCESS				0
#define PARAMERROR			BASICERROR<<1
#define MEMORYERROR			BASICERROR<<2
#define FILEERROR			BASICERROR<<3

#define REC_ZONE_WIDTH 80
#define REC_ZONE_HEIGHT 80
#define RECTANGLE_MARKER_WIDTH 240
#define RECTANGLE_MARKER_HEIGHT 240
#define RECTANGLE_MARKER_START_X (FRAME_WIDTH-RECTANGLE_MARKER_WIDTH)/2
#define RECTANGLE_MARKER_START_Y (FRAME_HEIGHT-RECTANGLE_MARKER_HEIGHT)/2

#define IMAGESACEPATH "/home/program/IMG/%d__%d.jpg"

#define IMAGESACEPATH_FULL "/home/program/IMG/*"

struct imageRecParam
{
    int isDisplay;
    int x;
    int y;
    int recWidth;
    int recHeight;
    int step;
};

#endif // SDF_H
