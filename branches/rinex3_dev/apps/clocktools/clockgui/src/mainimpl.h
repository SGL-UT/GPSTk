/***************************************************************************
 *   Copyright (C) 2008 by Timothy Craddock,C050 SGL/GISD,null,(214)215-   *
 *   craddock@arlut.utexas.edu                                             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef MAINIMPL_H
#define MAINIMPL_H

#include "mainbase.h"

class MainImpl: public MainBase {
Q_OBJECT
public:
    MainImpl(QWidget *parent = 0, const char *name = 0);
public slots:
    virtual void _listSelect();
    virtual void _openFile();
    virtual void printResult();
    virtual void _dallanVariance();
    virtual void _ohadamardVariance();
    virtual void _nallanVariance();
    virtual void callProgram(char *program, char *title, int dem);
    virtual void _totalVariance();
    virtual void _oallanDeviation();
    virtual void _Xslider();
    virtual void _Zslider();
};

#endif
