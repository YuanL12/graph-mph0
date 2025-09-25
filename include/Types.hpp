#pragma once
#include <vector>

// Define type
using EdgeId = int;  // -1 means null edge due to removal
using VertexId = int;
using GradeID = int;
// vertex adjacency for storing its adjacent edge indices
using VAdj = std::vector<EdgeId>;
using PointCloudType = double;  // input point cloud type