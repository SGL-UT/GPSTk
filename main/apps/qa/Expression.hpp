#pragma ident "$Id: //depot/sgl/gpstk/dev/apps/receiver/RinexObsRoller.hpp#1 $"

/**
 * @file Expression.hpp
 * Provides ability to resolve mathematical functions at runtime.
 * Class declarations.
 */

/* TO DO:
 1. Add variable node, add setvar 
 2. Add Unitary Operators node type. Implement trig funcs.
 3. move all classes to internal to Expression
 4. Add expression exception type, throw it when parsing user input.
 5. Make accessors const. Use const iterators w/in accessors, etc.
*/

#ifndef EXPRESSION__HPP
#define EXPRESSION__HPP

#include <iostream>
#include <string>
#include <list>
#include <map>
 
// Expression node classes based on material found at
// http://math.hws.edu/orr/s04/cpsc225/btrees/index.html

namespace gpstk
{
   /** @addtogroup MeasurementQuality */
   //@{
 
   /// Represents a node of any type in an expression tree.
   class ExpNode {
      public:

      virtual ~ExpNode() {}

      virtual double getValue() = 0;  // Return the value of this node
      virtual std::ostream& print(std::ostream& ostr) =0; // Write out this node to a stream

   }; // end class ExpNode


   // Represents a node that holds a number.  
   class ConstNode : public ExpNode {
      public:
         // Constructor.  Create a node to hold val.
         ConstNode( double theNum ): number(theNum) {}

         double getValue() { return number; }

         std::ostream& print(std::ostream& ostr) {
            ostr << number;
            return ostr;
         }

         double number;  // The number in the node.
   }; // end class ConstNode

   // Represents a node that holds an operator.
   class BinOpNode : public ExpNode {
      public:

         // Constructor.  Create a node to hold the given data.
         BinOpNode( const std::string& theOp, ExpNode *theLeft, ExpNode *theRight ):
                 op(theOp), left(theLeft), right(theRight){}

         double getValue();
        
         std::ostream& print(std::ostream& ostr);

         std::string op;        // The operator.
         ExpNode *left;   // The left operand.
         ExpNode *right;  // The right operand.

   }; // end class BinOpNode

   // Represents a node that holds a function of a signle variable
   class FuncOpNode : public ExpNode {
      public:

         // Constructor.  Create a node to hold the given data.
         FuncOpNode( const std::string& theOp, ExpNode *theRight ):
                 op(theOp), right(theRight){}

         double getValue();
        
         std::ostream& print(std::ostream& ostr);

         std::string op;        // The operator.
         ExpNode *right;  // The right operand.

   }; // end class FuncOpNode

   class Token
   {
      public:

         Token(std::string value, int relPriority, 
               bool isOperator);

         std::string getValue(void) {return value;}

         int getPriority(void) {return priority;}

         void setUsed(void) {used=true;}
         bool getUsed(void) {return used;}

         ExpNode * getNode(void) {return expNode;}
         void setNode(ExpNode *newNode) {expNode = newNode; }

         void setResolved(bool value) {resolved=value;}
         bool getResolved(void) {return resolved;}

         bool getOperator(void) {return isOperator;}
         void setOperator(bool value) {isOperator = value;}

         std::string getArgumentPattern(void) {return argumentPattern;}
         void setArgumentPattern(std::string value) {argumentPattern = value;}
      
         void print(std::ostream& ostr);
      
      private:
         std::string value;
         bool isOperator;
         bool resolved;
      
         int priority;
         ExpNode *expNode;
         bool used; // has the node of this token been used (linked to?)
      
         std::string argumentPattern;       
   };

   class Expression 
   {
      public:
        
         Expression(const std::string& str);
         bool canEvaluate();
         void setVariable(const std::string& name, double value);
         double evaluate(void) { return root->getValue(); }
         void print(std::ostream& ostr) {root->print(ostr);}
      

//         int isOperator(Token);
         int countResolvedTokens(void);
      
      private:

         void tokenize(const std::string& str);
         void buildExpressionTree(void);
      
         std::map<std::string,int> operatorMap;
         std::map<std::string,std::string> argumentPatternMap;
      
         std::list<Token> tList;
         std::list<ExpNode *> eList;
         ExpNode *root;      
   }; // End class expression
   
   
} // End namespace gpstk

#endif // EXPRESSION_HPP
