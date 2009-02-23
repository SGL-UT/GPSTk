#ifndef GLBOX_H
#define GLBOX_H

#include <math.h>
#include <qgl.h>
#include "resvec.h"

#define glRED	0
#define glGREEN	1
#define glBLUE	2


class GLBox : public QGLWidget
{
	Q_OBJECT
	public:
	 GLBox( QWidget* parent, const char* name );
	 ~GLBox();
	 void setWindow(double Xmin, double Xmax, double Ymin, double Ymax, double Zmin, double Zmax);
	 void plot(result r);
	 
	public slots:
	 virtual void setXRotation( int degrees );
	 virtual void setYRotation( int degrees );
	 virtual void setZRotation( int degrees );

	protected:
	 void initializeGL();
	 void paintGL();
	 void resizeGL( int w, int h );
	 virtual GLuint makeObject();
	 GLuint make3d(ulong period);

	private:
	 GLuint object;
	 GLfloat xRot, yRot, zRot, scale;
	 
	 result r;
	 double xmin, xmax;
	 double ymin, ymax;
	 double zmin, zmax;
	 double alphax, betax;
	 double alphay, betay;
	 double alphaz, betaz;
};

#endif // GLBOX_H
