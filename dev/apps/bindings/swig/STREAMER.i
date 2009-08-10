//swig interface for streaming operator

%module STREAMER

%{
#include "STREAMER.cpp"

using namespace std;
%}

%include "STREAMER.cpp"
