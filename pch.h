#pragma once


#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstdlib>

#include <algorithm>
#include <iostream>
#include <limits>
#include <map>
#include <string>
#include <vector>

#define SAFEFREE(ptr)		do {if(ptr) { free(ptr); (ptr) = nullptr; }} while(0)
#define SAFEDELETE(ptr)		do {if(ptr) { delete (ptr); (ptr) = nullptr; }} while(0)
#define SAFEDELETEARRAY(ptr) do {if(ptr) { delete [] (ptr); (ptr) = nullptr; }} while(0)
#define SAFERELEASE(ptr)	do {if(ptr) {(ptr)->Release(); (ptr) = nullptr; }} while(0)
#define DESTROYONFAILED(hr) do { if (FAILED(hr)) {Destroy(); return false;} } while(0)
#define RETURNONFAILED(hr)	do { if (FAILED(hr)) { return false;} } while(0)
#define RETURNONNULL(ptr)	do { if (!(ptr)) { return false;} } while(0)
