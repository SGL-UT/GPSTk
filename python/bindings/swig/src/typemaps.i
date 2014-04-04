// These typemaps help conversions between C style arrays and Python Lists.

// Note: if any more types needed to get added, it's time to start
//       using macros to do the job!

// Python Lists -> C arrays:
//     [PyLong] -> [long]
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
         PyErr_SetString(PyExc_TypeError, "Expecting a sequence of longs");
         return NULL;
      }
      temp[i] = PyLong_AsLong(o);
      Py_DECREF(o);
   }
   $1 = &temp[0];
}

//     [PyFloat] -> [double]
%typemap(in) double[ANY](double temp[$1_dim0]) {
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
      if (!PyFloat_Check(o))
      {
         Py_XDECREF(o);
         PyErr_SetString(PyExc_TypeError, "Expecting a sequence of doubles");
         return NULL;
      }
      temp[i] = PyFloat_AsDouble(o);
      Py_DECREF(o);
   }
   $1 = &temp[0];
}


// C arrays -> Python lists:
//     [int] -> [PyInt]
%typemap(out) int [ANY] {
   int i;
   $result = PyList_New($1_dim0);
   for (i = 0; i < $1_dim0; i++) {
      PyObject *o = PyInt_FromLong(static_cast<long>($1[i]));
      PyList_SetItem($result,i,o);
   }
}

//     [unsigned int] -> [PyInt]
%typemap(out) unsigned int [ANY] {
   int i;
   $result = PyList_New($1_dim0);
   for (i = 0; i < $1_dim0; i++) {
      PyObject *o = PyInt_FromLong(static_cast<long>($1[i]));
      PyList_SetItem($result,i,o);
   }
}

//     [long] -> [PyInt]
%typemap(out) long [ANY] {
   int i;
   $result = PyList_New($1_dim0);
   for (i = 0; i < $1_dim0; i++) {
      PyObject *o = PyInt_FromLong($1[i]);
      PyList_SetItem($result,i,o);
   }
}

//     [double] -> [PyFloat]
%typemap(out) double [ANY] {
   int i;
   $result = PyList_New($1_dim0);
   for (i = 0; i < $1_dim0; i++) {
      PyObject *o = PyFloat_FromDouble($1[i]);
      PyList_SetItem($result,i,o);
   }
}
