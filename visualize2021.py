from email.mime import base
from math import atan2, degrees, sqrt
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.patches import Arc as Arc
import os as os
import glob
from datetime import date

# add support for both linux(RHEL) and Windows directory system(2022/6/11)
# use os.path.join function
home = os.path.join(os.getcwd(), "2021_testcase")
target = os.path.join(home, "q?.txt")
result = glob.glob(target)
i = 0
print("What testcase you want to visualize?")
for entry in result:
    i += 1
    basename = os.path.basename(entry)
    filename = os.path.splitext(basename)[0]
    print(str(i) + '. ' + filename)
choice = input("Selection (input the name after q ): ")

path = os.path.join(home, "q"+choice+".txt")
# silkpath = os.path.join(home, "q"+choice+"_Ans.txt")
img_out = os.path.join(home, "q"+choice + "_output.png")

fg = plt.figure()
# fg.patch.set_facecolor('k')
ax = fg.add_subplot(111)
ax.set_facecolor('k')
today = date.today()
d1 = today.strftime("%d/%m/%Y")
plt.title("q" + choice + "  " + d1, color='y')

with open(path) as f:
    for line in f.readlines():
        s = line.split(',')
        if len(s) < 4:
            if s[0] == "drill\n":
                color = 'y'
            elif s[0] == "smd\n":
                color = 'r'
            elif s[0] == "pth\n":
                color = 'c'
            continue
        else:
            type = (str(s[0]))
            x1 = (float(s[1]))
            y1 = (float(s[2]))
            x2 = (float(s[3]))
            y2 = (float(s[4]))
            if s[0] == 'arc':
                cirx = (float(s[5]))
                ciry = (float(s[6]))
                cir_dir = (str(s[7]))
            else:
                cirx = (0)
                ciry = (0)
                cir_dir = ('0')
            if type == 'line':
                plt.plot([x1, x2], [y1, y2], lw=0.5, color=color)
            else:
                rad = sqrt((x2 - cirx)**2 + (y2 - ciry)**2)
                theta1 = degrees(atan2(y1-ciry, x1-cirx))
                theta2 = degrees(atan2(y2-ciry, x2-cirx))
                if theta1 == theta2:
                    theta2 = 360 + theta1
                if cir_dir == "CW\n":
                    theta1, theta2 = theta2, theta1

                pac = Arc([cirx, ciry], 2*rad, 2*rad, 0,
                          theta1, theta2, lw=0.5, color=color)
                ax.add_patch(pac)
fg.canvas.draw()
fg.savefig(img_out, dpi=300)
plt.show()
# plt.savefig(img_out)
