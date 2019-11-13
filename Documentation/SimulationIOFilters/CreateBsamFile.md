# CreateBsamFile #

## Group (Subgroup) ##

SimulationIO (SimulationIO)

## Description ##

This **Filter** writes the input files for BSAM. 

This **filter** assumes that the finite element discretization is equal to the discretization of the image. Thus, the number of finite elements in each direction are equal to the number of cells. Please try **Export 3D Solid Mesh** filter to make mesh that is different from the distribution of cells in an image.

This **filter** writes out the mesh related data in BSAM's input file. It will read in the mesh files provided by the user, and write their content to the input (*.in) file. User should specify the number of clusters and also provide a mesh file in *.ele format for each cluster. The mesh files should be named as <OutputFilePrefix_Cluster#.ele>, where # corresponds to the cluster number and placed in the directory specified in **Output Path**.

## Parameters ##

| Name | Type | Description |
|------|------|------|
| Output Path | Path | path of the directory where files will be created |
| Output File Prefix | String | output file prefix |
| Number of Clusters | Int | number of clusters |

## Required Geometry ##

Not Applicable

## Required Objects ##

Not Applicable

## Created Objects ##

## Example Pipelines ##
+pzflexPipeline

## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users
