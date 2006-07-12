#include <iostream>
#include <iomanip>
#include "Expression.hpp"

using namespace std;
using namespace gpstk;

int main(int argc, char* argv[])
{
   Expression::ExpNode *root;
   short test=1;
   
   cout << endl << "Unit Test #" << setw(2) << test++ << " -----------------------------------" << endl;
   {
      root = new Expression::ConstNode(12);
      root->print(cout);
      delete(root);
      cout << endl;
   }
   
   cout << endl << "Unit Test #" << setw(2) << test++ << " -----------------------------------" << endl;
   {
      Expression::ConstNode *node1 = new Expression::ConstNode(1);
      Expression::ConstNode *node2 = new Expression::ConstNode(3);
      root = new Expression::BinOpNode(string("+"),node1, node2);

      root->print(cout);
      cout << "=" << root->getValue();

      delete node1, node2, root;
      cout << endl;
   }
   
   cout << endl << "Unit Test #" << setw(2) << test++ << " -----------------------------------" << endl;
   {
      // Goal: 7+ (8-2)/3 = 9
      Expression::ConstNode *node1 = new Expression::ConstNode(2);
      Expression::ConstNode *node2 = new Expression::ConstNode(8);
      Expression::ConstNode *node3 = new Expression::ConstNode(3);
      Expression::ConstNode *node4 = new Expression::ConstNode(7);
      Expression::BinOpNode *op1   = new Expression::BinOpNode(string(" -"),node2, node1);
      Expression::BinOpNode *op2   = new Expression::BinOpNode(string("//"),op1,node3);
      root = new Expression::BinOpNode(string("+"),node4,op2);

      root->print(cout);
      cout << "=" << root->getValue() << endl;

      delete node1, node2, node3, node4, op1, op2, root;
   }

   cout << endl << "Unit Test #" << setw(2) << test++ << " -----------------------------------" << endl;
   {
         // Goal: compute cos(2pi)=-1
      Expression::ConstNode *node1 = new Expression::ConstNode(2);
      Expression::ConstNode *node2 = new Expression::ConstNode(3.141592647);
      Expression::BinOpNode *op1 = new Expression::BinOpNode(string("*"), node1, node2);
      root = new Expression::FuncOpNode(string("cos"), op1);
      root->print(cout);
      cout << "=" << root->getValue() << endl;

      delete node1, node2, op1, root;
      
   }
   
   cout << endl << "Unit Test #" << setw(2) << test++ << " -----------------------------------" << endl;
   {
      string istr("19");
      Expression xpr(istr);
      cout << "Input string: \"" << istr << "\"" << endl;
      xpr.print(cout);
      cout << "=" << xpr.evaluate() << endl;
   }

   cout << endl << "Unit Test #" << setw(2) << test++ << " -----------------------------------" << endl;
   {  
      string istr(" 1   + (6 - 2 ) * 3.2");
      Expression xpr(istr);
      cout << "Input string: \"" << istr << "\"" << endl;
      xpr.print(cout);
      cout << "=" << xpr.evaluate() << endl;
   }

   cout << endl << "Unit Test #" << setw(2) << test++ << " -----------------------------------" << endl;
      try {  
         string istr(" 1   + (6 - gamma ) * 3.2");
         Expression xpr(istr);
         cout << "Input string: \"" << istr << "\"" << endl;
         xpr.print(cout);
         cout << "=" << xpr.evaluate() << endl;
      }
      catch (gpstk::Expression::ExpressionException ee)
      {
         cout << endl << "The expected exception was generated: " << endl;
         cout << ee << endl;
      }
   
   cout << endl << "Unit Test #" << setw(2) << test++ << " -----------------------------------" << endl;
   {
      string istr(" 1 + 2*(3 + 1)"); 
      Expression xpr(istr);
      cout << "Input string: \"" << istr << "\"" << endl;
      xpr.print(cout);
      cout << "=" << xpr.evaluate() << endl;
   }

   cout << endl << "Unit Test #" << setw(2) << test++ << " -----------------------------------" << endl;
   {
      try {
         string istr(" 1 + 2*( beta + 1)"); 
         Expression xpr(istr);
         cout << "Input string: \"" << istr << "\"" << endl;
         xpr.print(cout);
         xpr.set("beta",1);
         cout << "=" << xpr.evaluate() << endl;
      }
      catch (...)
      {
         cout << "An unexpected exception was generated: ";
      }
   }

   cout << endl << "Unit Test #" << setw(2) << test++ << " -----------------------------------" << endl;
   {
      string istr(" 1 + 2*cos(3.141592647)"); 
      Expression xpr(istr);
      cout << "Input string: \"" << istr << "\"" << endl;
      xpr.print(cout);
      cout << "=" << xpr.evaluate() << endl;
   }

   cout << endl << "Unit Test #" << setw(2) << test++ << " -----------------------------------" << endl;
   {
      string istr(" 1E+1 + 4* 2E-2"); 
      Expression xpr(istr);
      cout << "Input string: \"" << istr << "\"" << endl;
      xpr.print(cout);
      cout << "=" << xpr.evaluate() << endl;
   }
   
   exit(0);   
}
