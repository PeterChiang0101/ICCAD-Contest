# boost library
## geometry
### model
- point
  ```C++
  <typename CoordinateType, std::size_t DimensionCount, typename CoordinateSystem>
  ```
- d2::point_xy
  ```C++
  <typename CoordinateType, typename CoordinateSystem>
  ```
- d3::point_xyz
  ```C++
  <typename CoordinateType, typename CoordinateSystem>
  ```
        CoordinateType : 資料型態 int, float, double, ...

        DimensionCount : n維空間 usually 2 or 3

        CoordinateSystem : cartesian, spherical, ...

- linestring
  ```C++
  <typename Point, template< typename, typename > class Container, template< typename > class Allocator>
  ```
        Point : point, point_xy, point_xyz, ...

        Container : vector, deque, ...

        Allocator : allocator
  
- polygon
  ```C++
  <typename Point, bool ClockWise, bool Closed, template< typename, typename > class PointList, template< typename, typename > class RingList, template< typename > class PointAlloc, template< typename > class RingAlloc>
  ```
        Point : point, point_xy, point_xyz, ...

        ClockWise : default true

        Closed : if it circled back, default true

        PointList : container type for points, vector, deque, ...

        RingList :　container type for inner rings, vector, deque, ...

        PointAlloc :　allocator

        RingAlloc : allocator

- multi_point
  ```C++
  <typename Point, template< typename, typename > class Container, template< typename > class Allocator>
  ```
        Point : point, point_xy, point_xyz, ...

        Container : vector, deque, ...

        Allocator : allocator

- multi_linestring
  ```C++
  <typename LineString, template< typename, typename > class Container, template< typename > class Allocator>
  ```
        LineString : linestring

        Container : vector, deque, ...

        Allocator : allocator
- multi_polygon
  ```C++
  <typename Polygon, template< typename, typename > class Container, template< typename > class Allocator>
  ```
        Polygon : polygon

        Container : vector, deque, ...

        Allocator : allocator
- box
    ```C++
    <typename Point>
    ```

        Point : point, point_xy, point_xyz, ...
- ring
  ```C++
  <typename Point, bool ClockWise, bool Closed, template< typename, typename > class Container, template< typename > class Allocator>
  ```
        Point : point, point_xy, point_xyz, ...

        ClockWise : default true

        Closed : if it circled back, default true

        Container : vector, deque, ...

        Allocator : allocator
- segment
    ```C++
    <typename Point>
    ```

        Point : point, point_xy, point_xyz, ...
- referring_segment
    ```C++
    <typename ConstOrNonConstPoint>
    ```

        ConstOrNotConstPoint : point, point_xy, point_xyz, ...