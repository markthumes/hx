#include "x11wrapper.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>


X11::Point::Point(short _x, short _y){
	x = _x;
	y = _y;
}
X11::Point::Point(const X11::Point &p){
	x = p.x;
	y = p.y;
}

X11::Point X11::Point::random( short xMax, short yMax ){
	struct timespec t;
	clock_gettime( CLOCK_REALTIME, &t );
	srand( t.tv_nsec );
	xMax = rand() % xMax;
	yMax = rand() % yMax;
	return X11::Point(xMax,yMax);
}

X11::Line::Line( X11::Point _a, X11::Point _b ){
	a = _a;
	b = _b;
}

X11::Line::Line( const X11::Line& l ){
	a = l.a;
	b = l.b;
}

Display* X11::Color::m_display;
X11::Color::Color( float r, float g, float b ){
	if(!m_display) m_display = XOpenDisplay(NULL);
	Colormap cmap = DefaultColormap( m_display, 0 );
	char hex[8];
	snprintf( hex, 8, "#%02x%02x%02x",
			(uint8_t)(r * UCHAR_MAX),
			(uint8_t)(g * UCHAR_MAX),
			(uint8_t)(b * UCHAR_MAX) );
	XParseColor( m_display, cmap, hex, &m_color );
	XAllocColor( m_display, cmap,      &m_color );
	XFreeColors( m_display, cmap, NULL, 0, 0 );
}

X11::Color::Color(const X11::Color &c){
	m_color = c.m_color;
}

X11::X11( const char* name, int w, int h):m_windowName(name){
	m_display = XOpenDisplay(NULL);
	m_attrib.width  = w;
	m_attrib.height = h;
}
void X11::open(){
	XSetWindowAttributes swa;
	swa.event_mask = ExposureMask | KeyPressMask;
	Window root = DefaultRootWindow(m_display);
	m_window = XCreateWindow( m_display, root, 0, 0, 
			m_attrib.width, m_attrib.height, 0, 
			CopyFromParent, InputOutput, CopyFromParent,
			CWEventMask, &swa);
	XMapWindow(m_display, m_window);
	setWindowName(m_windowName);
	m_open = true;
}

void X11::close(){
	if(m_open){
		m_open = false;
		if( m_doubleBuffered ){
			XdbeDeallocateBackBufferName( m_display, m_backBuffer );
		}
		XDestroyWindow( m_display, m_window );
		XCloseDisplay( m_display );
	}
}

void X11::getAttributes(){
	//TODO: Figure out how to make sure attributes are the same between window buffers
	XGetWindowAttributes(m_display, m_window, &m_attrib);
}

void X11::setWindowName( const char* name ){
	m_windowName = name;
	XStoreName( m_display, m_window, name );
}

void X11::setBackground( X11::Color c ){
	XSetWindowBackground( m_display, m_window, c.getColor().pixel );
}

void X11::drawLine ( Color c, X11::Line  l, int width ){
	drawLines( c, &l, 1, width );
}
void X11::drawLines( Color c, X11::Line *l, int count, int width ){
	XGCValues values;
	values.foreground = c.getColor().pixel;
	values.line_width = width;
	GC gc = XCreateGC( m_display, m_window,
			GCForeground | GCLineWidth,
			&values );

	Drawable buffer = m_window;
	if( m_doubleBuffered ) buffer = m_backBuffer;
	XDrawLines( m_display, buffer, gc, (XPoint*)l, count*2, CoordModeOrigin );
	XFlush( m_display );
	XFreeGC( m_display, gc );
	//TODO: keep track of gc in member variable. Modify it here rather than allocate
}

void X11::drawPoint ( Color c, X11::Point  p,            int size ){
	drawPoints( c, &p, 1, size );
}
void X11::drawPoints( Color c, X11::Point* p, int count, int size ){
	XGCValues values;
	values.foreground = c.getColor().pixel;
	GC gc = XCreateGC( m_display, m_window,
			GCForeground,
			&values );
	Drawable buffer = m_window;
	if( m_doubleBuffered ) buffer = m_backBuffer;
	if( size == 1 ){
		XDrawPoints( m_display, buffer, gc, (XPoint*)p, count, CoordModeOrigin );
	}
	else{
		XRectangle *rect = (XRectangle*)malloc(count*sizeof(XRectangle));
		for( int i = 0; i < size; i++ ){
			rect[i].x = p->x - size / 2;
			rect[i].y = p->y - size / 2;
			rect[i].width  = size;
			rect[i].height = size;
		}
		XFillRectangles( m_display, buffer, gc, rect, count);
		free(rect);
	}
	XFlush( m_display );
	XFreeGC( m_display, gc );
}

void X11::refresh(){
	if( m_doubleBuffered ){
		swapBuffers();
	}
	else{
		XClearArea( m_display, m_window, 0,0,0,0, false );
	}
}

void X11::swapBuffers(){
	XdbeSwapInfo i;
	i.swap_window = m_window;
	i.swap_action = XdbeBackground;
	XdbeSwapBuffers( m_display, &i, 1 );
}

void X11::setDoubleBuffered( bool mode ){
	m_doubleBuffered = true;
	int maj, min;
	XdbeQueryExtension( m_display, &maj, &min );
	m_backBuffer = XdbeAllocateBackBufferName( m_display, m_window, XdbeUndefined );
}

