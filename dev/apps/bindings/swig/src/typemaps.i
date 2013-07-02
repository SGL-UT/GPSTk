// Map a Python sequence into any sized C double array ( adapted from
// SWIG docs @ http://www.swig.org/Doc2.0/SWIGDocumentation.html )

%typemap(in) long[ANY](long temp[$1_dim0]) {
   int i;
   if (!PySequence_Check($input))
   {
      PyErr_SetString(PyExc_TypeError, "Expecting a sequence");
      return NULL;
   }
   if (PyObject_Length($input) != $1_dim0)
   {
      PyErr_SetString(PyExc_ValueError, "Expecting a sequence with $1_dim0 elements");
      return NULL;
   }
   for (i = 0; i < $1_dim0; i++)
   {
      PyObject *o = PySequence_GetItem($input,i);
      if (!PyLong_Check(o))
      {
         Py_XDECREF(o);
         PyErr_SetString(PyExc_ValueError, "Expecting a sequence of longs");
         return NULL;
      }
      temp[i] = PyLong_AsLong(o);
      Py_DECREF(o);
   }
   $1 = &temp[0];
}
