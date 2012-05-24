swig -c++ -octave gpstk_ANSITime.i
g++ -g -c -fpic  gpstk_ANSITime_wrap.cxx   -I /usr/include
gcc -shared  gpstk_ANSITime_wrap.o  -o gpstk_ANSITime.oct -lgpstk