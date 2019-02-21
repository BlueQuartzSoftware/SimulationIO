# ExportDAMASKFiles #


## Group (Subgroup) ##

SimulationIO (SimulationIO)

## Description ##

This **Filter** writes the geometry file (*.geom) and material.config file required as an input for DAMASK. *.geom file has the information about sample's origin, dimensions, and resolution, and spatial distribution of grains. material.config has the description of Euler Angles corresponding to feature IDs used in *.geom file.

## Parameters ##

| Name | Type | Description |
|------|------|------|
| Output Path | Path | Path of the directory where files will be created |
| Geometry File Name | Filename | Name of geometry (*.geom) file |
| Homogenization Index | int | homogenization index |

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