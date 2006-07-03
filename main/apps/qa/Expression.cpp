#pragma ident "$Id: //depot/sgl/gpstk/dev/apps/receiver/RinexObsRoller.hpp#1 $"

/**
 * @file Expression.cpp
 * Provides ability to resolve complex mathematical functions at runtime.
 * Class definitions.
 */

#include <sstream>
#include <map>
#include <vector>
#include <string>
#include <ctype.h>

#include "Expression.hpp"

static const int DEBUG=2;

namespace gpstk 
{
   
   double BinOpNode::getValue() 
   {
                   // To get the value, compute the value of the left and
                   // right operands, and combine them with the operator.
      double leftVal = left->getValue();
      double rightVal = right->getValue();

      switch ( op ) {
         case '+':  return leftVal + rightVal;
         case '-':  return leftVal - rightVal;
         case '*':  return leftVal * rightVal;
         case '/':  return leftVal / rightVal;
      }
   }


   std::ostream& BinOpNode::print(std::ostream& ostr) {
      ostr << "(";
      left->print(ostr);
      ostr << op;
      right->print(ostr);
      ostr << ")";

      return ostr;
   }

   Expression::Expression(const std::string& istr)
         : root(0)
   {
      tokenize(istr);
   }
   
   void Expression::tokenize(const std::string& istr)
   {
      using namespace std;

      if (DEBUG>1) cout << "Received: " << istr << endl;      

      // Remove spaces and parenthesis from the input string
      // Must store informatin from parenthesis in another list
      stringstream ss(istr);
      string str;
      char tempc;
      vector<int> baseOrder;
      int currentOrder = 0;
      
      while (ss >> skipws >> tempc)
      {
         bool strip=false;
         
         if (tempc == '(')
         {
            currentOrder+=10;
            strip=true;
         }
         
         if (tempc == ')')
         {
            currentOrder-=10;
            strip=true;
         }        
         
         if (!strip)
         { 
            baseOrder.push_back(currentOrder);
            str.append(&tempc,1);
         }
      }
      
      if (DEBUG>2)
      {
         for (int kk=0; kk<baseOrder.size(); kk++)
            cout << kk << " : " << str[kk] << " :  parenlvl "  << baseOrder[kk] << endl;
      }
      
         
      if (DEBUG>1) cout << "Whitespace eliminated: " << str << endl;
      
      map<string, int> opPrecMap;
      opPrecMap["+"]=0;
      opPrecMap["-"]=0;
      opPrecMap["cos"]=1;
      opPrecMap["sin"]=1;
      opPrecMap["tan"]=1;
      opPrecMap["*"]=2;
      opPrecMap["/"]=2;
      opPrecMap["^"]=3;

      map<string, int>::iterator it;
      list<int> breaks;
      breaks.push_back(0);

      
      // Break the expression up by operators          
      // Note the location and compute the order of operation of each.
      // key is location in string. value is ord. of op.
      map<int,int> opPriority;

      for (it=opPrecMap.begin(); it!=opPrecMap.end(); it++)
      {
         if (DEBUG>2) cout << "Looking fer " << it->first << endl;
         int position = 0;
         while ((position=str.find(it->first,position+1))!=string::npos)
         {
            if (DEBUG>2) cout << "found at : " << position << endl;
            breaks.push_back(position);
            opPriority[position] = it->second + baseOrder[position];

            int opPosition = position+(it->first.size());
            breaks.push_back(opPosition);
            opPriority[opPosition] = it->second  + baseOrder[position];
            
         }
         
      }
      breaks.push_back(str.size());

      // Sort the breaks into a list
      // Please note that sorting a linked list is expensive compared to
      // sorting a vector or map, as the search cost is high (lists are not sorted).
      // This should be revisited IF large expressions are handled by the GPSTk.
      breaks.sort();

      list<string> tokens;
      list<int>::iterator ls = breaks.begin(), rs = ls; // used to identify token string

      for (rs++ ;rs!=breaks.end(); rs++, ls++)
      {
         if (DEBUG>2) cout << "(" << *ls << ", " << *rs << ")" << endl;

         if (*rs!=*ls) // If not two operators in a row
         {
            string thisToken = str.substr(*ls,(*rs)-(*ls));
            int thisOop = opPriority[*ls];
            tokens.push_back(thisToken);
            if (DEBUG>1) cout << thisToken << " - " << thisOop << endl;

               /// !!!!!!!!!!!!!!!TODO
               // create one token with (thisToken, thisOop) per entry
               // and return the Token. gotta make a new class.
               // then create one opnode per token.
               // need to create unary op node for trig funcs
               // and var opnodes for user vars
               // and then RinexExpression (inherits from Expression) to overload 
               // az, el, P1, P2, etc.
               ///////////////////////
         }
         
      }

      int count=0;
      for (list<string>::iterator is=tokens.begin(); is!=tokens.end(); is++)
         if (DEBUG>2) cout << count++ << ": '" << *is << "' " << endl;
      
      
   } // end tokenize function
   
} // end namespace gpstk
 
