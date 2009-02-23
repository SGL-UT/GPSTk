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
#include "mainimpl.h"
#include "resvec.h"
#include "glbox.h"

#include <fstream>
#include <qfiledialog.h>
#include <qtextedit.h>
#include <qcheckbox.h>
#include <qlistbox.h>
#include <qslider.h>
#include <qstring.h>
#include <qgl.h>


using namespace std;

// Globals
resvec rvec;
QString inputFile;

MainImpl::MainImpl(QWidget *parent, const char *name)
    :MainBase(parent, name)
{
}

void MainImpl::_openFile()
{
	QString s = QFileDialog::getOpenFileName("./", "Raw Data (*.dat)", this, "open file dialog", "Choose a file to open");
	inputFile = s;
	
	double X, Y;
	QString sX, sY, T;
	ifstream data(inputFile.ascii());
	rawIn->clear();
	while(!data.eof())
	{
		data >> X >> Y;
		sX.setNum(X);
		sY.setNum(Y);
		T += (sX + "   " + sY + "\n");
		
	}
	rawIn->append(T);
	data.close();
}

void MainImpl::_nallanVariance()
{
	callProgram("nallandev", "Allan Variance", _2D); // add outlier removal status to title
	displayList->insertItem("Allan Variance");
	displayList->setCurrentItem(displayList->count()-1);
//	printResult();
}

void MainImpl::_oallanDeviation()
{
	callProgram("oallandev", "Overlapping Allan Variance", _2D); // add outlier removal status to title
	displayList->insertItem("Overlapping Allan Variance");
	displayList->setCurrentItem(displayList->count()-1);
//	printResult();
}

void MainImpl::_ohadamardVariance()
{
	callProgram("ohadamarddev", "Overlapping Hadamard Variance", _2D); // add outlier removal status to title
	displayList->insertItem("Overlapping Hadamard Variance");
	displayList->setCurrentItem(displayList->count()-1);
//	printResult();
}

void MainImpl::_totalVariance()
{
	callProgram("tallandev", "Total Variance", _2D); // add outlier removal status to title
	displayList->insertItem("Total Variance");
	displayList->setCurrentItem(displayList->count()-1);
//	printResult();
}

void MainImpl::_dallanVariance()
{
	callProgram("dallandev", "Dynamic Allan Variance", _3D);
	displayList->insertItem("Dynamic Allan Variance");
	displayList->setCurrentItem(displayList->count()-1);
//	printResult();
}

void MainImpl::callProgram(char *program, char *title, int dem)
{
	// Generate Command & Execute
	char *resultFile = ".ctresults~";
	char command[256] = {0};
	strcat(command, "cat ");
	strcat(command, inputFile.ascii());
	strcat(command, " | ");
	
	if(rmOutlier->isChecked())
	strcat(command, "rmoutlier | ");
	
	strcat(command, program);
	strcat(command, " > ");
	strcat(command, resultFile);
	system(command);
	qWarning("Command Complete");
	
	// Read Results
	result proc;
	double x, y, z;
	double prevN, newN;

	proc.setTitle(title);
	proc.setDem(dem);
	ifstream results(resultFile);
	prevN = newN = z = 0.0;
	
	if(dem == _2D)
	{
		if(!results.eof()) results >> x >> y;
		
		do
		{
			proc.addResult(x, y);
			prevN = x;
			results >> x >> y;
			newN = x;
		} while(!results.eof() && (prevN != newN));
	}
	if(dem == _3D)
	{
		if(!results.eof()) results >> x >> y >> z;
		
		do
		{
			proc.addResult(x, y, z);
			prevN = y;
			results >> x >> y >> z;
			newN = y;
		} while(!results.eof() && (prevN != newN));
	}
	qWarning("Reading Complete");
	
	rvec.addResult(proc);
	qWarning("Addition Complete");
}

void MainImpl::printResult()
{
	result data = rvec.getResult(displayList->currentItem());
	dataPoint point;
	QString X, Y, Z, T;
	int i;
	
	gLBox1->plot(data);
	qWarning("Plotting Complete");
	
	if(data.getDem() == _2D)
	{
		for(i = 0; i < data.getLength(); i++)
		{
			point = data.getResult(i);
			X.setNum(point.getX());
			Y.setNum(point.getY());
			T += X + "   " + Y + "\n";
		}
	}
	if(data.getDem() == _3D)
	{
		for(i = 0; i < data.getLength(); i++)
		{
			point = data.getResult(i);
			X.setNum(point.getX());
			Y.setNum(point.getY());
			Z.setNum(point.getZ());
			T += X + "   " + Y + "   " + Z + "\n";
		}
	}
	rawOut->setText(T);
	qWarning("Writing Complete");
}

void MainImpl::_listSelect()
{
	printResult();
}

void MainImpl::_Xslider()
{
	gLBox1->setXRotation(Xslider->value());
}

void MainImpl::_Zslider()
{
	gLBox1->setZRotation(Zslider->value());
}
