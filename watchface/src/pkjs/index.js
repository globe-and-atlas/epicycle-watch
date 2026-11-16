var Clay=require('@rebble/clay');
var defaults={Palette:0,Background:0,HourColor:0xFFFFFF,MinuteColor:0x55AAFF,SecondColor:0xFF5500,TimeFormat:0,DateFormat:0,Labels:1,DigitalTime:1};
function integer(value,fallback,max){
 if(value===undefined || value===null || value==='' || typeof value==='boolean')return fallback;
 var number=Number(value);
 return isFinite(number) && Math.floor(number)===number && number>=0 && number<=max?number:fallback;
}
function toggle(value,fallback){return value===true?1:value===false?0:integer(value,fallback,1);}
function normalize(raw){
 raw=raw||{};
 return {Palette:integer(raw.Palette,0,6),Background:integer(raw.Background,0,0xFFFFFF),
 HourColor:integer(raw.HourColor,0xFFFFFF,0xFFFFFF),MinuteColor:integer(raw.MinuteColor,0x55AAFF,0xFFFFFF),SecondColor:integer(raw.SecondColor,0xFF5500,0xFFFFFF),
 TimeFormat:integer(raw.TimeFormat,0,2),DateFormat:integer(raw.DateFormat,0,3),Labels:toggle(raw.Labels,1),DigitalTime:toggle(raw.DigitalTime,1)};
}
var clay=new Clay([
 {type:'heading',defaultValue:'Epicycle'},
 {type:'text',defaultValue:'One chain of three hands. Hours turn from the center, minutes from the hour tip, and seconds from the minute tip.'},
 {type:'toggle',messageKey:'Labels',label:'Numbers beside the joints',defaultValue:true},
 {type:'toggle',messageKey:'DigitalTime',label:'Digital time below the dial',defaultValue:true},
 {type:'select',messageKey:'TimeFormat',label:'Time format',defaultValue:'0',options:[
  {label:'Watch default',value:'0'},{label:'12-hour',value:'1'},{label:'24-hour',value:'2'}]},
 {type:'select',messageKey:'DateFormat',label:'Date at the top',defaultValue:'0',options:[
  {label:'Off',value:'0'},{label:'YYYY-MM-DD',value:'1'},{label:'MM/DD/YYYY',value:'2'},{label:'DD/MM/YYYY',value:'3'}]},
 {type:'heading',defaultValue:'Colors'},
 {type:'select',messageKey:'Palette',label:'Palette',defaultValue:'0',options:[
  {label:'Night',value:'0'},{label:'Paper',value:'1'},{label:'Mint',value:'2'},
  {label:'Ice',value:'3'},{label:'Graphite',value:'4'},{label:'Monochrome',value:'5'},{label:'Custom colors',value:'6'}]},
 {type:'text',defaultValue:'Choose Custom colors to use the four colors below. Numbers follow their hand colors.'},
 {type:'color',messageKey:'Background',label:'Background',defaultValue:'000000',sunlight:false,allowGray:true},
 {type:'color',messageKey:'HourColor',label:'Hour hand and number',defaultValue:'FFFFFF',sunlight:false,allowGray:true},
 {type:'color',messageKey:'MinuteColor',label:'Minute hand and number',defaultValue:'55AAFF',sunlight:false,allowGray:true},
 {type:'color',messageKey:'SecondColor',label:'Second hand and number',defaultValue:'FF5500',sunlight:false,allowGray:true},
 {type:'submit',defaultValue:'Save to watch'}
],null,{autoHandleEvents:false});
function sendSaved(){
 var raw=localStorage.getItem('clay-settings');if(!raw)return;
 try{Pebble.sendAppMessage(normalize(JSON.parse(raw)));}catch(error){console.log('Epicycle settings could not be loaded');}
}
Pebble.addEventListener('ready',sendSaved);
Pebble.addEventListener('showConfiguration',function(){Pebble.openURL(clay.generateUrl());});
Pebble.addEventListener('webviewclosed',function(event){
 if(!event || !event.response || event.response==='CANCELLED')return;
 try{clay.getSettings(event.response,false);sendSaved();}catch(error){console.log('Epicycle settings were not saved');}
});
module.exports={normalize:normalize,defaults:defaults};
