from email.mime import base
from math import atan2, degrees, sqrt
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.patches import Arc as Arc
import os as os
import glob
from datetime import date

type = []
x1 = []
x2 = []
y1 = []
y2 = []
cirx = []
ciry = []
cir_dir = []
# add support for both linux(RHEL) and Windows directory system(2022/6/11)
# use os.path.join function
home = os.path.join(os.getcwd(), "HiddenCase")
target = os.path.join(home, "hidden_case?.txt")
result = glob.glob(target)
i = 0
print("What testcase you want to visualize?")
for entry in result:
    i += 1
    basename = os.path.basename(entry)
    filename = os.path.splitext(basename)[0]
    print(str(i) + '. ' + filename)
choice = input("Selection (input the name after test_ ): ")

path = os.path.join(home, "hidden_case"+choice+".txt")
silkpath = os.path.join(home, "hidden_case"+choice+"_Ans.txt")
img_out = os.path.join(home, "hidden_case"+choice + "_output.png")

fg = plt.figure()
# fg.patch.set_facecolor('k')
ax = fg.add_subplot(111)
ax.set_facecolor('k')
today = date.today()
d1 = today.strftime("%d/%m/%Y")
plt.title("test_" + choice + "  " + d1, color='y')


def ReadIn(type, x1, x2, y1, y2, cirx, ciry, cir_dir, f):
    assembly_going = True
    for line in f.readlines():
        s = line.split(',')
        if len(s) < 4:
            if s[0] == "copper\n" and assembly_going:
                assembly_going = False
                type.append('end_of_assembly')
            continue
        else:
            type.append(str(s[0]))
            x1.append(float(s[1]))
            y1.append(float(s[2]))
            x2.append(float(s[3]))
            y2.append(float(s[4]))
            if s[0] == 'arc':
                cirx.append(float(s[5]))
                ciry.append(float(s[6]))
                cir_dir.append(str(s[7]))
            else:
                cirx.append(0)
                ciry.append(0)
                cir_dir.append('0')


with open(path) as f:
    ReadIn(type, x1, x2, y1, y2, cirx, ciry, cir_dir, f)
type.append('end_of_input')
with open(silkpath) as g:
    ReadIn(type, x1, x2, y1, y2, cirx, ciry, cir_dir, g)

color = 'g'
for i in range(len(x1)):
    if type[i] == 'end_of_input':
        type.remove('end_of_input')
        color = 'y'
    elif type[i] == 'end_of_assembly':
        type.remove('end_of_assembly')
        color = 'b'
    if type[i] == 'line':
        x = [x1[i], x2[i]]
        y = [y1[i], y2[i]]
        plt.plot(x, y, lw=0.5, c=color)
    else:  # arc
        rad = sqrt((x2[i] - cirx[i])**2 + (y2[i] - ciry[i])**2)
        theta1 = degrees(atan2(y1[i]-ciry[i], x1[i]-cirx[i]))
        theta2 = degrees(atan2(y2[i]-ciry[i], x2[i]-cirx[i]))
        if theta1 == theta2:
            theta2 = 360 + theta1
        if cir_dir[i] == "CW\n":
            theta1, theta2 = theta2, theta1

        pac = Arc([cirx[i], ciry[i]], 2*rad, 2*rad, 0,
                  theta1, theta2, lw=0.5, color=color)
        ax.add_patch(pac)


fg.canvas.draw()
fg.savefig(img_out, dpi=300)
plt.show()
# plt.savefig(img_out)
