#include "HelmertTests.hpp"
#include <limits>

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

int main(int argc, char** argv)
{
   using namespace std;
   
   fancy = false;
   color = false;
   
   all = true;
   nrnsnt = false;
   arbitraryTranslation = false;
   scaleOnly = false;
   xAxisRot = false;
   yAxisRot = false;
   zAxisRot = false;
   smallRot = false;
   translationScaling = false;
   rotationScaling = false;
   rotationTranslation = false;
   
   parseArguments(argc, argv);
   
   int ret = 0;
   
   //gpstk::HelmertTransform& transform = gpstk::HelmertTransform::instance();
   
   if(all || nrnsnt)
   {
      printTitle("No Rotation, No Scale, No Translation");
      ret += testNoRotationNoScaleNoTranslation();
   }
   
   if(all || arbitraryTranslation)
   {
      printTitle("Arbitrary Translation");
      ret += testArbitraryTranslation();
   }
   
   if(all || scaleOnly)
   {
      printTitle("Scaling Only");
      ret += testScaling();
   }
   
   if(all || xAxisRot)
   {
      printTitle("X Axis Rotation");
      ret += testXAxisRotation();
   }
   
   if(all || yAxisRot)
   {
      printTitle("Y Axis Rotation");
      ret += testYAxisRotation();
   }
   
   if(all || zAxisRot)
   {
      printTitle("Z Axis Rotation");
      ret += testZAxisRotation();
   }
   
   if(all || smallRot)
   {
      printTitle("Small Rotation");
      ret += testSmallRotation();
   }
   
   if(all || translationScaling)
   {
      printTitle("Translation Scaling");
      ret += testTranslationScaling();
   }
   
   if(all || rotationScaling)
   {
      printTitle("Rotation Scaling");
      ret += testRotationScaling();
   }
   
   if(all || rotationTranslation)
   {
   	printTitle("Rotation Translation");
      ret += testRotationTranslation();
   }
   
   if(fancy)
   {
      cout << seperatorEquals << endl << endl;
	}
   
   if( ret > 0 )
   {
      cout << "Total Failures: ";
      if(color)
      {
         cout << bash_linux_red;
      }
      cout << ret << endl;
      if(color)
      {
         cout << bash_linux_normal;
      }
   }
   else
   {
      if(color)
      {
         cout << bash_linux_green;
      }
      cout << "All Tests Passed!" << endl;
      if(color)
      {
         cout << bash_linux_normal;
      }
   }
   
   return ret;
}


int testNoRotationNoScaleNoTranslation()
{
   using namespace std;
   using namespace gpstk;
   
      //Set up...
   HelmertTransform& transform = HelmertTransform::instance();
   HelmertTransform::TransformParameters tp;
   tp.scale = 0;
   tp.r1 = 0;
   tp.r2 = 0;
   tp.r3 = 0;
   tp.t1 = 0;
   tp.t2 = 0;
   tp.t3 = 0;
   ReferenceFrame from;
   ReferenceFrame to;
   from = from.createReferenceFrame("GenericFrame");
   to = to.createReferenceFrame("NoChange");
   transform.defineTransform(tp, to, from);
   
      //Test
   try
   {
      Position p(150.0, 150.0, 150.0,Position::Cartesian,NULL,from);
      cout << "    Position " << p << endl;
      Position newPos = transform.transform(to, p);
      cout << "New Position " << newPos << endl;
      Position fin = transform.transform(from, newPos);
      cout << "Fin Position " << fin << endl << endl;
      cout << "Initial and transformed positions are the same(true): "; printBool(compare(p, newPos), cout) << endl;
      cout << "      Initial and final positions are the same(true): "; printBool(compare(p,fin), cout) << endl;
      cout << "  Transformed and final positions are the same(true): "; printBool(compare(newPos,fin), cout) << endl;
      
      cout << "Test Passed? ";
      if(compare(newPos,p) && compare(p,fin))
      {
         printBool(true, cout) << endl << endl;
         return 0;
      }
      else
      {
         printBool(false, cout) << endl << endl;
         return 1;
      }
   }
   catch(InvalidParameter& e)
   {
      cout << "InvalidParameter: " << e << endl << endl;
      return 1;
   }
}
int testArbitraryTranslation()
{
   using namespace std;
   using namespace gpstk;
   
      //Set up...
   HelmertTransform& transform = HelmertTransform::instance();
   HelmertTransform::TransformParameters tp;
   tp.scale = 0;
   tp.r1 = 0;
   tp.r2 = 0;
   tp.r3 = 0;
   tp.t1 = 10;
   tp.t2 = 10;
   tp.t3 = 10;
   ReferenceFrame from;
   ReferenceFrame to;
   from = from.createReferenceFrame("GenericFrame");
   to = to.createReferenceFrame("TranslationOnly");
   transform.defineTransform(tp, to, from);
   
      //Test
   try{
      
      Position p(150.0, 150.0, 150.0,Position::Cartesian,NULL,from);
      cout << "    Position " << p << endl;
      Position newPos = transform.transform(to, p);
      cout << "New Position " << newPos << endl;
      Position fin = transform.transform(from, newPos);
      cout << "Fin Position " << fin << endl;
      cout << "Initial and transformed positions are the same(false): "; printBool(compare(p, newPos), cout) << endl;
      cout << "       Initial and final positions are the same(true): "; printBool(compare(p,fin), cout) << endl;
      cout << "  Transformed and final positions are the same(false): "; printBool(compare(newPos,fin), cout) << endl;
      
      cout << "Test Passed? ";
      if(compare(p, fin))
      {
         printBool(true, cout) << endl << endl;
         return 0;
      }
      else
      {
         printBool(false, cout) << endl << endl;
         return 1;
      }
   }catch(InvalidParameter& e)
   {
      cout << "InvalidParameter: " << e << endl << endl;
      return 1;
   }
   
}
int testScaling()
{
   using namespace std;
   using namespace gpstk;
   
      //Set up...
   HelmertTransform& transform = HelmertTransform::instance();
   HelmertTransform::TransformParameters tp;
      //100% increase
   tp.scale = 1;
   tp.r1 = 0;
   tp.r2 = 0;
   tp.r3 = 0;
   tp.t1 = 0;
   tp.t2 = 0;
   tp.t3 = 0;
   ReferenceFrame from;
   ReferenceFrame to;
   from = from.createReferenceFrame("GenericFrame");
   to = to.createReferenceFrame("ScaleOnly");
   transform.defineTransform(tp, to, from);
   
      //Test
   try
   {
      Position p(150.0, 150.0, 150.0,Position::Cartesian,NULL,from);
      cout << "    Position " << p << endl;
      Position newPos = transform.transform(to, p);
      cout << "New Position " << newPos << endl;
      Position fin = transform.transform(from, newPos);
      cout << "Fin Position " << fin << endl << endl;
      cout << "Initial and transformed positions are the same(false): "; printBool(compare(p, newPos), cout) << endl;
      cout << "       Initial and final positions are the same(true): "; printBool(compare(p,fin), cout) << endl;
      cout << "  Transformed and final positions are the same(false): "; printBool(compare(newPos,fin), cout) << endl;
      
      cout << "Test Passed? ";
      if(compare(p,fin))
      {
         printBool(true, cout) << endl << endl;
         return 0;
      }
      else
      {
         printBool(false, cout) << endl << endl;
         return 1;
      }
   }
   catch(InvalidParameter& e)
   {
      cout << "InvalidParameter: " << e << endl << endl;
      cout << "Test Passed? ";
      printBool(false, cout) << endl << endl;
      return 1;
   }
}
int testSmallRotation()
{
   using namespace std;
   using namespace gpstk;
   
      //Set up...
   HelmertTransform& transform = HelmertTransform::instance();
   HelmertTransform::TransformParameters tp;
   tp.scale = 0;
   tp.r1 = 1 * HelmertTransform::MAS;
   tp.r2 = 2 * HelmertTransform::MAS;
   tp.r3 = 3 * HelmertTransform::MAS;
   tp.t1 = 0;
   tp.t2 = 0;
   tp.t3 = 0;
   ReferenceFrame from;
   ReferenceFrame to;
   from = from.createReferenceFrame("GenericFrame");
   to = to.createReferenceFrame("SmallRotation");
   transform.defineTransform(tp, to, from);
   
      //Test
   try
   {
      Position p(150.0, 150.0, 150.0,Position::Cartesian,NULL,from);
      cout << "    Position " << p << endl;
      Position newPos = transform.transform(to, p);
      cout << "New Position " << newPos << endl;
      Position fin = transform.transform(from, newPos);
      cout << "Fin Position " << fin << endl << endl;
      cout << "Initial and transformed positions are the same(false): "; printBool(compare(p, newPos), cout) << endl;
      cout << "       Initial and final positions are the same(true): "; printBool(compare(p,fin), cout) << endl;
      cout << "  Transformed and final positions are the same(false): "; printBool(compare(newPos,fin), cout) << endl;
      
      cout << "Test Passed? ";
      if(compare(p,fin))
      {
         printBool(true, cout) << endl << endl;
         return 0;
      }
      else
      {
         printBool(false, cout) << endl << endl;
         return 1;
      }
   }
   catch(InvalidParameter& e)
   {
      cout << "InvalidParameter: " << e << endl << endl;
      cout << "Test Passed? ";
      printBool(false, cout) << endl << endl;
      return 1;
   }
}
int testXAxisRotation()
{
   using namespace std;
   using namespace gpstk;
   
      //Set up...
   HelmertTransform& transform = HelmertTransform::instance();
   HelmertTransform::TransformParameters tp;
   tp.scale = 0;
   tp.r1 = 1 * HelmertTransform::MAS;
   tp.r2 = 0;
   tp.r3 = 0;
   tp.t1 = 0;
   tp.t2 = 0;
   tp.t3 = 0;
   ReferenceFrame from;
   ReferenceFrame to;
   from = from.createReferenceFrame("GenericFrame");
   to = to.createReferenceFrame("XRotationOnly");
   transform.defineTransform(tp, to, from);
   
      //Test
   try
   {
      Position p(150.0, 150.0, 150.0,Position::Cartesian,NULL,from);
      cout << "    Position " << p << endl;
      Position newPos = transform.transform(to, p);
      cout << "New Position " << newPos << endl;
      Position fin = transform.transform(from, newPos);
      cout << "Fin Position " << fin << endl << endl;
      cout << "Initial and transformed positions are the same(false): "; printBool(compare(p, newPos), cout) << endl;
      cout << "       Initial and final positions are the same(true): "; printBool(compare(p,fin), cout) << endl;
      cout << "  Transformed and final positions are the same(false): "; printBool(compare(newPos,fin), cout) << endl;
      
      cout << "Test Passed? ";
      if(compare(p,fin))
      {
         printBool(true, cout) << endl << endl;
         return 0;
      }
      else
      {
         printBool(false, cout) << endl << endl;
         return 1;
      }
   }
   catch(InvalidParameter& e)
   {
      cout << "InvalidParameter: " << e << endl << endl;
      cout << "Test Passed? ";
      printBool(false, cout) << endl << endl;
      return 1;
   }
}
int testYAxisRotation()
{
   using namespace std;
   using namespace gpstk;
   
      //Set up...
   HelmertTransform& transform = HelmertTransform::instance();
   HelmertTransform::TransformParameters tp;
   tp.scale = 0;
   tp.r1 = 0;
   tp.r2 = 1 * HelmertTransform::MAS;
   tp.r3 = 0;
   tp.t1 = 0;
   tp.t2 = 0;
   tp.t3 = 0;
   ReferenceFrame from;
   ReferenceFrame to;
   from = from.createReferenceFrame("GenericFrame");
   to = to.createReferenceFrame("YRotationOnly");
   transform.defineTransform(tp, to, from);
   
      //Test
   try
   {
      Position p(150.0, 150.0, 150.0,Position::Cartesian,NULL,from);
      cout << "    Position " << p << endl;
      Position newPos = transform.transform(to, p);
      cout << "New Position " << newPos << endl;
      Position fin = transform.transform(from, newPos);
      cout << "Fin Position " << fin << endl << endl;
      cout << "Initial and transformed positions are the same(false): "; printBool(compare(p, newPos), cout) << endl;
      cout << "       Initial and final positions are the same(true): "; printBool(compare(p,fin), cout) << endl;
      cout << "  Transformed and final positions are the same(false): "; printBool(compare(newPos,fin), cout) << endl;
      
      cout << "Test Passed? ";
      if(compare(p,fin))
      {
         printBool(true, cout) << endl << endl;
         return 0;
      }
      else
      {
         printBool(false, cout) << endl << endl;
         return 1;
      }
   }
   catch(InvalidParameter& e)
   {
      cout << "InvalidParameter: " << e << endl << endl;
      cout << "Test Passed? ";
      printBool(false, cout) << endl << endl;
      return 1;
   }
}
int testZAxisRotation()
{
   using namespace std;
   using namespace gpstk;
   
      //Set up...
   HelmertTransform& transform = HelmertTransform::instance();
   HelmertTransform::TransformParameters tp;
   tp.scale = 0;
   tp.r1 = 0;
   tp.r2 = 0;
   tp.r3 = 1 * HelmertTransform::MAS;
   tp.t1 = 0;
   tp.t2 = 0;
   tp.t3 = 0;
   ReferenceFrame from;
   ReferenceFrame to;
   from = from.createReferenceFrame("GenericFrame");
   to = to.createReferenceFrame("ZRotationOnly");
   transform.defineTransform(tp, to, from);
   
      //Test
   try
   {
      Position p(150.0, 150.0, 150.0,Position::Cartesian,NULL,from);
      cout << "    Position " << p << endl;
      Position newPos = transform.transform(to, p);
      cout << "New Position " << newPos << endl;
      Position fin = transform.transform(from, newPos);
      cout << "Fin Position " << fin << endl << endl;
      cout << "Initial and transformed positions are the same(false): "; printBool(compare(p, newPos), cout) << endl;
      cout << "       Initial and final positions are the same(true): "; printBool(compare(p,fin), cout) << endl;
      cout << "  Transformed and final positions are the same(false): "; printBool(compare(newPos,fin), cout) << endl;
      
      cout << "Test Passed? ";
      if(compare(p,fin))
      {
         printBool(true, cout) << endl << endl;
         return 0;
      }
      else
      {
         printBool(false, cout) << endl << endl;
         return 1;
      }
   }
   catch(InvalidParameter& e)
   {
      cout << "InvalidParameter: " << e << endl << endl;
      cout << "Test Passed? ";
      printBool(false, cout) << endl << endl;
      return 1;
   }
}
int testTranslationScaling()
{
   using namespace std;
   using namespace gpstk;
   
      //Set up...
   HelmertTransform& transform = HelmertTransform::instance();
   HelmertTransform::TransformParameters tp;
   tp.scale = 0.5;
   tp.r1 = 0;
   tp.r2 = 0;
   tp.r3 = 0;
   tp.t1 = 10;
   tp.t2 = 10;
   tp.t3 = 10;
   ReferenceFrame from;
   ReferenceFrame to;
   from = from.createReferenceFrame("GenericFrame");
   to = to.createReferenceFrame("TranslationScaling");
   transform.defineTransform(tp, to, from);
   
      //Test
   try
   {
      Position p(150.0, 150.0, 150.0,Position::Cartesian,NULL,from);
      cout << "    Position " << p << endl;
      Position newPos = transform.transform(to, p);
      cout << "New Position " << newPos << endl;
      Position fin = transform.transform(from, newPos);
      cout << "Fin Position " << fin << endl << endl;
      cout << "Initial and transformed positions are the same(false): "; printBool(compare(p, newPos), cout) << endl;
      cout << "       Initial and final positions are the same(true): "; printBool(compare(p,fin), cout) << endl;
      cout << "  Transformed and final positions are the same(false): "; printBool(compare(newPos,fin), cout) << endl;
      
      cout << "Test Passed? ";
      if(compare(p,fin))
      {
         printBool(true, cout) << endl << endl;
         return 0;
      }
      else
      {
         printBool(false, cout) << endl << endl;
         return 1;
      }
   }
   catch(InvalidParameter& e)
   {
      cout << "InvalidParameter: " << e << endl << endl;
      cout << "Test Passed? ";
      printBool(false, cout) << endl << endl;
      return 1;
   }
}
int testRotationScaling()
{
   using namespace std;
   using namespace gpstk;
   
      //Set up...
   HelmertTransform& transform = HelmertTransform::instance();
   HelmertTransform::TransformParameters tp;
   tp.scale = 0.5;
   tp.r1 = 3 * HelmertTransform::MAS;
   tp.r2 = 2 * HelmertTransform::MAS;
   tp.r3 = 1 * HelmertTransform::MAS;
   tp.t1 = 0;
   tp.t2 = 0;
   tp.t3 = 0;
   ReferenceFrame from;
   ReferenceFrame to;
   from = from.createReferenceFrame("GenericFrame");
   to = to.createReferenceFrame("RotationScaling");
   transform.defineTransform(tp, to, from);
   
      //Test
   try
   {
      Position p(150.0, 150.0, 150.0,Position::Cartesian,NULL,from);
      cout << "    Position " << p << endl;
      Position newPos = transform.transform(to, p);
      cout << "New Position " << newPos << endl;
      Position fin = transform.transform(from, newPos);
      cout << "Fin Position " << fin << endl << endl;
      
      cout.precision(16);
      Position delta = fin - p;
      cout << "delta(f,i) = " << delta << endl;
      std::numeric_limits<double> eps;
      cout << "double epsilon = ";
      if(color)
      {
         cout << bash_linux_green;
      }
      cout << eps.epsilon() << endl;
      if(color)
      {
         cout << bash_linux_normal;
      }
      cout << endl;
      
      cout << "Initial and transformed positions are the same(false): "; printBool(compare(p, newPos), cout) << endl;
      cout << "       Initial and final positions are the same(true): "; printBool(compare(p,fin), cout) << endl;
      cout << "  Transformed and final positions are the same(false): "; printBool(compare(newPos,fin), cout) << endl; 
      
      cout << "Test Passed? ";
      if(compare(p,fin))
      {
         printBool(true, cout) << endl << endl;
         return 0;
      }
      else
      {
         printBool(false, cout) << endl << endl;
         return 1;
      }
   }
   catch(InvalidParameter& e)
   {
      cout << "InvalidParameter: " << e << endl << endl;
      cout << "Test Passed? ";
      printBool(false, cout) << endl << endl;
      return 1;
   }
}
int testRotationTranslation()
{
   using namespace std;
   using namespace gpstk;
   
      //Set up...
   HelmertTransform& transform = HelmertTransform::instance();
   HelmertTransform::TransformParameters tp;
   tp.scale = 0;
   tp.r1 = 3 * HelmertTransform::MAS;
   tp.r2 = 2 * HelmertTransform::MAS;
   tp.r3 = 1 * HelmertTransform::MAS;
   tp.t1 = 10;
   tp.t2 = 15;
   tp.t3 = 20;
   ReferenceFrame from;
   ReferenceFrame to;
   from = from.createReferenceFrame("GenericFrame");
   to = to.createReferenceFrame("RotationTranslation");
   transform.defineTransform(tp, to, from);
   
      //Test
   try
   {
      Position p(150.0, 150.0, 150.0,Position::Cartesian,NULL,from);
      cout << "    Position " << p << endl;
      Position newPos = transform.transform(to, p);
      cout << "New Position " << newPos << endl;
      Position fin = transform.transform(from, newPos);
      cout << "Fin Position " << fin << endl << endl;
      cout << "Initial and transformed positions are the same(false): "; printBool(compare(p, newPos), cout) << endl;
      cout << "       Initial and final positions are the same(true): "; printBool(compare(p,fin), cout) << endl;
      cout << "  Transformed and final positions are the same(false): "; printBool(compare(newPos,fin), cout) << endl;
      
      cout << "Test Passed? ";
      if(compare(p,fin))
      {
         printBool(true, cout) << endl << endl;
         return 0;
      }
      else
      {
         printBool(false, cout) << endl << endl;
         return 1;
      }
   }
   catch(InvalidParameter& e)
   {
      cout << "InvalidParameter: " << e << endl;
      cout << "Test Passed? ";
      printBool(false, cout) << endl << endl;
      return 1;
   }
}
int realData()
{
   return 0;
}

std::ostream& printBool(const bool print, std::ostream& out)
{
   if(print)
   {
      if(color)
      {
         cout << bash_linux_green;
      }
      out << "true";
      if(color)
      {
         cout << bash_linux_normal;
      }
   }
   else
   {
      if(color)
      {
         cout << bash_linux_red;
      }
      out << "false";
      if(color)
      {
         cout << bash_linux_normal;
      }
   }
   return out;
}

bool compare(gpstk::Position& left, gpstk::Position& right)
{
   using namespace gpstk;
   std::numeric_limits<double> eps;
   
   return((ABS(left.getX() - right.getX()) <= eps.epsilon()) &&
            (ABS(left.getY() - right.getY()) <= eps.epsilon()) &&
            (ABS(left.getZ() - right.getZ()) <= eps.epsilon()));
}

std::ostream& operator<<(std::ostream& out, gpstk::Position& pos)
{
   out.precision(12);
   if(color)
   {
      cout << bash_linux_blue;
   }
   out << " " << pos.getX() << " m, " << pos.getY() << " m, " << pos.getZ() << " m";
   if(color)
   {
      cout << bash_linux_normal;
   }
}

void parseArguments(int argc, char** argv)
{
   std::string str;
   for(int i = 1; i < argc; ++i)
   {
      str = argv[i];
      if(str == "-c" || str == "--linuxcolor")
      {
         color = true;
      }
      else if(str == "-h" || str == "--help")
      {
         printHelp();
         exit(0);
      }
      else if(str == "-f" || str == "--formatted")
      {
         fancy = true;
      }
      else if(str == "--all")
      {
         all = true;
      }
      else if(str == "--nothing")
      {
         nrnsnt = true;
         all = false;
      }
      else if(str == "--translation")
      {
         arbitraryTranslation = true;
         all = false;
      }
      else if(str == "--scale")
      {
         scaleOnly = true;
         all = false;
      }
      else if(str == "--xaxisrot")
      {
         xAxisRot = true;
         all = false;
      }
      else if(str == "--yaxisrot")
      {
         yAxisRot = true;
         all = false;
      }
      else if(str == "--zaxisrot")
      {
         zAxisRot = true;
         all = false;
      }
      else if(str == "--smallrot")
      {
         smallRot = true;
         all = false;
      }
      else if(str == "--translationscale")
      {
         translationScaling = true;
         all = false;
      }
      else if(str == "--rotationscale")
      {
         rotationScaling = true;
         all = false;
      }
      else if(str == "--rotationtranslation")
      {
         rotationTranslation = true;
         all = false;
      }
      
      else
      {
         cout << "Unknown Option: " << str << endl;
      }
   }
}
void printHelp()
{
   cout << "<Usage> ./HelmertTests <options> <tests>" << endl;
   cout << "Options:" << endl;
   cout << "   -c | --color = Colorize the output with linux bash escapes."
        << endl;
   cout << "   -f | --fancy = Use line seperators 80 characters wide." << endl;
   cout << "   -h | --help  = Prints this message then quits." << endl;
   cout << "Tests:" << endl;
   cout << "   --all" << endl;
   cout << "   --nothing" << endl;
   cout << "   --translation" << endl;
   cout << "   --scale" << endl;
   cout << "   --xaxisrot" << endl;
   cout << "   --yaxisrot" << endl;
   cout << "   --zaxisrot" << endl;
   cout << "   --smallrot" << endl;
   cout << "   --translationscale" << endl;
   cout << "   --rotationscale" << endl;
   cout << "   --rotationtranslation" << endl;
}

void printTitle(const char* str)
{
   if(!(fancy))
   {
   	if(color)
   		cout << bash_linux_yellow;
      
      cout << str << endl << endl;
      
      if(color)
         cout << bash_linux_normal;
      
      return;
	}
	
   std::string title(str);
   
   //Truncate title if too long
   if(title.length() > 72)
   {
   	title = title.substr(0,72);
	}
   
   int half_width = 37;
   int space_right = half_width - (title.length() / 2);
   int space_left = half_width;
   if(title.length() % 2 == 1)
   {   //The title is an odd length
   	space_left -= ((title.length() / 2) + 1);
	}
	else
	{
	   space_left -= (title.length() / 2);
	}
   
   cout << seperatorEquals << endl << "===";
   for(int i = 0; i < space_left; ++i)
   {
      cout << " ";
	}
	
	if(color)
	{
	   cout << bash_linux_yellow;
	}
	cout << title;
	if(color)
	{
	   cout << bash_linux_normal;
	}
	
	for(int i = 0; i < space_right; ++i)
	{
	   cout << " ";
	}
	
	cout << "===" << endl << seperatorEquals << endl;
}

/*
std::ostream& operator<<(std::ostream& out, const bool right)
{
   if(right)
      out << "true";
   else
      out << "false";
   return out;
}
*/
