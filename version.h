#pragma once
#define MAJORVER 2
#define MINORVER 0
#define FIXVER   0

#define PRODUCTVER MAJORVER,MINORVER,FIXVER,0
#define _STR(x) #x
#define STR(x) _STR(x)
#define PRODUCTVERSTR STR(MAJORVER) "." STR(MINORVER) "." STR(FIXVER)
