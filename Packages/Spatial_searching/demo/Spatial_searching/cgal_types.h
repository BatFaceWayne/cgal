#include <CGAL/Cartesian.h>
#include <CGAL/Kd_tree.h>
#include <CGAL/Search_traits_2.h>
#include <CGAL/point_generators_2.h>
#include <CGAL/algorithm.h>
#include <CGAL/Orthogonal_k_neighbor_search.h>
#include <CGAL/Euclidean_distance.h>
#include <CGAL/Fuzzy_iso_box.h>
#include <CGAL/Fuzzy_sphere.h>

#include <vector>
#include <iostream>

typedef CGAL::Cartesian<double>                         R;
typedef R::Point_2                                      Point_2;
typedef R::Segment_2                                    Segment_2;
typedef R::Iso_rectangle_2                              Iso_rectangle_2;
typedef R::FT                                           FT;
typedef R::Circle_2                                     Circle_2;

typedef CGAL::Creator_uniform_2<FT, Point_2>            Creator;
typedef CGAL::Plane_separator<FT>                       Separator;
typedef CGAL::Search_traits_2<R>             Traits;
typedef CGAL::Euclidean_distance<Traits>               Distance;
typedef CGAL::Orthogonal_k_neighbor_search<Traits>        Neighbour_search;
typedef CGAL::Fuzzy_iso_box<Traits> Fuzzy_box;
typedef CGAL::Fuzzy_sphere<Traits>                   Fuzzy_circle;

typedef std::vector<Traits::Point_d>                      Vector;
typedef std::vector<Point_2>                            Query_vector;
