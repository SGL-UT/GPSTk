#include "HelmertTests.hpp"

int main(int argc, char** argv)
{
      //Use this here to make using cout a little nicer...
   using namespace std;
   
   bool ret;
   
   gpstk::HelmertTransform& transform = gpstk::HelmertTransform::instance();
   //transform.dump();
   
   cout << "No scale, No Rotation, No Translation:" << endl << endl;
   ret = testNoRotationNoScaleNoTranslation();
   cout << "Test Passed? "; printBool(ret, cout) << endl << endl;
   
   cout << "Translation Only" << endl << endl;
   ret = testArbitraryTranslation();
   cout << "Test Passed? "; printBool(ret, cout) << endl << endl << endl;
   
   cout << "Scaline Only" << endl << endl;
   ret = testScaling();
   cout << "Test Passed? "; printBool(ret, cout) << endl << endl << endl;
   
   cout << "X Axis Rotation" << endl << endl;
   ret = testXAxisRotation();
   cout << "Test Passed? "; printBool(ret, cout) << endl << endl << endl;
   
   cout << "Y Axis Rotation" << endl << endl;
   ret = testYAxisRotation();
   cout << "Test Passed? "; printBool(ret, cout) << endl << endl << endl;
   
   cout << "Z Axis Rotation" << endl << endl;
   ret = testZAxisRotation();
   cout << "Test Passed? "; printBool(ret, cout) << endl << endl << endl;
   
   cout << "Small Rotation" << endl << endl;
   ret = testSmallRotation();
   cout << "Test Passed? "; printBool(ret, cout) << endl << endl << endl;
   
   cout << "Translation Scaling" << endl << endl;
   ret = testTranslationScaling();
   cout << "Test Passed? "; printBool(ret, cout) << endl << endl << endl;
   
   cout << "Rotation Scaling" << endl << endl;
   ret = testRotationScaling();
   cout << "Test Passed? "; printBool(ret, cout) << endl << endl << endl;
   
   cout << "Rotation Translation" << endl << endl;
   ret = testRotationTranslation();
   cout << "Test Passed? "; printBool(ret, cout) << endl << endl << endl;
   
}


bool testNoRotationNoScaleNoTranslation()
{
   using namespace std;
   using namespace gpstk;
   
      //Set up...
   HelmertTransform& transform = HelmertTransform::instance();
   TransformParameters tp;
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
      Position p(150.0, 150.0, 150.0);
      cout << "    Position " << p << endl;
      Position newPos = transform.transform(from, to, p);
      cout << "New Position " << newPos << endl;
      Position fin = transform.transform(to, from, newPos);
      cout << "Fin Position " << fin << endl << endl;
      cout << "Initial and transformed positions are the same(true): "; printBool(compare(p, newPos), cout) << endl;
      cout << "      Initial and final positions are the same(true): "; printBool(compare(p,fin), cout) << endl;
      cout << "  Transformed and final positions are the same(true): "; printBool(compare(newPos,fin), cout) << endl;
      
      return (compare(newPos,p) && compare(p,fin));
   }
   catch(InvalidParameter& e)
   {
      cout << "InvalidParameter: " << e << endl << endl;
      return false;
   }
}
bool testArbitraryTranslation()
{
   using namespace std;
   using namespace gpstk;
   
      //Set up...
   HelmertTransform& transform = HelmertTransform::instance();
   TransformParameters tp;
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
      
      Position p(150.0, 150.0, 150.0);
      cout << "    Position " << p << endl;
      Position newPos = transform.transform(from, to, p);
      cout << "New Position " << newPos << endl;
      Position fin = transform.transform(to, from, newPos);
      cout << "Fin Position " << fin << endl;
      cout << "Initial and transformed positions are the same(false): "; printBool(compare(p, newPos), cout) << endl;
      cout << "Initial and final positions are the same(true): "; printBool(compare(p,fin), cout) << endl;
      cout << "Transformed and final positions are the same(false): "; printBool(compare(newPos,fin), cout) << endl;
      return compare(p, fin);
   }catch(InvalidParameter& e)
   {
      cout << "InvalidParameter: " << e << endl << endl;
      return false;
   }
   
}
bool testScaling()
{
   using namespace std;
   using namespace gpstk;
   
      //Set up...
   HelmertTransform& transform = HelmertTransform::instance();
   TransformParameters tp;
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
      Position p(150.0, 150.0, 150.0);
      cout << "    Position " << p << endl;
      Position newPos = transform.transform(from, to, p);
      cout << "New Position " << newPos << endl;
      Position fin = transform.transform(to, from, newPos);
      cout << "Fin Position " << fin << endl << endl;
      cout << "Initial and transformed positions are the same(false): "; printBool(compare(p, newPos), cout) << endl;
      cout << "      Initial and final positions are the same(true): "; printBool(compare(p,fin), cout) << endl;
      cout << "  Transformed and final positions are the same(false): "; printBool(compare(newPos,fin), cout) << endl;
      
      return (compare(p,fin));
   }
   catch(InvalidParameter& e)
   {
      cout << "InvalidParameter: " << e << endl << endl;
      return false;
   }
}
bool testSmallRotation()
{
   using namespace std;
   using namespace gpstk;
   
      //Set up...
   HelmertTransform& transform = HelmertTransform::instance();
   TransformParameters tp;
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
      Position p(150.0, 150.0, 150.0);
      cout << "    Position " << p << endl;
      Position newPos = transform.transform(from, to, p);
      cout << "New Position " << newPos << endl;
      Position fin = transform.transform(to, from, newPos);
      cout << "Fin Position " << fin << endl << endl;
      cout << "Initial and transformed positions are the same(false): "; printBool(compare(p, newPos), cout) << endl;
      cout << "      Initial and final positions are the same(true): "; printBool(compare(p,fin), cout) << endl;
      cout << "  Transformed and final positions are the same(false): "; printBool(compare(newPos,fin), cout) << endl;
      
      Position delta = p - fin;
      cout << delta << endl;
      printBool(compare(p,newPos),cout) << endl;
      printBool(compare(p,fin),cout) << endl;
      return (compare(p,fin));
   }
   catch(InvalidParameter& e)
   {
      cout << "InvalidParameter: " << e << endl << endl;
      return false;
   }
}
bool testXAxisRotation()
{
   using namespace std;
   using namespace gpstk;
   
      //Set up...
   HelmertTransform& transform = HelmertTransform::instance();
   TransformParameters tp;
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
      Position p(150.0, 150.0, 150.0);
      cout << "    Position " << p << endl;
      Position newPos = transform.transform(from, to, p);
      cout << "New Position " << newPos << endl;
      Position fin = transform.transform(to, from, newPos);
      cout << "Fin Position " << fin << endl << endl;
      cout << "Initial and transformed positions are the same(false): "; printBool(compare(p, newPos), cout) << endl;
      cout << "      Initial and final positions are the same(true): "; printBool(compare(p,fin), cout) << endl;
      cout << "  Transformed and final positions are the same(false): "; printBool(compare(newPos,fin), cout) << endl;
      
      return (compare(p,fin));
   }
   catch(InvalidParameter& e)
   {
      cout << "InvalidParameter: " << e << endl << endl;
      return false;
   }
}
bool testYAxisRotation()
{
   using namespace std;
   using namespace gpstk;
   
      //Set up...
   HelmertTransform& transform = HelmertTransform::instance();
   TransformParameters tp;
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
      Position p(150.0, 150.0, 150.0);
      cout << "    Position " << p << endl;
      Position newPos = transform.transform(from, to, p);
      cout << "New Position " << newPos << endl;
      Position fin = transform.transform(to, from, newPos);
      cout << "Fin Position " << fin << endl << endl;
      cout << "Initial and transformed positions are the same(false): "; printBool(compare(p, newPos), cout) << endl;
      cout << "      Initial and final positions are the same(true): "; printBool(compare(p,fin), cout) << endl;
      cout << "  Transformed and final positions are the same(false): "; printBool(compare(newPos,fin), cout) << endl;
      
      return (compare(p,fin));
   }
   catch(InvalidParameter& e)
   {
      cout << "InvalidParameter: " << e << endl << endl;
      return false;
   }
}
bool testZAxisRotation()
{
   using namespace std;
   using namespace gpstk;
   
      //Set up...
   HelmertTransform& transform = HelmertTransform::instance();
   TransformParameters tp;
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
      Position p(150.0, 150.0, 150.0);
      cout << "    Position " << p << endl;
      Position newPos = transform.transform(from, to, p);
      cout << "New Position " << newPos << endl;
      Position fin = transform.transform(to, from, newPos);
      cout << "Fin Position " << fin << endl << endl;
      cout << "Initial and transformed positions are the same(false): "; printBool(compare(p, newPos), cout) << endl;
      cout << "      Initial and final positions are the same(true): "; printBool(compare(p,fin), cout) << endl;
      cout << "  Transformed and final positions are the same(false): "; printBool(compare(newPos,fin), cout) << endl;
      
      return (compare(p,fin));
   }
   catch(InvalidParameter& e)
   {
      cout << "InvalidParameter: " << e << endl << endl;
      return false;
   }
}
bool testTranslationScaling()
{
   using namespace std;
   using namespace gpstk;
   
      //Set up...
   HelmertTransform& transform = HelmertTransform::instance();
   TransformParameters tp;
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
      Position p(150.0, 150.0, 150.0);
      cout << "    Position " << p << endl;
      Position newPos = transform.transform(from, to, p);
      cout << "New Position " << newPos << endl;
      Position fin = transform.transform(to, from, newPos);
      cout << "Fin Position " << fin << endl << endl;
      cout << "Initial and transformed positions are the same(false): "; printBool(compare(p, newPos), cout) << endl;
      cout << "      Initial and final positions are the same(true): "; printBool(compare(p,fin), cout) << endl;
      cout << "  Transformed and final positions are the same(false): "; printBool(compare(newPos,fin), cout) << endl;
      
      return (compare(p,fin));
   }
   catch(InvalidParameter& e)
   {
      cout << "InvalidParameter: " << e << endl << endl;
      return false;
   }
}
bool testRotationScaling()
{
   using namespace std;
   using namespace gpstk;
   
      //Set up...
   HelmertTransform& transform = HelmertTransform::instance();
   TransformParameters tp;
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
      Position p(150.0, 150.0, 150.0);
      cout << "    Position " << p << endl;
      Position newPos = transform.transform(from, to, p);
      cout << "New Position " << newPos << endl;
      Position fin = transform.transform(to, from, newPos);
      cout << "Fin Position " << fin << endl << endl;
      cout << "Initial and transformed positions are the same(false): "; printBool(compare(p, newPos), cout) << endl;
      cout << "      Initial and final positions are the same(true): "; printBool(compare(p,fin), cout) << endl;
      cout << "  Transformed and final positions are the same(false): "; printBool(compare(newPos,fin), cout) << endl;
      
      return (compare(p,fin));
   }
   catch(InvalidParameter& e)
   {
      cout << "InvalidParameter: " << e << endl << endl;
      return false;
   }
}
bool testRotationTranslation()
{
   using namespace std;
   using namespace gpstk;
   
      //Set up...
   HelmertTransform& transform = HelmertTransform::instance();
   TransformParameters tp;
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
      Position p(150.0, 150.0, 150.0);
      cout << "    Position " << p << endl;
      Position newPos = transform.transform(from, to, p);
      cout << "New Position " << newPos << endl;
      Position fin = transform.transform(to, from, newPos);
      cout << "Fin Position " << fin << endl << endl;
      cout << "Initial and transformed positions are the same(false): "; printBool(compare(p, newPos), cout) << endl;
      cout << "      Initial and final positions are the same(true): "; printBool(compare(p,fin), cout) << endl;
      cout << "  Transformed and final positions are the same(false): "; printBool(compare(newPos,fin), cout) << endl;
      
      return (compare(p,fin));
   }
   catch(InvalidParameter& e)
   {
      cout << "InvalidParameter: " << e << endl << endl;
      return false;
   }
}
bool realData()
{
   return false;
}

std::ostream& printBool(const bool print, std::ostream& out)
{   
   if(print)
      out << "true";
   else
      out << "false";
   return out;
}

bool compare(gpstk::Position& left, gpstk::Position& right)
{
   using namespace gpstk;
   
   return(left.getX() == right.getX() &&
            left.getY() == right.getY() &&
            left.getZ() == right.getZ());
}

std::ostream& operator<<(std::ostream& out, gpstk::Position& pos)
{
	out.precision(12);
   out << " " << pos.getX() << " m, " << pos.getY() << " m, " << pos.getZ() << " m";
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
