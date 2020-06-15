# CreateMultiOnScaleTableFile #

## Group (Subgroup) ##

SimulationIO (SimulationIO)

## Description ##

This **Filter** writes the input files for OnScale (PZFLEX).

This **filter** assumes that the finite element discretization is equal to the discretization of the image. Thus, the number of finite elements in each direction are equal to the number of cells. Please try **Export 3D Solid Mesh** filter to make mesh that is different from the distribution of cells in an image.

This **filter** writes out PZFLEX's input file (*.flxtbl). The required input for this option is *Feature IDs* and the names of different features, and the output consists of a header, nodal coordinates and a description of spatial distribution of features. Feature Names can be created using **Create Ensemble Info** **filter**. 

This filter will take in multiple DataContainers with a Cell AttributeMatrix that has an int32_t DataArray for feature ids. It will generate a .flxtbl file for each DataContainer. It is equivalent to running the CreateOnScaleTableFile on each of the selected DataContainers individually.

## Parameters ##

| Name | Type | Description |
|------|------|------|
| Output Path | Path | path of the directory where files will be created |
| Data Container Prefix | String | Prefix for which DataContainers will be selected |
| Matrix Name | String | Name of AttributeMatrix that will be used from the DataContainer |
| Array Name | String | Name of the DataArray that will be used from the AttributeMatrix |
| Number of Keypoints | Int | keypoints in x, y, and z dimension |
| Selected Arrays | String | Displays list of arrays that are of type int32_t in a Cell AttributeMatrix in a DataContainer with an ImageGeom. Each array specifies to which **Feature** each **Cell** belongs |


## Required Geometry ##

Image

## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| **Ensemble Attribute Array** | Phase Names | String | (1) |  Specifies names of different features |

## Created Objects ##

## Example Pipelines ##
+pzflexPipeline

## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users
