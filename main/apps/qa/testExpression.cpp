#include <iostream>
#include "Expression.hpp"

using namespace std;
using namespace gpstk;

int main(int argc, char* argv[])
{

   ExpNode *root;
   
   cout << endl << "Unit Test #1 -----------------------------------" << endl;
   {
      root = new ConstNode(12);
      root->print(cout);
      delete(root);
   }
   
   cout << endl << "Unit Test #2 -----------------------------------" << endl;
   {
      ConstNode *node1 = new ConstNode(1);
      ConstNode *node2 = new ConstNode(3);
      root = new BinOpNode('+',node1, node2);

      root->print(cout);
      cout << "=" << root->getValue();

      delete node1, node2, root;
   }
   
   cout << endl << "Unit Test #3 -----------------------------------" << endl;
   {
      // Goal: 7+ (8-2)/3 = 9
      ConstNode *node1 = new ConstNode(2);
      ConstNode *node2 = new ConstNode(8);
      ConstNode *node3 = new ConstNode(3);
      ConstNode *node4 = new ConstNode(7);
      BinOpNode *op1   = new BinOpNode('-',node2, node1);
      BinOpNode *op2   = new BinOpNode('/',op1,node3);
      root = new BinOpNode('+',node4,op2);

      root->print(cout);
      cout << "=" << root->getValue() << endl;

      delete node1, node2, node3, node4, op1, op2, root;
   }
   cout << endl << "Unit Test #4 -----------------------------------" << endl;
   {
      Expression xpr1(string(" 1 + (6 - 2) * 3.2 "));
         //Expression xpr2(string(" 1 + (cos 2) * 3.2 "));
   }
   
   exit(0);   
}
