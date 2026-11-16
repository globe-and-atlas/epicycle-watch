#include "model.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
static void fail(int w,int t,const char *why){fprintf(stderr,"FAIL width=%d time=%02d:%02d:%02d %s\n",w,t/3600,t/60%60,t%60,why);assert(0);}
/* Independent pixel-space collision oracle: sample each visible stroke densely. */
static void check(int w,int h,int t,ELayout *layout){
 const int widths[]={w==200?7:9,w==200?4:5,2};
 for(int i=0;i<3;i++){
  ERect r=layout->label[i];
  for(int j=i+1;j<3;j++){ERect b=layout->label[j];if(r.x<b.x+b.w && r.x+r.w>b.x && r.y<b.y+b.h && r.y+r.h>b.y)fail(w,t,"labels overlap");}
  for(int j=0;j<3;j++){
   for(int step=0;step<=240;step++){
    double f=step/240.0,x=layout->p[j].x+(layout->p[j+1].x-layout->p[j].x)*f,y=layout->p[j].y+(layout->p[j+1].y-layout->p[j].y)*f;
    double dx=x<r.x?r.x-x:x>r.x+r.w-1?x-r.x-r.w+1:0,dy=y<r.y?r.y-y:y>r.y+r.h-1?y-r.y-r.h+1:0;
    if(hypot(dx,dy)<widths[j]/2.0+1)fail(w,t,"label intersects a stroke");
   }
  }
  for(int x=0;x<2;x++)for(int y=0;y<2;y++){
   int xx=r.x+(x?r.w-1:0),yy=r.y+(y?r.h-1:0);
   if(xx<2||xx>=w-2||yy<18||yy>=h-18)fail(w,t,"label outside usable screen");
   if(w==260 && hypot(xx-130,yy-130)>127)fail(w,t,"label outside circle");
  }
 }
}
int main(void){
 assert(e_hour(0,1,false)==12);assert(e_hour(0,2,false)==0);assert(e_hour(13,1,false)==1);assert(e_hour(23,2,false)==23);assert(e_hour(23,0,true)==23);assert(e_hour(23,0,false)==11);
 char text[20];e_date(2026,11,8,1,text,20);assert(!strcmp(text,"2026-11-08"));e_date(2026,11,8,2,text,20);assert(!strcmp(text,"11/08/2026"));e_date(2026,11,8,3,text,20);assert(!strcmp(text,"08/11/2026"));e_date(2026,11,8,0,text,20);assert(!text[0]);
 for(int platform=0;platform<2;platform++){
  int w=platform?260:200,h=platform?260:228;ELayout previous={0};int radius[3]={platform?52:40,platform?35:27,platform?20:15};
  for(int t=0;t<43200;t++){
   ELayout layout;e_geometry(w,h,t/3600,t/60%60,t%60,&layout);
   if(layout.p[0].x!=w/2 || layout.p[0].y!=h/2)fail(w,t,"wrong center");
   double angles[]={t/120.0,(t%3600)/10.0,(t%60)*6.0};
   for(int j=0;j<3;j++){
    double a=angles[j]*3.141592653589793/180.0;
    if(fabs(layout.p[j+1].x-layout.p[j].x-radius[j]*sin(a))>1.0 || fabs(layout.p[j+1].y-layout.p[j].y+radius[j]*cos(a))>1.0)fail(w,t,"angle/pivot incorrect");
   }
   if(!e_place(w,h,&previous,&layout))fail(w,t,"no label placement");
   check(w,h,t,&layout);previous=layout;
   ELayout cold;e_geometry(w,h,t/3600,t/60%60,t%60,&cold);if(!e_place(w,h,NULL,&cold))fail(w,t,"cold label placement");check(w,h,t,&cold);
  }
  printf("PASS: %dx%d 43,200 linked poses and independently checked label placements\n",w,h);
 }
 return 0;
}
