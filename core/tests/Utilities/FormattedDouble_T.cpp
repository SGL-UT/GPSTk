#include "TestUtil.hpp"
#include "FormattedDouble.hpp"

namespace std
{
   std::ostream& operator<<(std::ostream& s, gpstk::StringUtils::FFLead v)
   {
      s << static_cast<int>(v);
      return s;
   }
   std::ostream& operator<<(std::ostream& s, gpstk::StringUtils::FFSign v)
   {
      s << static_cast<int>(v);
      return s;
   }
}

// EXP = expected value
// FD = FormattedDouble object
// VAL = double precision value expected after reading (precision is
//   being thrown away deliberately in the formatting tests, so we
//   need this).
#define STRMASSERT(EXP, FD, VAL)                                        \
   {                                                                    \
      std::ostringstream oss;                                           \
      oss << FD;                                                        \
      std::string formatted = oss.str();                                \
      TUASSERTE(std::string, EXP, formatted);                           \
      std::istringstream iss(formatted);                                \
      gpstk::FormattedDouble fdin(FD.totalLen, FD.exponentChar);        \
      iss >> fdin;                                                      \
      TUASSERTFE(VAL, fdin.val);                                        \
   }

class FormattedDouble_T
{
public:
   unsigned streamTest();
   unsigned castTest();
};


unsigned FormattedDouble_T ::
streamTest()
{
   TUDEF("FormattedDouble", "operator<<");

   double p1 = 1.2345;  // positive test value
   double n1 = -1.2345; // negative test value
   gpstk::FormattedDouble t1(p1,gpstk::StringUtils::FFLead::Zero,5,2);
   gpstk::FormattedDouble t2(p1,gpstk::StringUtils::FFLead::Decimal,5,2);
   gpstk::FormattedDouble t3(p1,gpstk::StringUtils::FFLead::NonZero,5,2);
   gpstk::FormattedDouble t4(p1,gpstk::StringUtils::FFLead::Zero,5,2,0,'d');
   gpstk::FormattedDouble t5(p1,gpstk::StringUtils::FFLead::Zero,5,2,0,'D');
   gpstk::FormattedDouble t6(p1,gpstk::StringUtils::FFLead::Zero,5,2,0,'x');
   gpstk::FormattedDouble t7(p1,gpstk::StringUtils::FFLead::Zero,5,2,0,'E',
                             gpstk::StringUtils::FFSign::NegOnly);
   gpstk::FormattedDouble t8(p1,gpstk::StringUtils::FFLead::Zero,5,2,0,'E',
                             gpstk::StringUtils::FFSign::NegSpace);
   gpstk::FormattedDouble t9(p1,gpstk::StringUtils::FFLead::Zero,5,2,0,'E',
                             gpstk::StringUtils::FFSign::NegPos);
   gpstk::FormattedDouble t10(p1,gpstk::StringUtils::FFLead::Zero,5,4,0,'E',
                              gpstk::StringUtils::FFSign::NegPos);
   gpstk::FormattedDouble t11(n1,gpstk::StringUtils::FFLead::Zero,5,2,0);
   gpstk::FormattedDouble t12(n1,gpstk::StringUtils::FFLead::Decimal,5,2,0);
   gpstk::FormattedDouble t13(n1,gpstk::StringUtils::FFLead::NonZero,5,2,0);
   gpstk::FormattedDouble t14(n1,gpstk::StringUtils::FFLead::Zero,5,2,0,'d');
   gpstk::FormattedDouble t15(n1,gpstk::StringUtils::FFLead::Zero,5,2,0,'D');
   gpstk::FormattedDouble t16(n1,gpstk::StringUtils::FFLead::Zero,5,2,0,'x');
   gpstk::FormattedDouble t17(n1,gpstk::StringUtils::FFLead::Zero,5,2,0,'E',
                              gpstk::StringUtils::FFSign::NegOnly);
   gpstk::FormattedDouble t18(n1,gpstk::StringUtils::FFLead::Zero,5,2,0,'E',
                              gpstk::StringUtils::FFSign::NegSpace);
   gpstk::FormattedDouble t19(n1,gpstk::StringUtils::FFLead::Zero,5,2,0,'E',
                              gpstk::StringUtils::FFSign::NegPos);
   gpstk::FormattedDouble t20(n1,gpstk::StringUtils::FFLead::Zero,5,4,0,'E',
                              gpstk::StringUtils::FFSign::NegPos);

   STRMASSERT("0.1234e+01", t1, 0.1234e+01);
   STRMASSERT(".12345e+01", t2, .12345e+01);
   STRMASSERT("1.2345e+00", t3, 1.2345e+00);
   STRMASSERT("0.1234d+01", t4, 0.1234e+01);
   STRMASSERT("0.1234D+01", t5, 0.1234e+01);
   STRMASSERT("0.1234x+01", t6, 0.1234e+01);
   STRMASSERT("0.1234E+01", t7, 0.1234E+01);
   STRMASSERT(" 0.1234E+01", t8,  0.1234E+01);
   STRMASSERT("+0.1234E+01", t9, +0.1234E+01);
   STRMASSERT("+0.1234E+0001", t10, +0.1234E+0001);

   STRMASSERT("-0.1234e+01", t11, -0.1234e+01);
   STRMASSERT("-.12345e+01", t12, -.12345e+01);
   STRMASSERT("-1.2345e+00", t13, -1.2345e+00);
   STRMASSERT("-0.1234d+01", t14, -0.1234e+01);
   STRMASSERT("-0.1234D+01", t15, -0.1234e+01);
   STRMASSERT("-0.1234x+01", t16, -0.1234e+01);
   STRMASSERT("-0.1234E+01", t17, -0.1234E+01);
   STRMASSERT("-0.1234E+01", t18, -0.1234E+01);
   STRMASSERT("-0.1234E+01", t19, -0.1234E+01);
   STRMASSERT("-0.1234E+0001", t20, -0.1234E+0001);

   TURETURN();
}


unsigned FormattedDouble_T ::
castTest()
{
   TUDEF("FormattedDouble", "operator double");

   gpstk::FormattedDouble t1;
   t1 = 9.0;
   TUASSERTFE(9.0, t1.val);
   double foo = t1 * 123.0;
   TUASSERTFE(1107.0, foo);
   double bar = t1;
   TUASSERTFE(9.0, bar);

   gpstk::FormattedDouble t2(123.0, gpstk::StringUtils::FFLead::Zero, 10, 3, 0,
                             'x', gpstk::StringUtils::FFSign::NegPos);
   t2 = 9.0;
   TUASSERTFE(9.0, t2.val);
      // make sure the assignment didn't reset any existing data fields
   TUASSERTE(gpstk::StringUtils::FFLead,
             gpstk::StringUtils::FFLead::Zero, t2.leadChar);
   TUASSERTE(unsigned, 10, t2.mantissaLen);
   TUASSERTE(unsigned, 3, t2.exponentLen);
   TUASSERTE(char, 'x', t2.exponentChar);
   TUASSERTE(gpstk::StringUtils::FFSign,
             gpstk::StringUtils::FFSign::NegPos, t2.leadSign);

      // implicit conversions
   unsigned u = 9;
   gpstk::FormattedDouble t3;
   t3 = u;
   TUASSERTFE(9.0, t3.val);
   int i = 9;
   gpstk::FormattedDouble t4;
   t4 = i;
   TUASSERTFE(9.0, t4.val);

   TURETURN();
}


int main()
{
   unsigned errorTotal = 0;
   FormattedDouble_T testClass;
   errorTotal += testClass.streamTest();
   errorTotal += testClass.castTest();
   std::cout << "Total Failures for " << __FILE__ << ": " << errorTotal
             << std::endl;
   return errorTotal;
}
