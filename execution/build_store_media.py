"""Capture native motion through six palettes; prepare ordered store assets."""
from pathlib import Path
import argparse,json,shutil,time
from PIL import Image
from capture_native import ROOT,OUT,command,capture,monitors,close,cases

def main():
 parser=argparse.ArgumentParser();parser.add_argument('--platform',choices=['emery','gabbro'],required=True);a=parser.parse_args();platform=a.platform
 store=ROOT/'media/store';store.mkdir(exist_ok=True)
 for port in monitors().values():close(port)
 command(['pebble','kill'])
 install=['pebble','install','--emulator',platform,str(OUT/'fixture.pbw')]
 command(install);command(install)
 keys=json.loads((OUT/'message_keys.json').read_text());uuid=json.loads((ROOT/'watchface/package.json').read_text())['pebble']['uuid']
 def send(values):command(['pebble','send-app-message','--emulator',platform,'--app-uuid',uuid,'--int']+[f'{keys.get(k,k)}={v}' for k,v in values.items()])
 frames=[]
 existing_gif=store/f'{platform}_01_motion-colors.gif'
 if existing_gif.exists():
  with Image.open(existing_gif) as old:
   for index in range(18):old.seek(index);frames.append(old.convert('RGB').copy())
 for index in range(18 if frames else 0,36):
  palette=index//6;seconds=index*2
  values={**dict(cases()[0][1]),'Palette':palette,'Labels':1 if palette<3 else 0,11000:10,11001:10+seconds//60,11002:seconds%60,11022:1}
  send(values)
  for attempt in range(30):
   im=capture();w=im.width
   if im.getpixel((w//2-3,20))==(255,0,255):
    assert im.getpixel((w//2+3,20))==(0,255,0);break
   time.sleep(.1)
  else:raise AssertionError('Fixture identity missing')
  send({11022:0})
  for attempt in range(30):
   im=capture(170 if palette==4 else 255)
   if im.getpixel((w//2-3,20))!=(255,0,255):break
   time.sleep(.1)
  else:raise AssertionError('Marker failed to clear')
  frames.append(im.copy());print(platform,index,flush=True)
 target=store/f'{platform}_01_motion-colors.gif'
 frames[0].save(target,save_all=True,append_images=frames[1:],duration=500,loop=0,disposal=2,optimize=False)
 with Image.open(target) as gif:assert gif.n_frames==36 and gif.size==frames[0].size
 for i,case in enumerate(['default','palette-1','palette-2','clean'],2):shutil.copy2(ROOT/f'media/{platform}-{case}.png',store/f'{platform}_{i:02d}_{case}.png')
 command(['pebble','kill'])
if __name__=='__main__':main()
