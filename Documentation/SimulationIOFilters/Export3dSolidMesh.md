# Export3dSolidMesh {#export3dsolidmesh} #


## Group (Subgroup) ##
SimulationIO (SimulationIO)

## Description ##
This **Filter** can be used to create a volume mesh of the sample using three different packages: TetGen, Netgen, and Gmsh. 

There are two ways to create a volume mesh using this filter. One of the ways is to use the surface mesh created by the filter **Quick Surface Mesh**. TetGen is used for this purpose. Another way is to use the STL files of individual grains. Gmsh or Netgen can be used for this purpose. 

DREAM.3D needs the exact location of these packages to be specified in the "Package Location" field. For example, in MacOS, if Netgen is installed in /Applications, user needs to enter the following address in the "Package Location" field:

/Applications/Netgen.app/Contents/MacOS

#### Packages ####

##### TetGen #####
Tetgen creates the volume mesh using the surface mesh created by the **Quick Surface Mesh** filter. It also needs the centroids of features which can be calculated using the "Find Feature Centroid" filter. The quality of the mesh can be controlled using the options available in the filter. The mesh files created by this filter are saved in the directory mentioned in the "Path" field. Tetgen creates a tetrahedral mesh and all the mesh related data is saved in a newly created **Data Container**. 

##### Netgen #####
Netgen is used to create a volume mesh from STL files of individual grains. All the STL files should be present in the directory mentioned in the "Path" field. "STL File Prefix" should be the same that was used for creating the STL files. First, volume mesh of each **feature** is created, followed by merging of individual meshes. File names of individual mesh files is STLFilePrefixFeature_#.vol and the file name of the merged mesh is STLFilePrefixMergedMesh.vol. All the mesh files are present in the directory mentioned in "Path" Field. User has the option of chosing the mesh quality from very coarse, coarse, moderate, fine, and very fine. 

It is required to use the filter "Reverse Triangle Winding" before creating the STL files for using Netgen flter.

##### Gmsh #####
Gmsh is used to create a volume mesh from STL files of individual grains. All the STL files should be present in the directory mentioned in the "Path" field. "STL File Prefix" should be the same that was used for creating the STL files. File name of the merged mesh is gmsh.xxx and is created in the directory mentioned in the "Path" Field. The extension of the merged mesh depends on the **Mesh File Format** that user uses. ABAQUS input file can be created from this filter by using the "inp" option.

## Parameters ##
| Name | Type | Description |
|------|------|------|
| Meshing package | Enumeration | Package to be used for creating a solid mesh |
| Path | Path | Path of the directory where the required files exist and new files will be created |
| Package Location | Path | Location of the executable |
| Refine Mesh(q) | bool | Option to set parameters for refining the mesh, if _TetGen_ is chosen|
| Maximum Radius-Edge Ratio | float | maximum radius-edge ratio, if _TetGen_ is chosen|
| Minimum Dihedral Angle | float | minimum dihedral angle, if _TetGen_ is chosen|
| Optimization Level | int | optimization level, if _TetGen_ is chosen|
| Limit Tetrahedra Volume | bool | Option to limit the volume of tetrahedrons, if _TetGen_ is chosen|
| Maximum Tetrahedron Volume | float | Maximum volume of tetrahedrons, if _TetGen_ is chosen|
| STL File Prefix | File Prefix | Prefix of STL filenames (xxxFeature_#.stl), if _Netgen_ or _Gmsh_ is chosen |
| Mesh Size | Enumeration | verycoarse/coarse/moderate/fine/veryfine, if _Netgen_ is chosen |
| Mesh File Format | Enumeration | mesh file format: msh or inp, if _Gmsh_ is chosen |

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

## Example Pipelines ##

+NetgenPipeline
+GmshPipeline
+TetGenPipeline

## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
