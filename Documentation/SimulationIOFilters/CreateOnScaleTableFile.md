# CreateOnScaleTableFile #

## Group (Subgroup) ##

SimulationIO (SimulationIO)

## Description ##

This **Filter** writes the input files for OnScale (PZFLEX).

This **filter** assumes that the finite element discretization is equal to the discretization of the image. Thus, the number of finite elements in each direction are equal to the number of cells. Please try **Export 3D Solid Mesh** filter to make mesh that is different from the distribution of cells in an image.

This **filter** writes out PZFLEX's input file (*.flxtbl). The required input for this option is *Feature IDs* and the names of different features, and the output consists of a header, nodal coordinates and a description of spatial distribution of features. Feature Names can be created using **Create Ensemble Info** **filter**. 

## Parameters ##

| Name | Type | Description |
|------|------|------|
| Output Path | Path | path of the directory where files will be created |
| Output File Prefix | String | output file prefix |
| Number of Keypoints | Int | keypoints in x, y, and z dimension, if _PZFLEX_ is chosen |


## Required Geometry ##

 Not Applicable

## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| **Feature Attribute Array** | FeatureIds | int32_t | (1) |  Specifies to which **Feature** each **Cell** belongs |
| **Ensemble Attribute Array** | Phase Names | String | (1) |  Specifies names of different features |

## Created Objects ##

## Example Pipelines ##
+pzflexPipeline

## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users
