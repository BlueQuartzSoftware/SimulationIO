# ExportDAMASKFiles #


## Group (Subgroup) ##

SimulationIO (SimulationIO)

## Description ##

This **Filter** writes the geometry file (*.geom) and material.config file required as an input for the DAMASK package. The geometry file has information about sample's origin, size, and dimensions, and spatial distribution of grains. The material.config file has description of Euler Angles and phase IDs. The material.config file created by DREAM.3D has information about texture and microstructure, however, the user needs to add information about homogenization, crystallite and phase.

There are two options for writing the files: poitwise and grainwise. In the pointwise case, every cell is considered to be a different grain. In the grainwise case, **Feature ID** is used to specify the feature to which a cell belongs.

## Parameters ##

| Name | Type | Description |
|------|------|------|
| Data Format | Enumeration | format type for DAMASK files |
| Output Path | Path | Path of the directory where files will be created |
| Geometry File Name | String | Name of geometry (*.geom) file |
| Homogenization Index | int | Homogenization index |
| Compress Geom File | bool | Option to chose between compressed and uncompressed versions of *.geom file |

## Required Geometry ##

Not Applicable

## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| **Feature Attribute Array** | FeatureIds | int32_t | (1) |  Specifies to which **Feature** each **Cell** belongs |
| **Feature Attribute Array** | Euler Angles | float | (3) | Three angles defining the orientation of the **Feature** |
| **Feature Attribute Array** | Phases | int32_t | (1) |  Specifies to which **Ensemble** each **Cell** belongs |

## Created Objects ##


## Example Pipelines ##

+DAMASKPipeline

## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users