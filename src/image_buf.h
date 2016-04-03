#ifndef image_buf_h
#define image_buf_h

typedef struct {
int image[1080][1920]; /*buffer to store image*/
int horizontalCounter;  /* the current index of image column to write*/
} image_buf;


#endif
