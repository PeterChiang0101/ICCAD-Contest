# ICCAD Contest 2022 Problem E

## Algorithms

!!!NOT_DONE!!!

    BUGsss

    圓與直線交點斜率

    外擴圓心

    copper 外擴 與 絲印交點

2022/5/26

fix the bugs in functions, still some weird bugs to go on. (a point is not correct)

change function

    Outside_of_Assembly() return type from Point to bool

add parameters

    Angle_Tolerance = 0.1

    PI = pi

Peter

2022/5/25

new function

    Assembly_Line_to_Point()

    change data structure from lines to points

Peter

2022/5/24

new function

    Print_Silkscreen(), Outside_of_Assembly()

Peter

## File Processing

2022/5/25

new function

    Write_File()

    can output the txt in correct format

Peter

2022/5/21

create new class

    Segment 

    replace the struct segment

Peter

---

2022/5/20

create new function

    File_to_String(string str)

use as processing the parameter of the silkscreen

Peter

---

2022/5/14

finished reading copper with 2 dimention vector.

merge read data of assembly with copper.

Macoto

---

2022/5/10

finished reading assembly.

    new function:
    const vector<string> split(const string& str, const char& delimiter)

Macoto

---

2022/5/9

Start processing of the file, finished three specs of silkscreen, working on reading assembly.

    assemblygap : float, parameter passed by problem

    coppergap : float, parameter passed by problem

    silkscreenlen : float, parameter passed by problem

    segment : struct, include lines and arcs

    assembly : vector<segment>, store the assembly details
Peter

## Optimization
