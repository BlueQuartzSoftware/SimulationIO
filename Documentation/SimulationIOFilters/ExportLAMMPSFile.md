# ExportLAMMPSFile #


## Group (Subgroup) ##

SimulationIO (SimulationIO)

## Description ##

This **Filter** is used to create an atomistic representation of microstructure from image data. The data file produced by this **filter** can be used for initializing the atomic coordinates in LAMMPS package.

## Parameters ##

| Name | Type | Description |
|------|------|------|
| LAMMPS File | Filename | Name of the data file |

## Required Geometry ##

Not Applicable

## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| **Feature Attribute Array** | FeatureIds | int32_t | (3) |  Specifies to which **Feature** each **Cell** belongs |
| **Feature Attribute Array** | Euler Angles | float | (3) | Three angles defining the orientation of the **Feature** |

## Created Objects ##


## Example Pipelines ##


## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users