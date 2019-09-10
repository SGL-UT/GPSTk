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
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
//
//  Copyright 2009, The University of Texas at Austin
//
//============================================================================


#ifndef GLBOX_H
#define GLBOX_H

#include <math.h>
#include <qgl.h>
#include "resvec.h"

#define glRED    0
#define glGREEN    1
#define glBLUE    2


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
