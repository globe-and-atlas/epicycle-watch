#include <pebble.h>
#include "config.h"
#include "model.h"

typedef struct { int palette,bg,hour,minute,second,time,date,labels,digital; } Settings;
static Settings s={0,0,0xFFFFFF,0x55AAFF,0xFF5500,0,0,1,1};
static const uint32_t PALETTES[6][4]={
 {0x000000,0xFFFFFF,0x55AAFF,0xFF5500},
 {0xFFFFAA,0x000000,0x0000AA,0xAA0000},
 {0x000000,0xAAFFAA,0x00FFAA,0xFFFF55},
 {0x000055,0xFFFFFF,0xAAFFFF,0xFF55AA},
 {0xAAAAAA,0x000000,0x000055,0xAA0000},
 {0x000000,0xFFFFFF,0xAAAAAA,0xFFFFFF}
};
static Window *window;
static Layer *face;
static struct tm current;
static ELayout previous;
static int width,height;
#if EPI_FIXTURE
static int fh=EPI_HOUR,fm=EPI_MINUTE,fs=EPI_SECOND;
static bool proof=false;
#endif
static uint32_t color(int i){
 if(s.palette<6)return PALETTES[s.palette][i];
 return i==0?(uint32_t)s.bg:i==1?(uint32_t)s.hour:i==2?(uint32_t)s.minute:(uint32_t)s.second;
}
static uint32_t mix(uint32_t a,uint32_t b){
 uint32_t out=0;
 for(int shift=0;shift<24;shift+=8){int value=(((a>>shift)&255)*2+((b>>shift)&255))/3;out|=(uint32_t)(((value+42)/85)*85)<<shift;}
 return out;
}
/* Original small bitmap glyphs: predictable native bounds on both displays. */
static const uint8_t NUMBERS[10][7]={
 {14,17,19,21,25,17,14},{4,12,4,4,4,4,14},{14,17,1,2,4,8,31},
 {30,1,1,14,1,1,30},{2,6,10,18,31,2,2},{31,16,16,30,1,1,30},
 {14,16,16,30,17,17,14},{31,1,2,4,8,8,8},{14,17,17,14,17,17,14},{14,17,17,15,1,1,14}
};
static uint8_t bits(char c,int row){
 if(c>='0'&&c<='9')return NUMBERS[c-'0'][row];
 static const char CHARS[]="EPICYLAM/-:";
 static const uint8_t OTHER[][7]={
  {31,16,16,30,16,16,31},{30,17,17,30,16,16,16},{31,4,4,4,4,4,31},
  {15,16,16,16,16,16,15},{17,17,10,4,4,4,4},{16,16,16,16,16,16,31},
  {14,17,17,31,17,17,17},{17,27,21,21,17,17,17},
  {1,2,2,4,8,8,16},{0,0,0,31,0,0,0},{0,1,1,0,1,1,0}
 };
 const char *at=strchr(CHARS,c);return at?OTHER[at-CHARS][row]:0;
}
static int char_width(char c){return c==':'?1:c==' '?2:5;}
static int text_width(const char *text){int n=0;while(*text)n+=char_width(*text++)+1;return n?n-1:0;}
static void text(GContext *ctx,const char *value,int x,int y,int w,int h,uint32_t ink){
 int source=text_width(value),offset=0;
 if(!source)return;
 graphics_context_set_fill_color(ctx,GColorFromHEX(ink));
 for(int i=0;value[i];i++){
  int columns=char_width(value[i]);
  for(int row=0;row<7;row++)for(int col=0;col<columns;col++)if(bits(value[i],row)&(1<<(columns-1-col))){
   int left=x+(offset+col)*w/source,right=x+(offset+col+1)*w/source;
   int top=y+row*h/7,bottom=y+(row+1)*h/7;
   graphics_fill_rect(ctx,GRect(left,top,right-left,bottom-top),0,GCornerNone);
  }
  offset+=columns+1;
 }
}
static void circle(GContext *ctx,EPoint p,int radius,uint32_t ink){
 graphics_context_set_fill_color(ctx,GColorFromHEX(ink));graphics_fill_circle(ctx,GPoint(p.x,p.y),radius);
}
static void line(GContext *ctx,EPoint a,EPoint b,int thickness,uint32_t ink){
 graphics_context_set_stroke_color(ctx,GColorFromHEX(ink));graphics_context_set_stroke_width(ctx,thickness);
 graphics_draw_line(ctx,GPoint(a.x,a.y),GPoint(b.x,b.y));
}
static void draw_footer(GContext *ctx){
 char group[3][4],date[20];uint32_t bg=color(0),h=color(1),dim=mix(bg,h);
 e_date(current.tm_year+1900,current.tm_mon+1,current.tm_mday,s.date,date,sizeof(date));
 const char *heading=s.date?date:"EPICYCLE";int count=text_width(heading);
 text(ctx,heading,(width-count)/2,width==200?9:5,count,7,mix(dim,h));
 if(!s.digital)return;
 snprintf(group[0],4,"%02d",e_hour(current.tm_hour,s.time,clock_is_24h_style()));
 snprintf(group[1],4,"%02d",current.tm_min);snprintf(group[2],4,"%02d",current.tm_sec);
 bool twenty_four=e_24(s.time,clock_is_24h_style());
 int target=twenty_four?70:78,left=(width-target)/2,y=height-16;
 if(width==260)y=243;
 // Fit the complete readout, including AM/PM, inside the lower round chord.
 char all[16];snprintf(all,sizeof(all),"%s:%s:%s%s",group[0],group[1],group[2],twenty_four?"":current.tm_hour>=12?" PM":" AM");
 int source=text_width(all),offset=0;
 for(int i=0;i<3;i++){
  int length=text_width(group[i]);int x=left+offset*target/source,advance=(offset+length)*target/source-x+left;
  text(ctx,group[i],x,y,advance,10,color(i+1));offset+=length;
  if(i<2){offset++;int colon_x=left+offset*target/source;int colon_w=(offset+1)*target/source-offset*target/source;text(ctx,":",colon_x,y,colon_w,10,dim);offset+=2;}
 }
 if(!twenty_four){const char *period=current.tm_hour>=12?"PM":"AM";offset+=4;int x=left+offset*target/source;text(ctx,period,x,y,left+target-x,10,h);}
}
static void draw(Layer *layer,GContext *ctx){
 graphics_context_set_antialiased(ctx,false);
 graphics_context_set_fill_color(ctx,GColorFromHEX(color(0)));
 graphics_fill_rect(ctx,layer_get_bounds(layer),0,GCornerNone);
 EPoint center={width/2,height/2};int radius=width==200?90:116;
 for(int i=0;i<60;i++){
  if(width==260 && s.digital && i>=28 && i<=32)continue;
  int length=i%5?2:8;uint32_t ink=i%5?mix(color(0),color(1)):mix(mix(color(0),color(1)),color(1));
  line(ctx,e_point(center,radius-length,i*60),e_point(center,radius,i*60),i%5?1:2,ink);
 }
 ELayout layout;e_geometry(width,height,current.tm_hour,current.tm_min,current.tm_sec,&layout);
 bool valid=!s.labels || e_place(width,height,previous.valid?&previous:NULL,&layout);
 int widths[]={width==200?7:9,width==200?4:5,2};
 for(int i=0;i<3;i++){line(ctx,layout.p[i],layout.p[i+1],widths[i],color(i+1));circle(ctx,layout.p[i+1],widths[i]/2,color(i+1));}
 int caps[]={width==200?6:8,width==200?6:8,width==200?5:6};
 for(int i=0;i<3;i++){
  circle(ctx,layout.p[i],caps[i],color(i==2?2:1));circle(ctx,layout.p[i],caps[i]-2,color(0));circle(ctx,layout.p[i],2,color(i+1));
 }
 circle(ctx,layout.p[3],2,color(3));
 if(s.labels && valid){
  int values[]={e_hour(current.tm_hour,s.time,clock_is_24h_style()),current.tm_min,current.tm_sec};
  for(int i=0;i<3;i++){
   ERect r=layout.label[i];char value[4];snprintf(value,4,"%02d",values[i]);
   graphics_context_set_fill_color(ctx,GColorFromHEX(color(0)));graphics_fill_rect(ctx,GRect(r.x,r.y,r.w,r.h),2,GCornersAll);
   text(ctx,value,r.x+2,r.y+2,22,14,color(i+1));
  }
  previous=layout;
 }else previous.valid=false;
 draw_footer(ctx);
#if EPI_FIXTURE
 if(proof){
  graphics_context_set_fill_color(ctx,GColorMagenta);graphics_fill_rect(ctx,GRect(width/2-4,19,2,2),0,GCornerNone);
  graphics_context_set_fill_color(ctx,valid?GColorGreen:GColorRed);graphics_fill_rect(ctx,GRect(width/2+2,19,2,2),0,GCornerNone);
 }
#endif
}
static void tick(struct tm *now,TimeUnits changed){
 (void)changed;current=*now;
#if EPI_FIXTURE
 current.tm_hour=fh;current.tm_min=fm;current.tm_sec=fs;current.tm_year=126;current.tm_mon=10;current.tm_mday=8;
#endif
 layer_mark_dirty(face);
}
static void setting(DictionaryIterator *iter,int message,int key,int maximum,int *value){
 Tuple *t=dict_find(iter,message);
 if(!t || (t->type!=TUPLE_INT && t->type!=TUPLE_UINT))return;
 int v=t->value->int32;if(v<0 || v>maximum)return;
 *value=v;persist_write_int(key,v);
}
static void inbox(DictionaryIterator *iter,void *context){
 (void)context;
 setting(iter,MESSAGE_KEY_Palette,1,6,&s.palette);setting(iter,MESSAGE_KEY_Background,2,0xFFFFFF,&s.bg);
 setting(iter,MESSAGE_KEY_HourColor,3,0xFFFFFF,&s.hour);setting(iter,MESSAGE_KEY_MinuteColor,4,0xFFFFFF,&s.minute);setting(iter,MESSAGE_KEY_SecondColor,5,0xFFFFFF,&s.second);
 setting(iter,MESSAGE_KEY_TimeFormat,6,2,&s.time);setting(iter,MESSAGE_KEY_DateFormat,7,3,&s.date);
 setting(iter,MESSAGE_KEY_Labels,8,1,&s.labels);setting(iter,MESSAGE_KEY_DigitalTime,9,1,&s.digital);
#if EPI_FIXTURE
 int *fields[]={&fh,&fm,&fs};int maximum[]={23,59,59};
 for(int i=0;i<3;i++){Tuple *t=dict_find(iter,11000+i);if(t && t->type==TUPLE_INT && t->value->int32>=0 && t->value->int32<=maximum[i])*fields[i]=t->value->int32;}
 Tuple *p=dict_find(iter,11022);if(p && p->type==TUPLE_INT)proof=p->value->int32==1;
#endif
 previous.valid=false;window_set_background_color(window,GColorFromHEX(color(0)));
 time_t now=time(NULL);tick(localtime(&now),SECOND_UNIT);
}
static void restore(int key,int max,int *value){if(persist_exists(key)){int v=persist_read_int(key);if(v>=0 && v<=max)*value=v;}}
static void init(void){
 restore(1,6,&s.palette);restore(2,0xFFFFFF,&s.bg);restore(3,0xFFFFFF,&s.hour);restore(4,0xFFFFFF,&s.minute);restore(5,0xFFFFFF,&s.second);
 restore(6,2,&s.time);restore(7,3,&s.date);restore(8,1,&s.labels);restore(9,1,&s.digital);
 window=window_create();window_set_background_color(window,GColorFromHEX(color(0)));
 Layer *root=window_get_root_layer(window);GRect bounds=layer_get_bounds(root);width=bounds.size.w;height=bounds.size.h;
 face=layer_create(bounds);layer_set_update_proc(face,draw);layer_add_child(root,face);window_stack_push(window,true);
 time_t now=time(NULL);tick(localtime(&now),SECOND_UNIT);tick_timer_service_subscribe(SECOND_UNIT,tick);
 app_message_register_inbox_received(inbox);app_message_open(256,64);
}
static void deinit(void){tick_timer_service_unsubscribe();app_message_deregister_callbacks();layer_destroy(face);window_destroy(window);}
int main(void){init();app_event_loop();deinit();}
