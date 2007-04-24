/* Dummy NAV Code generator, replace me with something that generates more than a square wave */

#ifndef _NAV_CODE_GENERATOR_HPP
#define _NAV_CODE_GENERATOR_HPP



class NAVCodeGenerator {
 public:
   
  NAVCodeGenerator() : bit(0) { }

  int operator*() const               { return bit;  }
  void operator++()                   { bit^=1; } 
 
  void setIndex(int new_index)     { bit=new_index&1; }
  /* int  getIndex() const  */

 private:
  int bit;
};




#endif
