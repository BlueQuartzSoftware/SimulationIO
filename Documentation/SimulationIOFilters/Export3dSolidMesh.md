Export3dSolidMesh {#export3dsolidmesh}
=============

## Group (Subgroup) ##
SimulationIO (SimulationIO)

## Description ##
This **Filter** can be used to create a volume mesh of the sample using three different packages: TegGen, Netgen, and Gmsh.

#### Packages ####

##### TetGen #####

##### Netgen #####


##### Gmsh #####


## Parameters ##
| Name | Type | Description |
|------|------|------|
| Meshing package | Enumeration | Package to be used for creating a solid mesh |
| Path | Path | Path of the directory where the required files exists and new files will be created |
| Package Location | Path | Location of the executable |
| Refine Mesh(q) | bool | Option to set parameters for refining the mesh, if _TetGen_ is chosen|
| Maximum Radius-Edge Ratio | float | maximum radius-edge ratio, if _TetGen_ is chosen|
| Minimum Dihedral Angle | float | minimum dihedral angle, if _TetGen_ is chosen|
| Optimization Level | int | optimization level, if _TetGen_ is chosen|
| Limit Tetrahedra Volume | bool | Option to limit the volume of tetrahedrons, if _TetGen_ is chosen|
| Maximum Tetrahedron Volume | float | Maximum volume of tetrahedrons, if _TetGen_ is chosen|
| STL File Prefix | File Prefix | Prefix of STL filenames (xxxFeature_#.stl), if _Netgen_ or _Gmsh_ is chosen |
| Mesh Size | String | verycoarse/coarse/moderate/fine/veryfine, if _Netgen_ is chosen |

## Required Geometry ##
 Not Applicable

## Required Objects ##
| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| **Face Attribute Array** | Face Labels | int32_t | (2) | Specifies which **Features** are on either side of each **Face**, if _TetGen_ is chosen |
| **Feature Attribute Array** | Euler Angles | float | (3) | Three angles defining the orientation of the **Feature** |
| **Feature Attribute Array** | Phases | int32_t | (1) |  Specifies to which **Ensemble** each **Cell** belongs if _TetGen_ is chosen |
| **Feature Attribute Array** | Feature Centroids | float | (3) | Centroid of each **Feature**, if _TetGen_ is chosen |

## Created Objects ##
| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| **Data Container** | TetrahedralDataContainer | N/A | N/A | Created **Data Container** with a **Tetrahedral Geometry**,  if _TetGen_ is chosen |
| **Attribute Matrix** | VertexData | Vertex | N/A | Created **Vertex Attribute Matrix** name, if _TetGen_ is chosen  |
| **Attribute Matrix** | CellData | Cell | N/A | Created **Cell Attribute Matrix** name, if _TetGen_ is chosen  |

## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users