#pragma ident "$Id$"

//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 2.1 of the License, or
//  any later version.
//
//  The GPSTk is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GPSTk; if not, write to the Free Software Foundation,
//  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//  Copyright 2009, The University of Texas at Austin
//
//============================================================================

#include "glbox.h"
#include <iostream>
using namespace std;

GLBox::GLBox( QWidget* parent, const char* name )
    : QGLWidget( parent, name )
{
	xRot = 0.0;
	yRot = 0.0;
	zRot = 0.0;
	scale = 1.0;
	object = 0;
	setWindow(pow(10, 2), pow(10, 6), pow(10, -9), pow(10, -6), 0.0, 0.0);
}

GLBox::~GLBox()
{
	makeCurrent();
	glDeleteLists( object, 1 );
}

void GLBox::setWindow(double Xmin, double Xmax, double Ymin, double Ymax, double Zmin, double Zmax)
{
	xmin = Xmin;
	xmax = Xmax;
	ymin = Ymin;
	ymax = Ymax;
	zmin = Zmin;
	zmax = Zmax;

	alphax = 2.0/(log10(xmax)-log10(xmin));
	betax = -1.0-alphax*log10(xmin);

	alphay = 2.0/(log10(ymax)-log10(ymin));
	betay = -1.0-alphay*log10(ymin);

	alphaz = 2.0/(log10(zmax)-log10(zmin));
	betaz = -1.0-alphaz*log10(zmin);
}

double logRnd(double n)
{
	return(pow(10, floor(log10(n))));
}
double logRnu(double n)
{
	return(pow(10, ceil(log10(n))));
}
void GLBox::plot(result R)
{
	r=R;
	
	double XMin, XMax;
	double YMin, YMax;
	double ZMin, ZMax;
	double tx, ty, tz;
	int i;

	XMin = XMax = r.getResult(0).getX();
	YMin = YMax = r.getResult(0).getY();
	ZMin = ZMax = r.getResult(0).getZ();

	for(i = 0; i < r.getLength(); i++)
	{
		tx = r.getResult(i).getX();
		ty = r.getResult(i).getY();
		tz = r.getResult(i).getZ();

		if(tx < XMin) XMin = tx;
		if(tx > XMax) XMax = tx;
		if(ty < YMin) YMin = ty;
		if(ty > YMax) YMax = ty;
		if(tz < ZMin) ZMin = tz;
		if(tz > ZMax) ZMax = tz;
	}

	setWindow(logRnd(XMin), logRnu(XMax), logRnd(YMin), logRnu(YMax), logRnd(ZMin), logRnu(ZMax));
	object = makeObject();
	updateGL();
}

void GLBox::setXRotation( int degrees )
{
	xRot = (GLfloat)(degrees % 360);
	updateGL();
}

void GLBox::setYRotation( int degrees )
{
	yRot = (GLfloat)(degrees % 360);
	updateGL();
}

void GLBox::setZRotation( int degrees )
{
	zRot = (GLfloat)(degrees % 360);
	updateGL();
}

void GLBox::paintGL()
{
	glClear( GL_COLOR_BUFFER_BIT );
	glLoadIdentity();
	switch(r.getDem())
	{
		case _2D : glTranslatef( 0.0, 0.0, -5.25); break;
		case _3D : glTranslatef( 0.0, 0.0, -7.0 ); break;
	}
	glScalef( scale, scale, scale );
	glRotatef(xRot, 1.0, 0.0, 0.0);
	glRotatef(yRot, 0.0, 1.0, 0.0);
	glRotatef(zRot, 0.0, 0.0, 1.0);
	glCallList( object );
}

void GLBox::initializeGL()
{
	qglClearColor( white );
	object = makeObject();
	glShadeModel( GL_FLAT );

	GLfloat LightAmbient[]= { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat LightDiffuse[]= { 0.0f, 0.0f, 1.0f, 1.0f };
	GLfloat LightPosition[]= { 2.0f, 2.0f, 2.0f, 1.0f };

	glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);
	glLightfv(GL_LIGHT1, GL_POSITION, LightPosition);
	glEnable(GL_LIGHT1);
	//glEnable(GL_LIGHTING);
}

void GLBox::resizeGL( int w, int h )
{
	glViewport( 0, 0, (GLint)w, (GLint)h );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glFrustum( -1.0, 1.0, -1.0, 1.0, 5.0, 15.0 );
	glMatrixMode( GL_MODELVIEW );
}

double logfnc(double n, double alpha, double beta)
{
	return(alpha*log10(n)+beta);
}
GLuint GLBox::makeObject()
{	
	GLuint list;
	list = glGenLists( 1 );
	glNewList( list, GL_COMPILE );
	qglColor( gray );
	glLineWidth( 1.0 );
	
	if(r.getDem() == _2D)
	{
		glBegin( GL_LINE_LOOP );
		glVertex3f(  -1.0,  -1.0,  0.0 );
		glVertex3f(  -1.0,   1.0,  0.0 );
		glVertex3f(   1.0,   1.0,  0.0 );
		glVertex3f(   1.0,  -1.0,  0.0 );
		glEnd();
		
		double i;
		double hash;
		
		for(i = xmin; i < xmax; i += pow(10, floor(log10(i))) )
		{
			hash = alphax*log10(i)+betax;
			
			glBegin( GL_LINES);
			 glVertex3f(  hash,  1.0,  0.0);
			 glVertex3f(  hash, -1.0,  0.0);
			glEnd();
		}
		
		for(i = ymin; i < ymax; i += pow(10, floor(log10(i))) )
		{
			hash = alphay*log10(i)+betay;
			
			glBegin( GL_LINES );
			 glVertex3f(  1.0, hash,  0.0);
			 glVertex3f( -1.0, hash,  0.0);
			glEnd();
		}
		
		int j;
		
		for(j = 0; j < r.getLength()-1; j++)
		{
			qglColor(red); // make this biotch multicolored
			glBegin( GL_LINES );
			 glVertex3f( logfnc(r.getResult(j).getX(),alphax,betax), logfnc(r.getResult(j).getY(),alphay,betay), 0.0);
			 glVertex3f( logfnc(r.getResult(j+1).getX(),alphax,betax), logfnc(r.getResult(j+1).getY(),alphay,betay), 0.0);
			glEnd();
		}
	}
	if(r.getDem() == _3D)
	{
		const int period = 96;
		int ysize = (period-1)/2;
		int xsize = r.getLength()/ysize;
		int x, y;
		int index;
		
		
		for(y = 0; y < 47-1; y++)
		{
		for(x = 0; x < r.getLength()/47-1; x++)
		{
			glBegin(GL_LINE_LOOP);
			index = y*47+x;
			glColor3f(0.0, 0.0,logfnc(r.getResult(index).getZ(),alphaz,betaz)/(alphaz*log10(zmax)+betaz)/2+ (alphaz*log10(zmax)+betaz)/2);
			glVertex3f(logfnc(r.getResult(index).getX(), alphax, betax), logfnc(r.getResult(index).getY(), alphay, betay), logfnc(r.getResult(index).getZ(), alphaz, betaz));
			index = (x+1)*ysize+y;
			glColor3f(0.0, 0.0,logfnc(r.getResult(index).getZ(),alphaz,betaz)/(alphaz*log10(zmax)+betaz)/2+ (alphaz*log10(zmax)+betaz)/2);
			glVertex3f(logfnc(r.getResult(index).getX(), alphax, betax), logfnc(r.getResult(index).getY(), alphay, betay), logfnc(r.getResult(index).getZ(), alphaz, betaz));
			index = (x+1)*ysize+(y+1);
			glColor3f(0.0, 0.0,logfnc(r.getResult(index).getZ(),alphaz,betaz)/(alphaz*log10(zmax)+betaz)/2+ (alphaz*log10(zmax)+betaz)/2);
			glVertex3f(logfnc(r.getResult(index).getX(), alphax, betax), logfnc(r.getResult(index).getY(), alphay, betay), logfnc(r.getResult(index).getZ(), alphaz, betaz));
			index = x*ysize+(y+1);
			glColor3f(0.0, 0.0,logfnc(r.getResult(index).getZ(),alphaz,betaz)/(alphaz*log10(zmax)+betaz)/2+ (alphaz*log10(zmax)+betaz)/2);
			glVertex3f(logfnc(r.getResult(index).getX(), alphax, betax), logfnc(r.getResult(index).getY(), alphay, betay), logfnc(r.getResult(index).getZ(), alphaz, betaz));
			glEnd();
		}
		}
	}
	
	glEndList();
	
	return list;
}

/*
GLuint GLBox::make3d(ulong period)
{
	GLuint list;
	list = glGenLists( 1 );
	glNewList( list, GL_COMPILE );
	qglColor( gray );
	glLineWidth( 1.0 );

	glBegin(GL_LINE_LOOP);
	 glVertex3f( -1.0, -1.0, -1.0);
	 glVertex3f( -1.0,  1.0, -1.0);
	 glVertex3f(  1.0,  1.0, -1.0);
	 glVertex3f(  1.0, -1.0, -1.0);
	glEnd();

	glBegin(GL_LINES);
	 glVertex3f( -1.0, -1.0, -1.0);
	 glVertex3f( -1.0, -1.0,  1.0);
	glEnd();

	ulong x, y, i;
	ulong xsize, ysize;
	ulong index;

	ysize = (period-1)/2; // make variable
	xsize = dsize/ysize;

	
	
	
	double *smodel;
	smodel = new double [dsize];

	for(i = 0; i < dsize; i++) smodel[i] = zarray[i];

	for(y = 0; y < ysize; y++)
	{
		for(x = 0; x < xsize; x++)
		{
			//printf("%e\n", smodel[y*xsize+x]);
		}
	}

	for(y = 0; y < ysize; y++)
	{
		double sum, total, mean, stddev;

		sum = 0.0;
		total = 0.0;
		for(x = 0; x < xsize; x++)
		{
			sum += smodel[x*ysize+y];
			total += 1.0;
		}
		mean = sum/total;

		sum = 0.0;
		for(x = 0; x < xsize; x++)
		{
			sum += pow(smodel[x*ysize+y]-mean, 2.0);
		}
		stddev = sqrt(sum/total);

		double numGaps = 0.0;
		for(x = 0; x < xsize; x++)
		{
			if(sqrt(pow(smodel[x*ysize+y], 2.0)) > 3.0*stddev)
			{
				smodel[x*ysize+y]=0.0;
				numGaps += 1.0;
			}
		}

		sum = 0.0;
		total = 0.0;
		for(x = 0; x < xsize; x++)
		{
			sum += smodel[x*ysize+y];
			total += 1.0;
		}
		mean = sum/(total-numGaps);

		for(x = 0; x < xsize; x++)
		{
			smodel[x*ysize+y]=mean;
		}
	}

	for(i = 0; i < dsize; i++)
	{
		//printf("%e\n",smodel[i]);
		smodel[i] = zarray[i]-smodel[i];
	}

	double smin, smax;
	smin = smax = smodel[0];

	for(i = 0; i < dsize; i++)
	{
		if(smodel[i] > smax) smax = smodel[i];
		if(smodel[i] < smin) smin = smodel[i];
	}

	//qWarning("%e %e\n", smin, smax);


	for(y = 0; y < ysize-1; y++)
	{
		for(x = 0; x < xsize-1; x++)
		{
			glBegin(GL_QUADS);

			 index = x*ysize+y;
			 glColor3f( smodel[index]/(smax-smin)-smin/(smax-smin), 0.0,0.0 (alphaz*log10(zarray[index])+betaz)/(alphaz*log10(zmax)+betaz)/2+ (alphaz*log10(zmax)+betaz)/2);
			 glVertex3f(alphax*log10(xarray[index])+betax, alphay*log10(yarray[index])+betay, alphaz*log10(zarray[index])+betaz);

			 index = (x+1)*ysize+y;
			 glColor3f( smodel[index]/(smax-smin)-smin/(smax-smin), 0.0,0.0 (alphaz*log10(zarray[index])+betaz)/(alphaz*log10(zmax)+betaz)/2+ (alphaz*log10(zmax)+betaz)/2);
			 glVertex3f(alphax*log10(xarray[index])+betax, alphay*log10(yarray[index])+betay, alphaz*log10(zarray[index])+betaz);

			 index = (x+1)*ysize+(y+1);
			 glColor3f( smodel[index]/(smax-smin)-smin/(smax-smin), 0.0,0.0 (alphaz*log10(zarray[index])+betaz)/(alphaz*log10(zmax)+betaz)/2+ (alphaz*log10(zmax)+betaz)/2);
			 glVertex3f(alphax*log10(xarray[index])+betax, alphay*log10(yarray[index])+betay, alphaz*log10(zarray[index])+betaz);

			 index = x*ysize+(y+1);
			 glColor3f( smodel[index]/(smax-smin)-smin/(smax-smin), 0.0,0.0 (alphaz*log10(zarray[index])+betaz)/(alphaz*log10(zmax)+betaz)/2+ (alphaz*log10(zmax)+betaz)/2);
			 glVertex3f(alphax*log10(xarray[index])+betax, alphay*log10(yarray[index])+betay, alphaz*log10(zarray[index])+betaz);

			glEnd();
		}
	}

	delete [] smodel;
	glEndList();	
	return list;
}
*/
