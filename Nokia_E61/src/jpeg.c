/*
 *  jpeg.c
 *
 *  Author: Kestutis Bivainis
 *
 */

#include <stdint.h>
#include <string.h>
#include "tjpgd.h"
#include "MC2PA8201.h"
#include "jpeg.h"

uint8_t* dataptr;
extern COLOR_MODE current_color_mode;

__align(4) char Work[4*1024];	/* Minimum working buffer for the most JPEG picture */

static uint32_t tjd_input (
	JDEC* jd,		    /* Decompression object */
	uint8_t* buff,	/* Pointer to the read buffer (NULL:skip) */
	uint32_t nd			/* Number of bytes to read/skip from input stream */
)
{
	if (buff) {
	  memcpy(buff,dataptr,nd);
	}
	dataptr+=nd;
	return nd;
}

/* User defined call-back function to output RGB bitmap */
static uint32_t tjd_output (
	JDEC* jd,		/* Decoder object */
	void* bitmap,	/* Bitmap data to be output */
	JRECT* rect		/* Rectangular region to output */
)
{
	uint8_t* buf=(uint8_t*)bitmap;

	jd = jd;	/* Suppress warning (device identifier is not needed) */
	
	MC2PA8201_FillFromBuffer(rect->left,rect->top,rect->right,rect->bottom,buf);

	return 1;	/* Continue decompression */
}

void load_jpg (void) {
	
	JDEC jd;		/* Decompression object (70 bytes) */
	JRESULT rc;

	uint8_t tjpegd_format;
	
	switch(current_color_mode) {
		case COLOR_12BIT:tjpegd_format=3;break;
		case COLOR_16BIT:tjpegd_format=2;break;
		case COLOR_18BIT:tjpegd_format=1;break;
		case COLOR_24BIT:tjpegd_format=0;break;
		default:
			return;
	}
	
	/* Prepare to decompress the file */
	rc = jd_prepare(&jd, tjd_input, Work, sizeof(Work), 0);
	
	if (rc == JDR_OK) {
  	/* Start to decompress the JPEG file */
    rc = jd_decomp(&jd, tjd_output, 0, tjpegd_format);
	}
}
