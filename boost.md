# boost library
## geometry
### model

***
[boost::geometry::model website](https://www.boost.org/doc/libs/1_79_0/libs/geometry/doc/html/geometry/reference/models.html)
***

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
### algorithms
***
[boost::geometry::algorithms website](https://www.boost.org/doc/libs/1_79_0/libs/geometry/doc/html/geometry/reference/algorithms.html)
***
- area
  ```C++
  <typename Geometry>
  ```
        Geometry : the geometry object

- closest_points
  ```C++
  <typename Geometry1, typename Geometry2, typename Segment>
  ```
        Geometry1 : first geometry object
        Geometry2 : second geometry object
        Segment : segment type ex: model::segment
- distance
  ```C++
  <typename Geometry1, typename Geometry2>
  ```
        Geometry1 : first geometry object
        Geometry2 : second geometry object
- envelope
  ```C++
  <typename Geometry, typename Box>
  ```
        Geometry : the geometry object
        Box : the bounding box of the geometry
- buffer (with strategies)
  ```C++
  <typename GeometryIn, typename MultiPolygon, typename DistanceStrategy, typename SideStrategy, typename JoinStrategy, typename EndStrategy, typename PointStrategy>
  ```
        GeometryIn : input geometry object 
        MultiPolygon : output geometry object
        DistanceStrategy : distance_strategy
        SideStrategy : side_strategy
        JoinStrategy : join_strategy
        EndStrategy : end_strategy
        PointStrategy : point_strategy


        strategy::buffer::join_round
        strategy::buffer::join_miter
        strategy::buffer::end_round
        strategy::buffer::end_flat
        strategy::buffer::distance_symmetric
        strategy::buffer::distance_asymmetric
        strategy::buffer::point_circle
        strategy::buffer::point_square
        strategy::buffer::geographic_point_circle
- perimeter
  ```C++
  <typename Geometry>
  ```
      Geometry : the geometry object
### strategies
***
[boost::geometry::strategies website](https://www.boost.org/doc/libs/1_79_0/libs/geometry/doc/html/geometry/reference/strategies.html)
***
