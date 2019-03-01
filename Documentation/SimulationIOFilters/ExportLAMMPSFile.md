# ExportLAMMPSFile #


## Group (Subgroup) ##

SimulationIO (SimulationIO)

## Description ##

This **Filter** is used to create an atomistic representation of microstructure from image data. The data file produced by this **filter** can be used for initializing the atomic coordinates in LAMMPS package.

This **filter** should be used in conjunction with another **filter** named "Insert Atoms". Given a microstructure, the "Insert Atoms" filter follows the orientation of different features to insert atoms in them and saves the configuration of atoms in a **Vertex Data Container**. The "Export LAMMPS Filter" uses this **Vertex Data Container** to create an input file for LAMMPS.  

## Parameters ##

| Name | Type | Description |
|------|------|------|
| LAMMPS File | Filename | Name of the data file |

## Required Geometry ##

Not Applicable

## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| **Feature Attribute Array** | Atom Feature Labels | int32_t | (1) |  Specifies to which **Feature** each atom belongs |

## Created Objects ##


## Example Pipelines ##


## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users