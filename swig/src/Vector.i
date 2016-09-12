#ifndef GPSTK_VECTOR_HPP
#define GPSTK_VECTOR_HPP
#include <limits>
#include <vector>
#include "VectorBase.hpp"

namespace gpstk
{
   template <class T> class VectorSlice;

   template <class T>
   class Vector : public RefVectorBase<T, Vector<T> >
   {
   public:
      // typedef T value_type;
      // typedef T& reference;
      // typedef const T& const_reference;
      // typedef T* iterator;
      // typedef const T* const_iterator;

      Vector() : v(NULL), s(0) {}
      Vector(size_t siz) : s(siz)
         {
            v = new T[siz];
            if(!v) {
               VectorException e("Vector(size_t) failed to allocate");
               GPSTK_THROW(e);
            }
         }

      Vector(size_t siz, const T defaultValue) : s(siz)
         {
            v = new T[siz];
            if(!v) {
               VectorException e("Vector<T>(size_t, const T) failed to allocate");
               GPSTK_THROW(e);
            }
            this->assignFrom(defaultValue);
         }

      // template <class E>
      // Vector(const ConstVectorBase<T, E>& r) : s(r.size())
      //    {
      //       v = new T[r.size()];
      //       if(!v) {
      //          VectorException e("Vector<T>(ConstVectorBase) failed to allocate");
      //          GPSTK_THROW(e);
      //       }
      //       this->assignFrom(r);
      //    }

      Vector(const Vector& r) : s(r.s)
         {
            v = new T[r.s];
            if(!v) {
               VectorException e("Vector(Vector) failed to allocate");
               GPSTK_THROW(e);
            }
            this->assignFrom(r);
         }

      // Vector(const std::valarray<T>& r) : s(r.size())
      //    {
      //       v = new T[r.size()];
      //       if(!v) {
      //          VectorException e("Vector(valarray) failed to allocate");
      //          GPSTK_THROW(e);
      //       }
      //       this->assignFrom(r);
      //    }


      // template <class E>
      // Vector(const ConstVectorBase<T, E>& vec,
      //        size_t top,
      //        size_t num) : v(size_t(0)),s(0)
      //    {

      //       if ( top >= vec.size() ||
      //            top + num > vec.size())
      //       {
      //          VectorException e("Invalid dimensions or size for Vector(VectorBase)");
      //          GPSTK_THROW(e);
      //       }

      //       v = new T[num];
      //       if(!v) {
      //          VectorException e("Vector(subvector) failed to allocate");
      //          GPSTK_THROW(e);
      //       }
      //       size_t i;
      //       for(i = 0; i < num; i++)
      //          v[i] = vec(top+i);
      //       s = num;
      //    }

      ~Vector()
         { if (v) delete [] v; }

      // iterator begin() { return v; }

      // const_iterator begin() const { return v; }

      // iterator end() { return v + s; }

      // const_iterator end() const { return v + s; }

      // value_type front() { return v[s-1]; }

      // const_reference front() const { return v[s-1];}

      bool empty() const { return size() == 0; }

      // size_t size() const {return s; }

      size_t max_size() const { return std::numeric_limits<size_t>().max(); }

      // T& operator[] (size_t i)
      //    { return v[i]; }

      // T operator[] (size_t i) const
      //    { return v[i]; }

      // T& operator() (size_t i)
      //    { return v[i]; }

      // T operator() (size_t i) const
      //    { return v[i]; }

      // VectorSlice<T> operator[] (const std::slice& sli)
      //    { return VectorSlice<T>(*this, sli); }

      // Vector& operator=(const Vector& x)
      //    { resize(x.s); return this->assignFrom(x); }

      // template <class E>
      // Vector& operator=(const ConstVectorBase<T, E>& x)
      //    { resize(x.size()); return this->assignFrom(x); }

      // Vector& operator=(const std::valarray<T>& x)
      //    { resize(x.size()); return this->assignFrom(x); }

      // Vector& operator=(const T x)
      //    { return this->assignFrom(x); }

      // Vector& operator=(const T* x)
      //    { return this->assignFrom(x); }

      // inline Vector& operator=(const std::vector<T>& x)
      // {
      //     size_t i;
      //     size_t vs = x.size();
      //     (*this).resize(vs);

      //     for (i = 0; i < vs; i++)
      //         (*this)[i] = x[i];

      //     return (*this);
      // }

      Vector& resize(const size_t index)
         {
            if (index > s)
            {
               if (v)
                  delete [] v;
               v = new T[index];
               if(!v) {
                  VectorException e("Vector.resize(size_t) failed to allocate");
                  GPSTK_THROW(e);
               }
            }
            s = index;
            return *this;
         }

      Vector& resize(const size_t index, const T defaultValue)
         {
            resize(index);
            size_t i;
            for(i = 0; i < s; i++)
               v[i] = defaultValue;
            return *this;
         }

      std::vector<T> toStdVector()
      {
          std::vector<T> v;
          for(size_t i = 0; i < s; i++)
              v.push_back(v[i] );
          return v;
      }

   // inline Vector& operator<<(const Vector& b)
   // {
   //     size_t i;
   //     size_t vs = this->size();
   //     size_t bs = b.size();
   //     size_t rows = vs + bs;
   //     Vector<T> toReturn(rows);

   //     for (i = 0; i < vs; i++)
   //         toReturn[i] = (*this)[i];

   //     for (i = 0; i < bs; i++)
   //         toReturn[i+vs] = b[i];

   //     (*this) = toReturn;

   //     return (*this);
   // }

   // inline Vector& operator<<(const T &b)
   // {
   //     return (*this) << Vector(1,b);
   // }

   //  inline Vector operator&&(const Vector &b)
   //  {
   //      size_t i;
   //      size_t vs = this->size();
   //      size_t bs = b.size();
   //      size_t rows = vs + bs;
   //      Vector<T> toReturn(rows);

   //      for (i = 0; i < vs; i++)
   //          toReturn[i] = (*this)[i];

   //      for (i = 0; i < bs; i++)
   //          toReturn[i+vs] = b[i];

   //      return toReturn;
   //  }

   //  inline Vector operator&&(const T &b)
   //  {
   //      size_t i;
   //      size_t vs = this->size();
   //      size_t rows = vs + 1;
   //      Vector<T> toReturn(rows);

   //      for (i = 0; i < vs; i++)
   //          toReturn[i] = (*this)[i];

   //      toReturn[rows - 1] = b;

   //      return toReturn;
   //  }

//    private:

//       inline bool rangeCheck(const size_t index) const
//          {
// #ifdef RANGECHECK
//             return (index < s);
// #else
//             return true;
// #endif
//          }

//       T* v;
//       size_t s;
   };

   // template <class T>
   // class VectorSlice : public RefVectorSliceBase<T, VectorSlice<T> >
   // {
   // public:
   //    VectorSlice()
   //          : v(NULL), s(std::slice(0,0,0))
   //       { }

   //    VectorSlice(Vector<T>& vv)
   //          : v(&vv), s(std::slice(0,vv.size(),1))
   //       { }

   //    VectorSlice(Vector<T>& vv, const std::slice& ss)
   //          : v(&vv), s(ss)
   //       { vecSliceCheck(vv.size()); }

   //    template <class V>
   //    VectorSlice& operator=(const ConstVectorBase<T, V>& x)
   //       { return this->assignFrom(x); }

   //    VectorSlice& operator=(const std::valarray<T>& x)
   //       { return this->assignFrom(x); }

   //    VectorSlice& operator=(const T x)
   //       { return this->assignFrom(x); }

   //    VectorSlice& operator=(const T* x)
   //       { return this->assignFrom(x); }

   //    T& operator[] (size_t i)
   //       { return (*v)[start() + i * stride()]; }

   //    T operator[] (size_t i) const
   //       { return (*v)[start() + i * stride()]; }

   //    T& operator() (size_t i)
   //       { return (*v)[start() + i * stride()]; }

   //    T operator() (size_t i) const
   //       { return (*v)[start() + i * stride()]; }

   //    inline size_t size() const { return s.size(); }

   //    inline size_t start() const { return s.start(); }

   //    inline size_t stride() const { return s.stride(); }
   // private:

   //    Vector<T>* v;

   //    std::slice s;
   // };

   // template <class T>
   // class ConstVectorSlice : public ConstVectorSliceBase<T, ConstVectorSlice<T> >
   // {
   // public:
   //    ConstVectorSlice()
   //          : v(NULL), s(std::slice(0,0,0))
   //       { }

   //    ConstVectorSlice(const Vector<T>& vv)
   //          : v(&vv), s(std::slice(0,vv.size(),1))
   //       { }

   //    ConstVectorSlice(const Vector<T>& vv, const std::slice& ss)
   //          : v(&vv), s(ss)
   //       { vecSliceCheck(vv.size()); }

   //    T operator[] (size_t i) const
   //       { return (*v)[start() + i * stride()]; }

   //    T operator() (size_t i) const
   //       { return (*v)[start() + i * stride()]; }

   //    inline size_t size() const { return s.size(); }

   //    inline size_t start() const { return s.start(); }

   //    inline size_t stride() const { return s.stride(); }

   // private:
   //    const Vector<T>* v;
   //    std::slice s;
   // };
}

#include "VectorOperators.hpp"
#endif


%extend gpstk::Vector {
   gpstk::Vector<double> gpstk::Vector(std::vector<double> input) {
      const int n = input.size();
      std::valarray<double> tmp(n);
      for (int i = 0; i < n; i++) {
         tmp[i] = input[i];
      }
     return new gpstk::Vector<double>(tmp);
   }

   double __getitem__(unsigned int i) {
      return (*($self))[i];
   }

   // alias for operator<<
   Vector& concatenate(const Vector& b)
   {
      (*$self) << b;
   }

   std::string __str__() {
      std::ostringstream ss;
      size_t i;
      for(i = 0; i < $self->size() - 1; i++)
         ss << (*($self))[i] << ", ";
      ss << (*($self))[i];
      return ss.str();
   }

}


// be careful with whitespace inside templates, SWIG needs the extra spacing
%template (ConstVectorBase_double) gpstk::ConstVectorBase< double,Vector< double > >;
%template (RefVectorBase_double) gpstk::RefVectorBase< double,Vector< double > >;
%template (vector) gpstk::Vector<double>;


%pythoncode %{
    vector.__len__ = lambda self: self.size()

    def __iter__(self):
        self.index = 0
        return self
    vector.__iter__ = __iter__

    def next(self):
          if self.index >= len(self):
            raise StopIteration
          else:
            self.index += 1
            return self[self.index - 1]
    vector.next = next
%}
