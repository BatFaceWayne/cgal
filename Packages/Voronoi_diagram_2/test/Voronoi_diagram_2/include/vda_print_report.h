// Copyright (c) 2005 Foundation for Research and Technology-Hellas (Greece).
// All rights reserved.
//
// This file is part of CGAL (www.cgal.org); you may redistribute it under
// the terms of the Q Public License version 1.0.
// See the file LICENSE.QPL distributed with CGAL.
//
// Licensees holding a valid commercial license may use this file in
// accordance with the commercial license agreement provided with the software.
//
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//
// $Source$
// $Revision$ $Date$
// $Name$
//
// Author(s)     : Menelaos Karavelas <mkaravel@tem.uoc.gr>

#ifndef VDA_PRINT_REPORT_H
#define VDA_PRINT_REPORT_H 1

#include <CGAL/basic.h>
#include "vda_aux.h"
#include "helper_functions.h"
#include <iostream>
#include <CGAL/Voronoi_diagram_2/Accessor.h>

template<class VDA, class Projector, class Dual_primal_projector,
	 class Stream>
void print_report(const VDA& vda, const Projector& project,
		  const Dual_primal_projector& dp_project,
		  Stream& os = std::cout)
{
  const typename VDA::Voronoi_traits::Edge_degeneracy_tester& edge_tester =
    vda.voronoi_traits().edge_degeneracy_tester_object();

  const typename VDA::Voronoi_traits::Face_degeneracy_tester& face_tester =
    vda.voronoi_traits().face_degeneracy_tester_object();

  std::cout << std::endl;
  std::cout << "is Delaunay graph valid? "
	    << (vda.dual().is_valid() ? "yes" : "no") << std::endl;
  std::cout << std::endl;

  int n_all = 0, n_empty = 0, n_vert = 0;
  for (typename VDA::Face_iterator fit = vda.faces_begin();
       fit != vda.faces_end(); ++fit) {
    //    CGAL_assertion( !face_tester(vda.dual(), fit->dual_vertex()) );
    n_all++;
  }

  typename VDA::Delaunay_graph::Finite_vertices_iterator vit;
  for ( vit = vda.dual().finite_vertices_begin();
	vit != vda.dual().finite_vertices_end(); ++vit) {
    n_vert++;
    typename VDA::Delaunay_graph::Vertex_handle v(vit);
    if ( face_tester(vda.dual(), v) ) {
      n_empty++;
    }    
  }
  
  {
    unsigned int n_dual_edges = 0;
    unsigned int n_dual_fin_faces = 0;
    unsigned int n_dual_faces = 0;
    unsigned int n_edges = 0;
    unsigned int n_edge_inf = 0;
    unsigned int n_edge_fin = 0;
    unsigned int n_edge_degen = 0;
    unsigned int n_edge_non_degen = 0;
    unsigned int n_hedges = 0;
    unsigned int n_halfedges = 0;
    unsigned int n_faces = 0;
    unsigned int n_empty_faces = 0;
    unsigned int n_vertices = 0;
    unsigned int sum_deg = 0;
    unsigned int n_unbounded_faces = 0;
    unsigned int n_unbounded_faces2 = 0;

    // computing statistics in the dual (Delaunay graph)
    typename VDA::Delaunay_graph::All_edges_iterator deit;
    for (deit = vda.dual().all_edges_begin();
	 deit != vda.dual().all_edges_end(); ++deit) {
      n_dual_edges++;
      if ( edge_tester(vda.dual(), deit) ) {
	os << "degenerate edge: " << std::flush;
	print_dual_edge(vda, *deit, project, os);
	n_edge_degen++;
      } else {
	n_edge_non_degen++;
      }
    }
    os << std::endl;

    typename VDA::Delaunay_graph::Finite_faces_iterator dfit;
    for (dfit = vda.dual().finite_faces_begin();
	 dfit != vda.dual().finite_faces_end(); ++dfit) {
      n_dual_fin_faces++;
    }
    os << std::endl;

    typename VDA::Delaunay_graph::All_faces_iterator dafit;
    for (dafit = vda.dual().all_faces_begin();
	 dafit != vda.dual().all_faces_end(); ++dafit) {
      n_dual_faces++;
    }
    os << std::endl;

    typename VDA::Delaunay_graph::Finite_vertices_iterator dvit;
    for ( dvit = vda.dual().finite_vertices_begin();
	  dvit != vda.dual().finite_vertices_end(); ++dvit) {
      if ( face_tester(vda.dual(),dvit) ) {
	n_empty_faces++;
      }
    }

    // computing statistics in the primal (Voronoi diagram)
    typename VDA::Edge_iterator eit;
    for (eit = vda.edges_begin(); eit != vda.edges_end(); ++eit) {
      typename VDA::Halfedge::Delaunay_edge e = eit->dual();
      if ( vda.dual().is_infinite(e) ) {
	n_edge_inf++;
      } else {
	n_edge_fin++;
      }
      n_edges++;
    }

    typename VDA::Halfedge_iterator heit;
    for (heit = vda.halfedges_begin(); heit != vda.halfedges_end(); ++heit) {
      n_hedges++;
    }

    typename VDA::Face_iterator fit;
    for (fit = vda.faces_begin(); fit != vda.faces_end(); ++fit) {
      n_faces++;
    }

    typename VDA::Vertex_iterator vit;
    for (vit = vda.vertices_begin(); vit != vda.vertices_end(); ++vit) {
      os << "vertex (degree = " << vit->degree() << "): " << std::flush;

      typedef typename VDA::Accessor::Find_valid_vertex  Find_valid_vertex;

      typename VDA::Delaunay_graph::Face_handle fvalid =
	Find_valid_vertex()(&vda,vit->dual());
      os << dp_project(vda, fvalid) << std::endl;

      n_vertices++;
    }
    os << std::endl;

    // testing circulators
    for (typename VDA::Vertex_iterator vit = vda.vertices_begin();
	 vit != vda.vertices_end(); ++vit) {
      sum_deg += vit->degree();
    }

    if ( vda.dual().dimension() > 0 ) {
      for (typename VDA::Face_iterator fit = vda.faces_begin();
	   fit != vda.faces_end(); ++fit) {
	typename VDA::Halfedge_handle he = fit->halfedge();
	typename VDA::Ccb_halfedge_circulator hc, hc_start;
	hc = he->ccb();
	hc_start = hc;
	do {
	  hc++;
	  n_halfedges++;
	  CGAL_assertion( fit->is_halfedge_on_ccb(hc) );
	} while ( hc != hc_start );
      }
    }

    // computing number of unbounded faces
    typename VDA::Unbounded_faces_iterator ufit;
    for (ufit = vda.unbounded_faces_begin();
	 ufit != vda.unbounded_faces_end(); ++ufit) {
      CGAL_assertion( ufit->is_unbounded() );
      n_unbounded_faces++;
    }

    if ( vda.unbounded_faces_begin() != vda.unbounded_faces_end() ) {
      for (ufit = --vda.unbounded_faces_end();
	   ufit != vda.unbounded_faces_begin(); --ufit) {
	CGAL_assertion( ufit->is_unbounded() );
	n_unbounded_faces2++;
      }
      CGAL_assertion( ufit->is_unbounded() );
      n_unbounded_faces2++;
    }

    // testing calls to access unbounded/bounded faces/edges
    {
      typename VDA::Face_handle f;
      f = vda.unbounded_face();
      if ( vda.dual().dimension() < 0 ) {
	CGAL_assertion( f == typename VDA::Face_handle() );
      } else {
	CGAL_assertion( f != typename VDA::Face_handle() );
	CGAL_assertion( vda.unbounded_faces_begin() !=
			vda.unbounded_faces_end() );
      }
      f = vda.bounded_face();
      if ( f == typename VDA::Face_handle() ) {
	CGAL_assertion( vda.bounded_faces_begin() ==
			vda.bounded_faces_end() );
      } else {
	CGAL_assertion( vda.bounded_faces_begin() !=
			vda.bounded_faces_end() );
      }

      typename VDA::Halfedge_handle e;
      e = vda.unbounded_halfedge();
      if ( e == typename VDA::Halfedge_handle() ) {
	CGAL_assertion( vda.unbounded_halfedges_begin() ==
			vda.unbounded_halfedges_end() );
      } else {
	CGAL_assertion( vda.unbounded_halfedges_begin() !=
			vda.unbounded_halfedges_end() );
      }
      e = vda.bounded_halfedge();
      if ( e == typename VDA::Halfedge_handle() ) {
	CGAL_assertion( vda.bounded_halfedges_begin() ==
			vda.bounded_halfedges_end() );
      } else {
	CGAL_assertion( vda.bounded_halfedges_begin() !=
			vda.bounded_halfedges_end() );
      }
    }

    // computing statistics on the Voronoi edges:
    typename VDA::size_type n_bisectors = 0, n_rays = 0, n_segments = 0;
    for (typename VDA::Edge_iterator eit = vda.edges_begin();
	 eit != vda.edges_end(); ++eit) {
      if ( eit->is_bisector() ) { n_bisectors++; }
      else if ( eit->is_ray() ) { n_rays++; }
      else { n_segments++; }
    }

    // print out the Voronoi vertices
    os << "Voronoi vertices:" << std::endl;
    int i = 1;
    for (typename VDA::Vertex_iterator vit = vda.vertices_begin();
	 vit != vda.vertices_end(); ++vit, ++i) {
      os << i << " " << vit->point() << std::endl;
    }
    os << std::endl;


    // print out the endpoints of Voronoi edges
    os << "Voronoi edges:" << std::endl;
    for (typename VDA::Edge_iterator eit = vda.edges_begin();
	 eit != vda.edges_end(); ++eit) {
      typename VDA::Voronoi_traits::Point_2 p_src, p_trg;
      if ( eit->is_bisector() ) {
	os << "inf - inf" << std::endl;
      } else if ( eit->is_ray() ) {
	if ( eit->has_source() ) {
	  p_src = eit->source()->point();
	  os << p_src << " - inf" << std::endl;
	} else {
	  CGAL_assertion( eit->has_target() );
	  p_trg = eit->target()->point();
	  os << "inf - " << p_trg << std::endl;
	}
      } else {
	p_src = eit->source()->point();
	p_trg = eit->target()->point();
	os << p_src << " - " << p_trg << std::endl;
      }
    }
    os << std::endl;



    std::cout << "STATISTICS:" << std::endl;
    std::cout << "-----------" << std::endl;
    std::cout << "Dimension of Delaunay graph: "
	      << vda.dual().dimension() << std::endl << std::endl;
    std::cout << "# of Voronoi edges: "	<< n_edges << std::endl;
    std::cout << "# of finite Voronoi edges: "
	      << n_edge_fin << std::endl;
    std::cout << "# of infinite Voronoi edges: " << n_edge_inf << std::endl;
    std::cout << std::endl;
    std::cout << "# of connected components: "
	      << vda.number_of_connected_components() << std::endl;
    std::cout << std::endl;
    std::cout << "# of bisecting segments: " << n_segments << std::endl;
    std::cout << "# of bisecting rays:     " << n_rays << std::endl;
    std::cout << "# of full bisectors:     " << n_bisectors << std::endl;
    std::cout << std::endl;
    std::cout << "# of Voronoi halfedges: " << n_hedges << std::endl;
    std::cout << "# of Voronoi halfedges (2nd count): "
	      << n_halfedges << std::endl;
    std::cout << "# of Voronoi halfedges (3rd count): "
	      << vda.number_of_halfedges() << std::endl;
    std::cout << "# of Voronoi faces: " << n_faces << std::endl;
    std::cout << "# of Voronoi faces (2nd count): "
	      << vda.number_of_faces() << std::endl;
    std::cout << "# of unbounded faces: " << n_unbounded_faces
	      << std::endl;
    std::cout << "# of unbounded faces (2nd count): " << n_unbounded_faces2
	      << std::endl;
    //    std::cout << "# of unbounded faces (3rd count): "
    //	      << vda.dual().degree( vda.dual().infinite_vertex() )
    //	      << std::endl;
    std::cout << "# of Voronoi vertices: " << n_vertices << std::endl;
    std::cout << "# of Voronoi vertices (2nd count): "
	      << vda.number_of_vertices() << std::endl;
    std::cout << std::endl;

    std::cout << "sum of degrees of Voronoi vertices: "
	      << sum_deg << std::endl;
    std::cout << std::endl;

    std::cout << "# of Delaunay edges: " << n_dual_edges << std::endl;
    std::cout << "# of Voronoi edges in the dual: "
	      << n_dual_edges << std::endl;
    std::cout << "# of non-degenerate Voronoi edges in the dual: "
	      << n_edge_non_degen << std::endl;
    std::cout << "# of degenerate Voronoi edges in the dual: "
	      << n_edge_degen << std::endl;
    std::cout << std::endl;
    std::cout << "# of Delaunay faces (triangles): "
	      << n_dual_faces << std::endl;
    std::cout << "# of finite Delaunay faces (triangles): "
	      << n_dual_fin_faces << std::endl;
    std::cout << "# of Voronoi faces in the dual with empty interior: "
	      << n_empty_faces << std::endl;
    std::cout << std::endl;  
  }

#if 0
  std::cout << "# of dual vertices: " << n_vert << std::endl;
  std::cout << "# of Voronoi cells: " << n_all << std::endl;
  std::cout << "# of Voronoi cells with empty interior: "
	    << n_empty << std::endl;
#endif

  std::cout << "is Voronoi diagram valid? "
	    << (vda.is_valid() ? "yes" : "no") << std::endl;
}


#endif // VDA_PRINT_REPORT_H
