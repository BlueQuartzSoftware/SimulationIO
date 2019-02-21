# CreateFEAInputFiles #


## Group (Subgroup) ##

SimulationIO (SimulationIO)

## Description ##

This **Filter** writes the input files for different packages.

#### Packages ####

##### ABAQUS #####

##### PZFLEX #####

##### BSAM #####

## Parameters ##

| Name | Type | Description |
|------|------|------|
| FEA package | Enumeration | Package to be used |
| Output Path | Path | Path of the directory where files will be created |
| Output File Prefix | String | output file prefix |
| Job Name | String | job name, if _ABAQUS_ is chosen |
| Number of Solution Dependent Variables | int | number of solution dependent variables, if _ABAQUS_ is chosen |
| Number of Material Constants | int | number of material constants, if _ABAQUS_ is chosen |
| Number of User Output Variables | int | number of usev output variables, if _ABAQUS_ is chosen |
| Material Constants | DynamicTableData | values of material constants, if _ABAQUS_ is chosen |
| Delamination Material | String | Name of the delamination material |
| Number of Keypoints | Int | keypoints in x, y, and z dimension |

## Required Geometry ##

 Not Applicable

## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| **Feature Attribute Array** | FeatureIds | int32_t | (3) |  Specifies to which **Feature** each **Cell** belongs |
| **Feature Attribute Array** | Euler Angles | float | (3) | Three angles defining the orientation of the **Feature** |
| **Feature Attribute Array** | Phases | int32_t | (1) |  Specifies to which **Ensemble** each **Cell** belongs |

## Created Objects ##

## Example Pipelines ##

## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users