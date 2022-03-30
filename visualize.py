from math import atan2, degrees, sqrt
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.patches import Arc as Arc

type = []
x1 = []
x2 = []
y1 = []
y2 = []
cirx = []
ciry=[]
cir_dir = []
path = 'input.txt'
silkpath = 'output.txt'
img_out = 'output.png'

fg=plt.figure()
fg.patch.set_facecolor('k')
ax=fg.add_subplot(111)
ax.set_facecolor('k')
def new_func(type, x1, x2, y1, y2, cirx, ciry, cir_dir, f):
    for line in f.readlines():
        s=line.split(',')
        if len(s)<4:
            continue
        else:
            type.append(str(s[0]))    
            x1.append(float(s[1]))
            y1.append(float(s[2]))
            x2.append(float(s[3]))
            y2.append(float(s[4]))
            if s[0]=='arc':
                cirx.append(float(s[5]))
                ciry.append(float(s[6]))
                cir_dir.append(str(s[7]))
            else:
                cirx.append(0)
                ciry.append(0)
                cir_dir.append('0')

with open(path) as f:
    new_func(type, x1, x2, y1, y2, cirx, ciry, cir_dir, f)
type.append('end_of_input')
with open(silkpath) as g:
    new_func(type, x1, x2, y1, y2, cirx, ciry, cir_dir, g)

color='g'
for i in range(len(x1)):
    if type[i]=='end_of_input':
        type.remove('end_of_input')
        color='y'
    if type[i] == 'line':
        x=[x1[i], x2[i]]
        y=[y1[i], y2[i]]
        plt.plot(x, y, lw=1.5, c=color)
    else: #arc
        rad = sqrt((x2[i] - cirx[i])**2 + (y2[i] - ciry[i])**2)
        theta1 = degrees(atan2(y1[i]-ciry[i],x1[i]-cirx[i]))
        theta2 = degrees(atan2(y2[i]-ciry[i],x2[i]-cirx[i]))
        pac = Arc([cirx[i], ciry[i]], 2*rad, 2*rad, 0, theta1, theta2, lw=1.5, color=color)
        ax.add_patch(pac)

        
fg.canvas.draw()
fg.savefig(img_out)
plt.show()
#plt.savefig(img_out)