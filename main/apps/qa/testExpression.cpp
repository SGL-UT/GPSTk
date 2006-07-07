#include <iostream>
#include <iomanip>
#include "Expression.hpp"

using namespace std;
using namespace gpstk;

int main(int argc, char* argv[])
{

   ExpNode *root;
   short test=1;
   
   cout << endl << "Unit Test #" << setw(2) << test++ << " -----------------------------------" << endl;
   {
      root = new ConstNode(12);
      root->print(cout);
      delete(root);
      cout << endl;
   }
   
   cout << endl << "Unit Test #" << setw(2) << test++ << " -----------------------------------" << endl;
   {
      ConstNode *node1 = new ConstNode(1);
      ConstNode *node2 = new ConstNode(3);
      root = new BinOpNode(string("+"),node1, node2);

      root->print(cout);
      cout << "=" << root->getValue();

      delete node1, node2, root;
      cout << endl;
   }
   
   cout << endl << "Unit Test #" << setw(2) << test++ << " -----------------------------------" << endl;
   {
      // Goal: 7+ (8-2)/3 = 9
      ConstNode *node1 = new ConstNode(2);
      ConstNode *node2 = new ConstNode(8);
      ConstNode *node3 = new ConstNode(3);
      ConstNode *node4 = new ConstNode(7);
      BinOpNode *op1   = new BinOpNode(string(" -"),node2, node1);
      BinOpNode *op2   = new BinOpNode(string("//"),op1,node3);
      root = new BinOpNode(string("+"),node4,op2);

      root->print(cout);
      cout << "=" << root->getValue() << endl;

      delete node1, node2, node3, node4, op1, op2, root;
   }

   cout << endl << "Unit Test #" << setw(2) << test++ << " -----------------------------------" << endl;
   {
         // Goal: compute cos(2pi)=-1
      ConstNode *node1 = new ConstNode(2);
      ConstNode *node2 = new ConstNode(3.141592647);
      BinOpNode *op1 = new BinOpNode(string("*"), node1, node2);
      root = new FuncOpNode(string("cos"), op1);
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
   {
      string istr(" 1 + 2*(3 + 1)"); 
      Expression xpr(istr);
      cout << "Input string: \"" << istr << "\"" << endl;
      xpr.print(cout);
      cout << "=" << xpr.evaluate() << endl;
   }
   
   cout << endl << "Unit Test #" << setw(2) << test++ << " -----------------------------------" << endl;
   {
      string istr(" 1 + 2*cos(3.141592647)"); 
      Expression xpr(istr);
      cout << "Input string: \"" << istr << "\"" << endl;
      xpr.print(cout);
      cout << "=" << xpr.evaluate() << endl;
   }
   
   exit(0);   
}
