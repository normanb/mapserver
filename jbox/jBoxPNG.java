//
// Jbox applet is part of the MapServer client support package (PNG version). - SDL -
//
// Paramters:
//    color int, int, int - color to use (rgb) for the selection rectangle.
//    jitter int - minimum size (in pixels) for a dragged box.
//    image string - url for the image to display, must be fully qualified.
//    thickness int - thickness (in pixels) of the dragged box.
//    box on/off - status for box drawing, default is on
//    verbose on/off - turns on verbosity so that all mouse movements are passed out to javascript	
//
// Public methods:
//    boxon - toggles box drawing on.
//    boxoff - toggles box drawing off.
//    setimage(string) - displays the image loaded from the passed url. 
//    setcursor(string) - sets the cursor to one of crosshair, hand or default

import java.applet.Applet;
import java.awt.*;
import java.awt.event.*;
import java.net.*;
import java.util.*;
import netscape.javascript.*;

import com.sixlegs.image.png.*;

class evalThread extends Thread {
    JSObject twindow;
    double tx1, tx2, ty1, ty2;
    String tname;

    public evalThread(JSObject window, String name, double x1, double y1, double x2, double y2) {
	twindow = window;
	tname = name;
	tx1 = x1;
	ty1 = y1;
	tx2 = x2;
	ty2 = y2;
    }

    public void run () {
	twindow.eval("setbox_handler('" + tname + "'," + Math.min(tx1,tx2) + "," + Math.min(ty1,ty2) + "," + Math.max(tx1,tx2) + "," + Math.max(ty1,ty2) + ");");
    }
}

public class jBox extends Applet implements MouseListener, MouseMotionListener {
    boolean     busy=false, box=true, init=true, verbose=false;
    Image 	img, busyimg=null;
    double      x1=-1, y1=-1, x2=-1, y2=-1;    
    int		jitter=5, cursorsize=4, thickness=1;

    PngImage	pngimg;

    Color       color=Color.red;

    JSObject  	window;

    Image       offScreenImage;
    Graphics 	offScreenGraphics;

    Dimension   screenSize;

    String 	name;
    
    public void init () {
	StringTokenizer st;
	String 	s=null, t=null;
	URL url=null;

	screenSize = this.getSize(); // nab the applet size

	offScreenImage = createImage(screenSize.width, screenSize.height);
	offScreenGraphics = offScreenImage.getGraphics();
	
	// get the Navigator window handle
	window = JSObject.getWindow(this);

	s = getParameter("jitter");
	if(s != null)
	    jitter = Integer.parseInt(s);
	
	s = getParameter("color");
	if(s != null) {
	    color = getColorParameter(s);
	}
	
	s = getParameter("thickness");
	if(s != null)
	    thickness = Integer.parseInt(s);

	s = getParameter("cursorsize");
	if(s != null)
	    cursorsize = Integer.parseInt(s);

        s = getParameter("verbose");
        if(s != null) {
          if(s.equalsIgnoreCase("true")) verbose = true;
        }

        name = getParameter("name");

	s = getParameter("busyimage");
	if(s != null) {
	    try {
		url = new URL(s);
	    } catch(MalformedURLException e) {
		window.eval("seterror_handler('Applet error. Malformed image URL.');");
		this.stop();
	    }
	    busyimg = getImage(url);
	}

	s = getParameter("box");
	if(s != null) {
	    if(s.equalsIgnoreCase("off")) box = false;
	}

	// nab the image itself
	s = getParameter("image");
        try {
	    url = new URL(s);
	} catch(MalformedURLException e) { 
	    window.eval("seterror_handler('Applet error. Malformed image URL.');");
	    this.stop();
	}	
	try {
  	  pngimg = new PngImage(url);
        } catch(java.io.IOException e) {
          return;
        }
        img = Toolkit.getDefaultToolkit().createImage(pngimg);	

	// we want mouse events and mouse movement events
	addMouseListener(this);
	addMouseMotionListener(this);
    }

    private Color getColorParameter(String s) {
	StringTokenizer st;
	int r, g, b;
	
	// check if a pre-defined color is specified	
	if (s.equalsIgnoreCase("black"))
	    return(Color.black);
	if (s.equalsIgnoreCase("blue"))
	    return(Color.blue);
	if (s.equalsIgnoreCase("cyan"))
	    return(Color.cyan);
	if (s.equalsIgnoreCase("darkGray"))
	    return(Color.darkGray);
	if (s.equalsIgnoreCase("gray"))
	    return(Color.gray);
	if (s.equalsIgnoreCase("green"))
	    return(Color.green);
	if (s.equalsIgnoreCase("lightGray"))
	    return(Color.lightGray);
	if (s.equalsIgnoreCase("magenta"))
	    return(Color.magenta);
	if (s.equalsIgnoreCase("orange"))
	    return(Color.orange);
	if (s.equalsIgnoreCase("pink"))
	    return(Color.pink);
	if (s.equalsIgnoreCase("red"))
	    return(Color.red);
	if (s.equalsIgnoreCase("white"))
	    return(Color.white);
	if (s.equalsIgnoreCase("yellow"))
	    return(Color.yellow);

	// nope, must be an RGB triplet
	st = new StringTokenizer(s, ",");
	r = Integer.parseInt(st.nextToken());
	g = Integer.parseInt(st.nextToken());
	b = Integer.parseInt(st.nextToken());
	return(new Color(r,g,b));
    }

    public void setcursorimage(String s1, String s2, int x, int y) {
	Image img;
	URL url=null;
	Point hotspot = new Point(x, y);
	Toolkit tk = getToolkit();

	try {
	    url = new URL(s2);
	} catch(MalformedURLException e) { 
	    window.eval("seterror_handler('Applet error. Malformed cursor image URL.');");
	    this.stop();
	}
	
	img = getImage(url);
	
	try {
	    this.setCursor(tk.createCustomCursor(img, hotspot, s1));
	} catch(IndexOutOfBoundsException e) { 
	    window.eval("seterror_handler('Applet error. Cursor hotspot out of bounds.');");
	    this.stop();
	}

	return;
    }

    public void setcursor(String name) {
       if (name.equalsIgnoreCase("hand"))
	 this.setCursor(java.awt.Cursor.getPredefinedCursor(java.awt.Cursor.HAND_CURSOR));
       else if (name.equalsIgnoreCase("crosshair"))
	 this.setCursor(java.awt.Cursor.getPredefinedCursor(java.awt.Cursor.CROSSHAIR_CURSOR));
       else
         this.setCursor(java.awt.Cursor.getPredefinedCursor(java.awt.Cursor.DEFAULT_CURSOR));       
    }

    public void boxon () {
	box = true;
	return;
    }
    
    public void boxoff () {
	box = false;
	x2 = x1; // collapse
	y2 = y1;
	
	repaint();

	window.eval("reset_handler('" + name + "'," + Math.min(x1,x2) + "," + Math.min(y1,y2) + "," + Math.max(x1,x2) + "," + Math.max(y1,y2) + ");");

	return;
    }
    
    public void setimage(String s) {
	URL url=null;
	MediaTracker tracker = new MediaTracker(this);
	
	busy = true;

	// reinitialize the cursor position
	x1 = x2 = (screenSize.width-1)/2.0;
	y1 = y2 = (screenSize.height-1)/2.0;
	
	if(busyimg != null)
	    repaint(); // show busy image

	// img.flush();
	   
	try {
	    url = new URL(s);
	} catch(MalformedURLException e) {
	    return;
	}

        try {
  	  pngimg = new PngImage(url);
        } catch(java.io.IOException e) {
          return;
        }
        img = Toolkit.getDefaultToolkit().createImage(pngimg);
	tracker.addImage(img, 0);

	try { 
	  tracker.waitForID(0); // wait till it loads
	} catch (InterruptedException e) {
	  return;
	}
	
	busy = false;	
        repaint();	
        window.eval("reset_handler('" + name + "'," + Math.min(x1,x2) + "," + Math.min(y1,y2) + "," + Math.max(x1,x2) + "," + Math.max(y1,y2) + ");");

	return;
    }

    //
    // Mouse event handlers
    //
    public void mouseClicked(MouseEvent event) { }
    public void mouseEntered(MouseEvent event) { 
      if(verbose) window.eval("mouseenter_handler('" + this.name + "');");
    }
    public void mouseExited(MouseEvent event) { 
      if(verbose) window.eval("mouseexit_handler('" + this.name + "');");
    }

    public void mouseMoved(MouseEvent event) { 
      if(verbose) window.eval("mousemove_handler('" + this.name + "'," + event.getX() + "," + event.getY() + ");");
    }

    public void mousePressed(MouseEvent event) {
	x1 = x2 = event.getX();
	y1 = y2 = event.getY();
    }

    public void mouseDragged(MouseEvent event) {
	x2 = event.getX();
	y2 = event.getY();
	if(!box) {
	    x1 = x2;
	    y1 = y2;	    
	}
	repaint();
    }

    public void mouseReleased(MouseEvent event) { 	
	
	if(box) {
	    x2 = event.getX();
	    y2 = event.getY();
	    if ( x2 > screenSize.width) { x2 = screenSize.width-1; } 
	    if ( x2 < 0 ) { x2 = 0; } 
	    if ( y2 > screenSize.height) { y2 = screenSize.height-1; } 
	    if ( y2 < 0 ) { y2 = 0; } 
	    
	    // check to see if (x2,y2) forms a large enough rectangle
	    // to be considered a new extent or if the user is just a
	    // poor mouse clikcker
	    if((Math.abs(x1-x2) <= jitter) || (Math.abs(y1-y2) <= jitter)) {
		x2 = x1;
		y2 = y1;
	    }
	} else {
	    x2 = x1;
	    y2 = y1;
	}
	
	repaint();

	// this a time for a re-draw if the application so chooses
	if(!busy)
	    new evalThread(window, name, x1, y1, x2, y2).start();
    }
    
    public void paint(Graphics g) {
	int i;
	int x, y, w, h;
	Rectangle rect;
	Polygon poly;

	// draw the image
	offScreenGraphics.drawImage(img,0,0,this);	
	
	// draw the user defined rectangle or crosshair
	offScreenGraphics.setColor(color);
	if((x1==x2) && (y1==y2)) {
	    if(cursorsize > 0) {
	      offScreenGraphics.drawLine((int)(x2-cursorsize), (int)y2, (int)(x2+cursorsize), (int)y2);
	      offScreenGraphics.drawLine((int)x2, (int)(y2-cursorsize), (int)x2, (int)(y2+cursorsize));
	    }
	} else {
	    x = (int)Math.min(x1,x2);
	    y = (int)Math.min(y1,y2);
	    w = (int)Math.abs(x1-x2);
	    h = (int)Math.abs(y1-y2);
	    
	    for(i=0; i<thickness; i++)
		offScreenGraphics.drawRect(x+i, y+i, w-(2*i), h-(2*i));
	}
	
	if(busy && busyimg != null) {
	    x = screenSize.width/2 - busyimg.getWidth(this)/2;
	    y = screenSize.height/2 - busyimg.getHeight(this)/2;
	    offScreenGraphics.drawImage(busyimg,x,y,this);
	}

        g.drawImage(offScreenImage, 0, 0, this);
    }
    
    public void destroy () {}
    
    public void update (Graphics g) {
	paint(g);
    }   
}
