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

  // Load a general surface mesh from file
  std::unique_ptr<SurfaceMesh> mesh;
  std::unique_ptr<VertexPositionGeometry> geometry;

  // load the mesh, constructing connectivity & geometry
  std::tie(mesh, geometry) = readSurfaceMesh(args::get(inputFilename));

  // alternately, load a mesh which is required to be manifold
  // std::tie(mesh, geometry) = readManifoldSurfaceMesh(args::get(inputFilename));

  // print some information about the mesh
  mesh->printStatistics();

  // Iterate through the vertices of the mesh, printing the degree of each and the incident faces
  for (Vertex v : mesh->vertices()) {
    std::cout << "Vertex " << v << " has degree " << v.degree() << "\n";
    for (Face fn : v.adjacentFaces()) {
      std::cout << "  incident on face " << fn << "\n";
    }
  }


  // visualize the mesh with polyscope
  polyscope::init();
  polyscope::registerSurfaceMesh("my mesh", geometry->inputVertexPositions, mesh->getFaceVertexList());
  polyscope::show();

  return EXIT_SUCCESS;
}
