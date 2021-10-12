#include <X11/Xlib.h>
#include <X11/extensions/Xdbe.h>
#include <inttypes.h>
#include <limits.h>
#include <unistd.h>



class X11{
public:
	class Point : public XPoint {
	public:
		Point(){};
		Point(short _x, short _y);
		Point(const Point &p);
		Point random( short xMax = SHRT_MAX, short yMax = SHRT_MAX );
	};
	class Line{
	public:
		Line(){};
		Line( Point _a, Point _b );
		Line( const Line& l );
		Point a, b;
	};
	class Color{
	public:
		Color(){};
		Color(float r, float g, float b);
		Color(const Color &c );
		XColor getColor(){ return m_color; }
		void setReferenceDisplay( Display* d){ m_display = d; }
	private:
		static Display* m_display;
		XColor   m_color;
	};
public:
	X11( const char* name = "New Window", int w = 100, int h = 100 );
	void open();
	void close();

	int      getWidth()  { return m_attrib.width;  }
	int      getHeight() { return m_attrib.height; }
	Window   getWindow() { return m_window;        }
	Display* getDisplay(){ return m_display;       }
	bool     isOpen()    { return m_open;          }
	Point    getCenter() { return Point(m_attrib.width/2, m_attrib.height/2); }

	//**************** Drawing and Updating ****************//
	void drawLines ( Color c, Line*  l, int count = 1, int width = 1 );
	void drawLine  ( Color c, Line   l,                int width = 1 );
	void drawPoints( Color c, Point* p, int count = 1, int size  = 1 );
	void drawPoint ( Color c, Point  p,                int size  = 1 );
	void refresh( float timeSeconds = 0.0 );

	void setWindowName( const char* name );
	void setBackground( Color c );
	void setDoubleBuffered( bool mode = true );

	/* Swap using DBE (Double Buffer Extension) [front, back]
	 * 
	 *
	 */
	XID buffer[2]; //XID buffer name,
	void swapBuffers();
	void getAttributes();
protected:
	void drawToBuffer();
	void readFromBuffer();
//private:
	uint8_t m_bufferCounter;
	const char*    m_windowName;
	bool m_open;
	bool m_doubleBuffered;
	Display* m_display;
	Window   m_window;
	XdbeBackBuffer m_backBuffer;
	XWindowAttributes m_attrib;
};


