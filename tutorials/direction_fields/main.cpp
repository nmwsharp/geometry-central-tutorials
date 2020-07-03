#include "geometrycentral/surface/direction_fields.h"
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
  std::tie(mesh, geometry) = readManifoldSurfaceMesh(args::get(inputFilename));

  polyscope::init();
  auto* psMesh =
      polyscope::registerSurfaceMesh("input mesh",
          geometry->inputVertexPositions, 
          mesh->getFaceVertexList());

  // Tell polyscope about vertex tangent spaces
  geometry->requireVertexTangentBasis();
  VertexData<Vector3> vBasisX(*mesh);
  for (Vertex v : mesh->vertices()) {
    vBasisX[v] = geometry->vertexTangentBasis[v][0];
  }
  psMesh->setVertexTangentBasisX(vBasisX);


  VertexData<Vector2> field = computeSmoothestVertexDirectionField(*geometry);
  psMesh->addVertexIntrinsicVectorQuantity("vectors", field);

  polyscope::show();

  return EXIT_SUCCESS;
}
