#include "geometrycentral/surface/manifold_surface_mesh.h"
#include "geometrycentral/surface/meshio.h"
#include "geometrycentral/surface/surface_mesh.h"

#include "polyscope/polyscope.h"
#include "polyscope/surface_mesh.h"

#include "args/args.hxx"

using namespace geometrycentral;
using namespace geometrycentral::surface;

int main(int argc, char** argv) {

  // Configure a simple argument parser
  // clang-format off
  args::ArgumentParser parser("");
  args::HelpFlag help(parser, "help", "Display this help message", {'h', "help"});

  args::Positional<std::string> inputFilename(parser, "mesh", "A surface mesh file (see geometry-central docs for valid formats)");
  // clang-format on

  // Parse args
  try {
    parser.ParseCLI(argc, argv);
  } catch (args::Help) {
    std::cout << parser;
    return 0;
  } catch (args::ParseError e) {
    std::cerr << e.what() << std::endl;
    std::cerr << parser;
    return 1;
  }

  // Make sure a mesh name was given
  if (!inputFilename) {
    std::cout << parser;
    return EXIT_FAILURE;
  }

  // Load a manifold surface mesh from file
  std::unique_ptr<ManifoldSurfaceMesh> mesh;
  std::unique_ptr<VertexPositionGeometry> geometry;
  std::tie(mesh, geometry) = readManifoldSurfaceMesh(args::get(inputFilename));

  polyscope::init();
  polyscope::registerSurfaceMesh("input mesh", geometry->inputVertexPositions, mesh->getFaceVertexList());

  VertexData<bool> isOrigVert(*mesh, true);
  EdgeData<bool> isOrigEdge(*mesh, true);

  // Split all edges
  std::vector<Edge> toFlip;
  for (Edge e : mesh->edges()) {
    if (!isOrigEdge[e]) continue;

    Vertex oldA = e.halfedge().tipVertex();
    Vertex oldB = e.halfedge().tailVertex();
    Vector3 oldAPos = geometry->inputVertexPositions[oldA];
    Vector3 oldBPos = geometry->inputVertexPositions[oldB];

    // split the edge
    Vertex newV = mesh->splitEdgeTriangular(e).vertex();
    isOrigVert[newV] = false;

    // position the new vertex
    Vector3 newPos = 0.5 * (oldAPos + oldBPos);
    geometry->inputVertexPositions[newV] = newPos;

    // make the new edges
    for (Edge e : newV.adjacentEdges()) {
      isOrigEdge[e] = false;
      Vertex otherV = e.otherVertex(newV);
      if (isOrigVert[otherV] && otherV != oldA && otherV != oldB) {
        toFlip.push_back(e);
      }
    }
  }

  // Flip edges connecting old to new
  for (Edge e : toFlip) {
    mesh->flip(e);
  }

  auto* psMesh =
      polyscope::registerSurfaceMesh("subdiv mesh", geometry->inputVertexPositions, mesh->getFaceVertexList());



  polyscope::show();

  return EXIT_SUCCESS;
}
