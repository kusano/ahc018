# https://github.com/Razaekel/noise-rs

import random

class PermutationTable:
  def __init__(self):
    self.values = list(range(256))
    random.shuffle(self.values)

  def hash(self, A):
    x = 0
    for a in A:
      x = self.values[x]^(a&0xff)
    return self.values[x]

def gradient_dot_v(h, x, y):
  h &= 3
  if h==0:
    return x+y
  elif h==1:
    return -x+y
  elif h==2:
    return x-y
  elif h==3:
    return -x-y

def quintic(x):
    return x*x*x*(x*(x*6.-15.)+10.)

def noise(x, y, perm):
  fx = int(x)
  fy = int(y)
  dx = x-fx
  dy = y-fy

  G = []
  for ox, oy in [(0, 0), (1, 0), (0, 1), (1, 1)]:
    G += [gradient_dot_v(perm.hash([fx+ox, fy+oy]), dx-ox, dy-oy)]
  g00, g10, g01, g11 = G

  u = quintic(dx)
  v = quintic(dy)

  k0 = g00
  k1 = g10-g00
  k2 = g01-g00
  k3 = g00+g11-g10-g01

  v = k0+k1*u+k2*v+k3*u*v
  return max(-1., min(1., v*2**.5))

N = 200

from PIL import Image, ImageDraw, ImageFont

im = Image.new("RGB", (1536, 1024), (230, 240, 250))

def draw(S, x, y, mn, mx):
  for yy in range(N):
    for xx in range(N):
      v = S[yy][xx]
      v = (v-mn)/(mx-mn)
      v = max(0, min(255, int(v*256)))
      v = 255-v
      im.putpixel((x+xx*2  , y+yy*2  ), (v, v, v))
      im.putpixel((x+xx*2+1, y+yy*2  ), (v, v, v))
      im.putpixel((x+xx*2  , y+yy*2+1), (v, v, v))
      im.putpixel((x+xx*2+1, y+yy*2+1), (v, v, v))

random.seed(1234)

f0 = random.uniform(2., 8.)
f0 = 8.
dx0 = random.uniform(0., 1.)
dy0 = random.uniform(0., 1.)
seed0 = PermutationTable()
S0 = [[0.]*N for _ in range(N)]
for y in range(N):
  for x in range(N):
    S0[y][x] = noise(f0*x/N+dx0, f0*y/N+dy0, seed0)
draw(S0, 56, 56, -1., 1.)

f1 = random.uniform(10., 20.)
f1 = 20.
dx1 = random.uniform(0., 1.)
dy1 = random.uniform(0., 1.)
seed1 = PermutationTable()
S1 = [[0.]*N for _ in range(N)]
for y in range(N):
  for x in range(N):
    S1[y][x] = 0.2*noise(f1*x/N+dx1, f1*y/N+dy1, seed1)
draw(S1, 568, 56, -1., 1.)

S = [[0.]*N for _ in range(N)]
for y in range(N):
  for x in range(N):
    S[y][x] = S0[y][x]+S1[y][x]
draw(S, 1080, 56, -1.2, 1.2)

import math

for y in range(N):
  for x in range(N):
    S[y][x] = 1./(1.+math.exp(-3.*(S[y][x]-.25)))
draw(S, 56, 568, 0., 1.)

p = random.uniform(2., 4.)
p = 4.
for y in range(N):
  for x in range(N):
    S[y][x] = math.pow(S[y][x], p)
draw(S, 568, 568, 0., 1.)

l = 1e10
u = -1e10
for y in range(N):
  for x in range(N):
    l = min(l, S[y][x])
    u = max(u, S[y][x])
for y in range(N):
  for x in range(N):
    S[y][x] = (S[y][x]-l)*(5000-10)/(u-l)+10
draw(S, 1080, 568, 10., 5000.)

font = ImageFont.truetype('NotoMono-Regular.ttf', 32)

draw = ImageDraw.Draw(im)
text = f"{f0=:.4f}, {f1=:.4f}, {p=:.4f}"
draw.text((8, 8), text, (20, 40, 60), font=font)

im.save("output.png")
