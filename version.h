#pragma once
#define MAJORVER 0
#define MINORVER 9
#define FIXVER   0

#define PRODUCTVER MAJORVER,MINORVER,FIXVER,0
#define _STR(x) #x
#define STR(x) _STR(x)
#define PRODUCTVERSTR STR(MAJORVER) "." STR(MINORVER) "." STR(FIXVER)
