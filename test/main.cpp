#include "x11wrapper.h"
#include <math.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>

#define UNUSED __attribute__((unused))

#define MAXPOINTS 2000

int random( int min, int max ){
	struct timespec t;
	clock_gettime( CLOCK_REALTIME, &t );
	srand( t.tv_nsec );
	return min + rand() % (max - min);
}

void drawPlotAxis(X11 *x, float xPadding, float yPadding){
	
}

class XPlot : public X11{

};

int main( UNUSED int argc, UNUSED char** argv ){
	
	float frate = 1.0/120.0;

	X11 x("New Window", 1000, 1000);
	//x.setDebug(false);
	//x.setWarnings(false);
	//x.autoScale();
	x.open();
	x.setBackground( X11::Color(0,0,0) );
	x.setDoubleBuffered();


	X11::Color colors[7];
	colors[0] = X11::Color(0,0,1);
	colors[1] = X11::Color(0,1,0);
	colors[2] = X11::Color(0,1,1);
	colors[3] = X11::Color(1,0,0);
	colors[4] = X11::Color(1,0,1);
	colors[5] = X11::Color(1,1,0);
	colors[6] = X11::Color(1,1,1);
	float timer = 0.0;
	X11::Point points[7][MAXPOINTS];
	X11::Point meter[MAXPOINTS];
	for( int i = 0; i < 6; i++ ){
		for( int j = 0; j < MAXPOINTS; j++ ){
			points[i][j] = X11::Point(0,0);
		}
	}
	int frame = 0;
	bool open = true;
	while(open){
		while( XPending(x.getDisplay()) ){
			XEvent xev;
			XNextEvent( x.getDisplay(), &xev );
			if( xev.type == Expose ){
				x.getAttributes();
				x.refresh();
			}
			if( xev.type == KeyPress ){
				open = false;
			}
		}

		int len[6];
		len[0] = 100;
		len[1] =  50;
		len[2] = 100;
		len[3] =  50;
		len[4] = 100;
		len[5] =  50;
		X11::Point seg[7];
		seg[0] = x.getCenter(); seg[0].x -= x.getWidth()/4;
		seg[1] = X11::Point(seg[0].x + sinf(timer*1.0)*len[0],seg[0].y + cosf(timer*1.0)*len[0] );
		seg[2] = X11::Point(seg[1].x + sinf(timer*2.0)*len[1],seg[1].y + cosf(timer*2.0)*len[1] );
		seg[3] = X11::Point(seg[2].x + sinf(timer*3.0)*len[2],seg[2].y + cosf(timer*3.0)*len[2] );
		seg[4] = X11::Point(seg[3].x + sinf(timer*4.0)*len[3],seg[3].y + cosf(timer*4.0)*len[3] );
		seg[5] = X11::Point(seg[4].x + sinf(timer*5.0)*len[4],seg[4].y + cosf(timer*5.0)*len[4] );
		seg[6] = X11::Point(seg[5].x + sinf(timer*6.0)*len[5],seg[5].y + cosf(timer*6.0)*len[5] );
		X11::Color rainbow((1+sinf(timer))/2,(1+cosf(timer))/2,(1+sinf(timer/2))/2);

		X11::Point adj = seg[6];
		adj.x += 500;
		adj.x = x.getCenter().x;
		X11::Line som( seg[6], adj );


		for( int i = 0; i < 6; i++ ){
			points[i][frame%MAXPOINTS] = seg[i+1];
			x.drawLine  ( colors[i], X11::Line( seg[i], seg[i+1] ), 2 );
		//	x.drawPoints( colors[i], points[i], MAXPOINTS );
		}
		meter[frame%MAXPOINTS] = adj;
		x.drawLine( colors[6], som );
		x.drawPoints( colors[6], meter, MAXPOINTS );
		for( int i = 0; i < MAXPOINTS; i++ ){
			meter[i].x++;
		}

		timer += 0.005;
		XFlush(x.getDisplay());
		usleep(frate * 1000 * 1000);
		x.refresh();
		frame++;
	}
	x.close();

}

#undef UNUSED

