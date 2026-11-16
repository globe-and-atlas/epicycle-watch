#include "model.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
static const int16_t SINE[91]={0,286,572,857,1143,1428,1713,1997,2280,2563,2845,3126,3406,3686,3964,4240,4516,4790,5063,5334,5604,5872,6138,6402,6664,6924,7182,7438,7692,7943,8192,8438,8682,8923,9162,9397,9630,9860,10087,10311,10531,10749,10963,11174,11381,11585,11786,11982,12176,12365,12551,12733,12911,13085,13255,13421,13583,13741,13894,14044,14189,14330,14466,14598,14726,14849,14968,15082,15191,15296,15396,15491,15582,15668,15749,15826,15897,15964,16026,16083,16135,16182,16225,16262,16294,16322,16344,16362,16374,16382,16384};
int e_sin(int angle){
 angle%=3600;if(angle<0)angle+=3600;
 int sign=1;if(angle>=1800){sign=-1;angle-=1800;}if(angle>900)angle=1800-angle;
 int degree=angle/10,part=angle%10;
 return sign*(SINE[degree]+(degree<90?(SINE[degree+1]-SINE[degree])*part/10:0));
}
static int rounded(int value){return value>=0?(value+8192)/16384:-((-value+8192)/16384);}
EPoint e_point(EPoint p,int radius,int angle){EPoint q={p.x+rounded(radius*e_sin(angle)),p.y-rounded(radius*e_sin(angle+900))};return q;}
bool e_24(int mode,bool system24){return mode==2 || (mode==0 && system24);}
int e_hour(int hour,int mode,bool system24){return e_24(mode,system24)?hour:(hour%12?hour%12:12);}
void e_date(int year,int month,int day,int mode,char *out,int n){
 if(mode==1)snprintf(out,n,"%04d-%02d-%02d",year,month,day);
 else if(mode==2)snprintf(out,n,"%02d/%02d/%04d",month,day,year);
 else if(mode==3)snprintf(out,n,"%02d/%02d/%04d",day,month,year);
 else if(n>0)out[0]=0;
}
void e_geometry(int width,int height,int hour,int minute,int second,ELayout *out){
 memset(out,0,sizeof(*out));out->p[0]=(EPoint){width/2,height/2};
 int radii[3]={width==200?40:52,width==200?27:35,width==200?15:20};
 int angles[3]={(hour%12)*300+minute*5+second/12,minute*60+second,second*60};
 for(int i=0;i<3;i++)out->p[i+1]=e_point(out->p[i],radii[i],angles[i]);
}
static bool inside(EPoint p,ERect r){return p.x>=r.x && p.x<r.x+r.w && p.y>=r.y && p.y<r.y+r.h;}
static int32_t cross(EPoint a,EPoint b,EPoint c){return (int32_t)(b.x-a.x)*(c.y-a.y)-(int32_t)(b.y-a.y)*(c.x-a.x);}
static bool between(int a,int b,int n){return n>=(a<b?a:b) && n<=(a>b?a:b);}
static bool on(EPoint a,EPoint b,EPoint p){return !cross(a,b,p) && between(a.x,b.x,p.x) && between(a.y,b.y,p.y);}
static bool intersects(EPoint a,EPoint b,EPoint c,EPoint d){
 int32_t x=cross(a,b,c),y=cross(a,b,d),u=cross(c,d,a),v=cross(c,d,b);
 if(((x<0 && y>0)||(x>0 && y<0)) && ((u<0 && v>0)||(u>0 && v<0)))return true;
 return (!x && on(a,b,c))||(!y && on(a,b,d))||(!u && on(c,d,a))||(!v && on(c,d,b));
}
static bool line_rect(EPoint a,EPoint b,ERect r,int margin){
 r.x-=margin;r.y-=margin;r.w+=2*margin;r.h+=2*margin;
 if(inside(a,r)||inside(b,r))return true;
 EPoint v[4]={{r.x,r.y},{r.x+r.w-1,r.y},{r.x+r.w-1,r.y+r.h-1},{r.x,r.y+r.h-1}};
 for(int i=0;i<4;i++)if(intersects(a,b,v[i],v[(i+1)%4]))return true;
 return false;
}
static bool overlaps(ERect a,ERect b,int margin){return a.x<b.x+b.w+margin && a.x+a.w+margin>b.x && a.y<b.y+b.h+margin && a.y+a.h+margin>b.y;}
static bool available(ERect r,int width,int height,ELayout *out,int mask){
 int radius=width==200?77:102,cx=width/2,cy=height/2;
 for(int x=0;x<2;x++)for(int y=0;y<2;y++){
  int dx=r.x+(x?r.w-1:0)-cx,dy=r.y+(y?r.h-1:0)-cy;
  if(dx*dx+dy*dy>radius*radius)return false;
 }
 int widths[3]={width==200?7:9,width==200?4:5,2};
 for(int i=0;i<3;i++)if(line_rect(out->p[i],out->p[i+1],r,widths[i]/2+2))return false;
 int cap[4]={width==200?6:8,width==200?6:8,width==200?5:6,2};
 for(int i=0;i<4;i++){
  ERect pivot={out->p[i].x-cap[i],out->p[i].y-cap[i],2*cap[i]+1,2*cap[i]+1};
  if(overlaps(r,pivot,2))return false;
 }
 for(int i=0;i<3;i++)if((mask&(1<<i)) && overlaps(r,out->label[i],3))return false;
 return true;
}
static int score(ERect r,EPoint anchor){int dx=r.x+r.w/2-anchor.x,dy=r.y+r.h/2-anchor.y;return dx*dx+dy*dy;}
/* Rare folded poses need a joint assignment, not a greedy last-label fallback.
   Static pools avoid a large stack allocation on the watch. */
static bool compact_labels(int width,int height,const int order[3],ELayout *out){
 static ERect pool[3][96];static int cost[3][96];int count[3]={0};
 const int radii[]={20,24,28,32,36,40,44,48,52};
 for(int label=0;label<3;label++){
  EPoint anchor=out->p[label+1];
  for(int d=0;d<9;d++)for(int angle=0;angle<3600;angle+=300){
   EPoint p=e_point(anchor,radii[d],angle);ERect r={p.x-13,p.y-9,26,18};
   if(!available(r,width,height,out,0))continue;
   bool duplicate=false;for(int i=0;i<count[label];i++)if(pool[label][i].x==r.x && pool[label][i].y==r.y)duplicate=true;
   if(!duplicate && count[label]<96){int n=count[label]++;pool[label][n]=r;cost[label][n]=score(r,anchor);}
  }
 }
 int a=order[0],b=order[1],c=order[2];
 const int limits[]={36*36,44*44,52*52};
 for(int limit=0;limit<3;limit++)for(int i=0;i<count[a];i++){
  if(cost[a][i]>limits[limit])continue;
  for(int j=0;j<count[b];j++){
   if(cost[b][j]>limits[limit] || overlaps(pool[a][i],pool[b][j],3))continue;
   for(int k=0;k<count[c];k++){
    if(cost[c][k]>limits[limit] || overlaps(pool[a][i],pool[c][k],3) || overlaps(pool[b][j],pool[c][k],3))continue;
    out->label[a]=pool[a][i];out->label[b]=pool[b][j];out->label[c]=pool[c][k];return true;
   }
  }
 }
 return false;
}
bool e_place(int width,int height,const ELayout *previous,ELayout *out){
 const int distances[]={20,28,36,48,62,78,96};
 // Place the most boundary-constrained pivot first, rather than pushing the
 // seconds label far inward when all three hands line up near midnight.
 int order[3]={0,1,2},mask=0;
 for(int i=0;i<3;i++)for(int j=i+1;j<3;j++){
  int a=order[i]+1,b=order[j]+1;
  int ax=out->p[a].x-width/2,ay=out->p[a].y-height/2,bx=out->p[b].x-width/2,by=out->p[b].y-height/2;
  if(bx*bx+by*by>ax*ax+ay*ay){int t=order[i];order[i]=order[j];order[j]=t;}
 }
 for(int step=0;step<3;step++){
  int label=order[step];
  EPoint anchor=out->p[label+1];ERect best={0};int best_score=INT_MAX;
  if(previous && previous->valid){
   ERect r=previous->label[label];r.x+=anchor.x-previous->p[label+1].x;r.y+=anchor.y-previous->p[label+1].y;
   if(score(r,anchor)<=52*52 && available(r,width,height,out,mask)){out->label[label]=r;mask|=1<<label;continue;}
  }
  for(int distance=0;distance<7;distance++)for(int angle=0;angle<3600;angle+=225){
   EPoint center=e_point(anchor,distances[distance],angle);ERect r={center.x-13,center.y-9,26,18};
   int value=score(r,anchor);
   if(value<best_score && available(r,width,height,out,mask)){best=r;best_score=value;}
  }
  if(best_score==INT_MAX){
   for(int y=18;y<height-18;y+=2)for(int x=4;x<width-26;x+=2){
    ERect r={x,y,26,18};int value=score(r,anchor);
    if(value<best_score && available(r,width,height,out,mask)){best=r;best_score=value;}
   }
  }
  if(best_score==INT_MAX){out->valid=false;return false;}
  out->label[label]=best;mask|=1<<label;
 }
 bool far=false;for(int i=0;i<3;i++)if(score(out->label[i],out->p[i+1])>44*44)far=true;
 if(far)compact_labels(width,height,order,out);
 out->valid=true;return true;
}
