#pragma ident "$Id: //depot/sgl/gpstk/dev/apps/receiver/RinexObsRoller.hpp#1 $"

/**
 * @file Expression.cpp
 * Provides ability to resolve complex mathematical functions at runtime.
 * Class definitions.
 */

#include <sstream>
#include <map>
#include <list>
#include <vector>
#include <string>
#include <ctype.h>
#include <math.h>

#include "StringUtils.hpp"
#include "Expression.hpp"

static const int DEBUG=0;

namespace gpstk 
{
   
   double BinOpNode::getValue() 
   {

      // To get the value, compute the value of the left and
      // right operands, and combine them with the operator.
      double leftVal = left->getValue();
      double rightVal = right->getValue();

      if (op=="+") return leftVal + rightVal;
      if (op=="-") return leftVal - rightVal;
      if (op=="*") return leftVal * rightVal;
      if (op=="//") return leftVal / rightVal;

   }

   double FuncOpNode::getValue() 
   {
      // To get the value, compute the value of the right first
      double rightVal = right->getValue();

      if (op=="cos") return cos(rightVal);
      if (op=="sin") return sin(rightVal);
      if (op=="tan") return tan(rightVal);
      if (op=="acos") return acos(rightVal);
      if (op=="asin") return asin(rightVal);
      if (op=="atan") return atan(rightVal);
      if (op=="exp") return exp(rightVal);
      if (op=="abs") return fabs(rightVal);
      if (op=="sqrt") return sqrt(rightVal);
      if (op=="log") return log(rightVal);
      if (op=="log10") return log10(rightVal);
// else THROW exception
   }

   std::ostream& FuncOpNode::print(std::ostream& ostr) {
      ostr << op;
      right->print(ostr);

      return ostr;
   }

   std::ostream& BinOpNode::print(std::ostream& ostr) {
      ostr << "(";
      left->print(ostr);
      ostr << op;
      right->print(ostr);
      ostr << ")";

      return ostr;
   }

   Token::Token(std::string iValue, int iPriority, 
                bool isOp=false)
         :
         value(iValue), priority(iPriority), used(false), resolved(false),
         expNode(0), isOperator(isOp)
   {
   }

   void Token::print(std::ostream& ostr)
   {
      ostr <<" Value '" << value;
      ostr << "', operation priority " << priority << ", ";
    
      if (isOperator) ostr << "operator";
      else ostr << "not operator";

      ostr << ", ";
      
      if (used) ostr << "used,";
      else ostr << "not used,";

      if (resolved) ostr << "resolved";
      else ostr << "not resolved ";

      return;
   }
   

   Expression::Expression(const std::string& istr)
         : root(0)
   {
      operatorMap["+"]=1; 
      operatorMap["-"]=1;
      operatorMap["*"]=2;
      operatorMap["/"]=2;
      operatorMap["^"]=3;
      operatorMap["cos"]=4;
      operatorMap["sin"]=4;
      operatorMap["tan"]=4;
      operatorMap["acos"]=4;
      operatorMap["asin"]=4;
      operatorMap["atan"]=4;
      operatorMap["exp"]=4;
      operatorMap["abs"]=4;
      operatorMap["sqrt"]=4;
      operatorMap["log"]=4;
      operatorMap["log10"]=4;

      argumentPatternMap["+"]="RL";
      argumentPatternMap["-"]="RL";
      argumentPatternMap["*"]="RL";
      argumentPatternMap["/"]="RL";
      argumentPatternMap["^"]="RL";
      argumentPatternMap["cos"]="R";
      argumentPatternMap["sin"]="R";
      argumentPatternMap["tan"]="R";
      argumentPatternMap["acos"]="R";
      argumentPatternMap["asin"]="R";
      argumentPatternMap["atan"]="R";
      argumentPatternMap["exp"]="R";
      argumentPatternMap["abs"]="R";
      argumentPatternMap["sqrt"]="R";
      argumentPatternMap["log"]="R";
      argumentPatternMap["log10"]="R";
      
      tokenize(istr);
      buildExpressionTree();
   }
   
   void Expression::tokenize(const std::string& istr)
   {
      using namespace std;

      if (DEBUG>=1) cout << "Received: " << istr << endl;      

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
      
      if (DEBUG>=2)
      {
         for (int kk=0; kk<baseOrder.size(); kk++)
            cout << kk << " : " << str[kk] << " :  parenlvl "  << baseOrder[kk] << endl;
      }
      
         
      if (DEBUG>=3) cout << "Whitespace, parntheses eliminated: " << str << endl;

      map<string, int>::iterator it;
      list<int> breaks;
      breaks.push_back(0);

      
      // Break the expression into candidates for tokens. First known operators and functions
      // are found and marked with as a "break" in the the string.        
      // Note the location and compute the order of operation of each.
      // key is location in string. value is ord. of op.
      map<int,int> breakPriority;

      // Note when the breaks are due to an operator or to an operand.
      // Each break can become a token but not all othem do. 
      // Key is location in the string, value is boolean, true for operators and functions.
      map<int, bool> breakType;

      for (it=operatorMap.begin(); it!=operatorMap.end(); it++)
      {
         if (DEBUG>=2) cout << "Looking fer " << it->first << endl;
         int position = 0;
         while ((position=str.find(it->first,position+1))!=string::npos)
         {
            if (DEBUG>=2) cout << "found at : " << position << endl;
            breaks.push_back(position);
            breakPriority[position] = it->second + baseOrder[position];
            breakType[position] = true;

            int operandPos = position+(it->first.size());
            breaks.push_back(operandPos);
            breakPriority[operandPos] = baseOrder[operandPos];
            breakType[operandPos] = false;
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
         if (DEBUG>=2) cout << "(" << *ls << ", " << *rs << ")" << endl;

         if (*rs!=*ls) // If not two operators in a row
         {
            string thisToken = str.substr(*ls,(*rs)-(*ls));
            int thisOop = breakPriority[*ls];
            bool isOp = breakType[*ls];

               // Create the token
            Token tok(thisToken,thisOop, isOp);

            if ( tok.getOperator() ) 
               tok.setArgumentPattern( argumentPatternMap[thisToken] );
            
            // Create an expression node, save it, and link it to the token
            ExpNode *expNode;
            

            if (!isOp) 
            {
               expNode = new ConstNode(StringUtils::asDouble(thisToken));
               eList.push_back(expNode);
               tok.setNode(expNode);
               tok.setResolved(true);
            }     

            if (DEBUG>=3) cout << thisToken << " - " << thisOop << endl;

            // Now that the token has the best possible state, save it
            tList.push_back(tok);
         }
      }

      if (DEBUG>=1) {
         int count=0;
         cout << "Initial set of tokens:" << endl;         
         for (list<Token>::iterator is=tList.begin(); is!=tList.end(); is++)
         {
            cout << count++ << ": " ;
            is->print(cout);
            cout << endl;
	 }
      }      
      
   } // end tokenize function


   int Expression::countResolvedTokens(void)
   {
      using namespace std;
      
      list<Token>::iterator itt;
   
      // How many have already been processed? Are we done yet?
      int totalResolved=0;
      for (itt = tList.begin(); itt!=tList.end(); itt++)
      {
         if (itt->getResolved()) totalResolved++;
      }
      return totalResolved;
   }
   
   
   void Expression::buildExpressionTree(void)
   {
      using namespace std;
       
      list<Token>::iterator itt, targetToken;

      if ((tList.size()==1)&&(tList.begin()->getResolved()))
      {
         root = tList.begin()->getNode();
         return;
      }
      
      int totalResolved = countResolvedTokens();

      while (totalResolved<tList.size())
      {
         
         // 
         // Step through tokens to find the value for the highest priority
         // that doesn not yet have an expression node ExpNode assigned to it.
         // A subtle but important sideeffect of this traversal is taht
         // operators with the same priority get evaluated from right to
         // left.
         itt=tList.begin();
         int highestP = -1;

         for (itt = tList.begin(); itt !=tList.end(); itt++)
         {
            if ( itt->getOperator() && !itt->getResolved() )
            {
               if (itt->getPriority()>highestP) 
               {
                  targetToken = itt;
                  highestP=itt->getPriority();
               }
            }
         }
          
         if (DEBUG>=1) 
         {
            cout << "Highest unprocessed priority is " << flush;
            
            cout << targetToken->getValue() << " of priority "  << flush;;
            cout << targetToken->getPriority() <<  flush << endl;
         }

         if ( targetToken->getOperator() )
         {
            // Find the arg(s) for this operator.
            list<Token>::iterator leftArg=targetToken, rightArg=leftArg;


            stringstream argstr(targetToken->getArgumentPattern());
            char thisArg;
            bool searching;
            
            while (argstr >> thisArg)
            {
               switch (thisArg) {
                  case 'R': 
                     searching = true;

                     while (searching)
                     {
                        if (rightArg==tList.end())// TODO throw exception
                           cout << "Mistake, no right arg for " << targetToken->getValue() << endl;
                        else
                        rightArg++;

                        searching = (rightArg->getUsed());
                     }
                     
                     break;

                  case 'L':
               
                        // Resolve left arg
                     searching=true;

                     while (searching)
                     {
                        if (leftArg == tList.begin()) // TODO throw
                           cout << "Mistake - no right argument for operator?!" << endl;
                        else
                           leftArg--;

                        searching = (leftArg->getUsed());
                     }
                     
                     break;
               } // end of argumentPattern cases
            } // done processing argument list
            
            
           if (targetToken->getArgumentPattern()=="RL")
           {
              ExpNode *opNode = 
              new BinOpNode(targetToken->getValue(),leftArg->getNode(), rightArg->getNode());
              targetToken->setNode(opNode);
              eList.push_back(opNode);

              targetToken->setResolved(true);
              root = targetToken->getNode();

              leftArg->setUsed();
              rightArg->setUsed();
           }

           if (targetToken->getArgumentPattern()=="R")
           {
              ExpNode *opNode = 
              new FuncOpNode(targetToken->getValue(),rightArg->getNode());
              targetToken->setNode(opNode);

              eList.push_back(opNode);

              targetToken->setResolved(true);
              root = targetToken->getNode();

              rightArg->setUsed();
           }
            
         } // If this is an operator

            /**************
       else // This be a constant or a variable
         {
            targetToken->setResolved(true);
            targetToken->setUsed();
            root = targetToken->getNode();
         }
         
            ***********8*/
        if (DEBUG>=1) 
        {
           int count=0;
           for (list<Token>::iterator is=tList.begin(); is!=tList.end(); is++)
           {
              cout << count++ << ": " ;
              is->print(cout);
              cout << endl;
	   }
        }
        

            // Are we done yet?
         totalResolved = countResolvedTokens();


         if (DEBUG>=1) 
            cout << "Total processed is " << totalResolved << endl;
      }
      
      
   } // end buildExpressionTree
   
   
} // end namespace gpstk
 
