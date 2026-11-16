#pragma once
#include <stdbool.h>
#include <stdint.h>

typedef struct { int16_t x,y; } EPoint;
typedef struct { int16_t x,y,w,h; } ERect;
typedef struct { EPoint p[4]; ERect label[3]; bool valid; } ELayout;
int e_sin(int angle);
EPoint e_point(EPoint origin,int radius,int angle);
void e_geometry(int width,int height,int hour,int minute,int second,ELayout *out);
bool e_place(int width,int height,const ELayout *previous,ELayout *out);
int e_hour(int hour,int format,bool system24);
bool e_24(int format,bool system24);
void e_date(int year,int month,int day,int format,char *out,int capacity);
