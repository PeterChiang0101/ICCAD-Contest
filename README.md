# ICCAD Contest 2022 Problem E

## !!!NOT_DONE

    BUGsss

        FIXED:

        直線外擴焦點

    (DONE) 圓與直線交點斜率

    (DONE) 外擴圓心

    copper 外擴 與 絲印交點

## I/O

    i/o file location: TestingCase folder

    naming rule:    test_"NAME".txt    test_"NAME"_Ans.txt

## Outline

### struct

segment

    ```C++
    bool is_line; // 0 = arc, 1 = line
    float x1;
    float y1;
    float x2;
    float y2;
    float slope;       //斜率
    float y_intercept; //截距
    double theta;      // the angle reference to positive x axis
    // below is needed by arc, when deals with line set all to 0
    float center_x;
    float center_y;
    bool direction; // 0 = ClockWise(CW), 1 = ConterClockwise(CCW)
    ```

Point

    ```C++
    float x;
    float y;
    bool Next_Arc;
    ```

### functions

    ```C++
    float File_to_Parameter(const string); // 匯入三個絲印參數

    const vector<string> split(const string &, const char &); // 將匯入文字分解

    segment String_to_Line(const string); // 將文字轉換為struct segment
    // dependency: split()

    vector<segment> Read_Assembly(fstream &); // 將Assembly讀入
    // dependency: String_to_Line()

    vector<vector<segment>> Read_Copper(fstream &); // 將Copper讀入
    // dependency: String_to_Line()

    vector<Point> Line_to_Point(const vector<segment>); //將線段切割成點

    vector<segment> Buffer(const vector<segment>); // 繪製絲印
    // dependency: Line_to_Point(), point_in_polygon(), Arc_Optimization()

    float interpolate_x(const float, const Point, const Point);

    bool point_in_polygon(const Point, const vector<Point>, const vector<vector<Point>>);
    // dependency: interpolate_x()

    void Write_File(const vector<segment>); //匯出

    vector<segment> Arc_to_Poly(segment);

    vector<vector<Point>> Arc_Optimization(const vector<segment>);
    // dependency: Arc_to_Poly(), Line_to_Point()

    int main(); // 主程式
    ```

## Algorithms

2022/6/2

add comment to improve readability

implement Arc_to_poly() into other functions

suggestion: Arc_to_poly() can return vector of points, the vector includes start point and end point

Peter

2022/6/2

Arc_to_Poly (finish!!!!)

help me to run the PublicCases to check if it successes, plz.

Macoto

2022/6/2

Arc_to_Poly (not finish!!!!)

last point access

Macoto

2022/6/1

silkscreen successfull printed, same as copper expansion

new function

    interpolate_x()
    
    point_in_polygon()

    use them to correct the point in polygon judgement

copyright: <https://web.ntnu.edu.tw/~algo/Polygon.html>

Peter Raymond Macoto

2022/5/27

start processing arc, have some issue.(see test_A pic)

Peter Raymond

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

2022/6/1

visualizer input update, show the file list in the folder, easier to understand how to use

Peter

2022/5/29

new functions

    String_to_Line()

    Read_Assembly()

    Read_Copper()

    propose: migrate input into functions

change name function

    File_to_String() -> File_to_Parameter()

class Segment Abandoned

Peter

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
