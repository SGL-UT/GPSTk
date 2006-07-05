#pragma ident "$Id: //depot/sgl/gpstk/dev/apps/receiver/RinexObsRoller.hpp#1 $"

/**
 * @file Expression.hpp
 * Provides ability to resolve complex mathematical functions at runtime.
 * Class declarations.
 */

#ifndef EXPRESSION__HPP
#define EXPRESSION__HPP

#include <iostream>
#include <string>
#include <list>
 
// Classes based on notes and code found at
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
         BinOpNode( char theOp, ExpNode *theLeft, ExpNode *theRight ):
                 op(theOp), left(theLeft), right(theRight){}

         double getValue();
        
         std::ostream& print(std::ostream& ostr);

         char op;        // The operator.
         ExpNode *left;   // The left operand.
         ExpNode *right;  // The right operand.

   }; // end class BinOpNode

   class Token
   {
      public:

         Token(std::string value, int relPriority);
         std::string getValue(void) {return value;}
         int getPriority(void) {return priority;}

      private:
         std::string value;
         int priority;

   };

   class Expression 
   {
      public:
        
         Expression(const std::string& str);
         bool canEvaluate();
         void setVariable(const std::string& name, double value);
         double evaluate(void);
      
      private:

         void tokenize(const std::string& str);
         void buildExpressionTree(void);
      
         std::list<Token> tList;
         std::list<ExpNode *> eList;
         ExpNode *root;      
   }; // End class expression
   
   
} // End namespace gpstk

#endif // EXPRESSION_HPP
