#ifndef _H_AGK_TEMPLATE_
#define _H_AGK_TEMPLATE_

#include "AGK.h"

#define DEVICE_WIDTH 1300
#define DEVICE_HEIGHT 800
#define DEVICE_POS_X 32
#define DEVICE_POS_Y 32
#define FULLSCREEN false

// used to make a more unique folder for the write path
#define COMPANY_NAME "My Company"










class app
{
public:

	app() { memset ( this, 0, sizeof(app)); }

	void Begin( void );
	int Loop( void );
	void End( void );

};

extern app App;

















#endif

