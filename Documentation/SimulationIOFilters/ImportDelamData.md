# ImportDelamData #

## Group (Subgroup) ##

SimulationIO (SimulationIO)

## Description ##

This **Filter** imports data from a Bvid and a Bvid stdout text file and creates a rectilinear geometry out of it.

## Parameters ##

| Name | Type | Description |
|------|------|------|
| Bvid File | Input File | The path to the Bvid file |
| Bvid StdOut File | Input File | The path to the Bvid stdout file |
| Interface Thickness | Float | The interface thickness for the data |

## Required Geometry ##

Not Applicable

## Required Objects ##

Not Applicable

## Created Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| **Data Container** | Data Container | RectGrid | N/A | The rectilinear grid data container |
| **Attribute Matrix** | Attribute Matrix | Cell | (1) | The Cell attribute matrix to store the data |
| **Element/Feature/Ensemble/etc. Attribute Array** | Data Array | float | (1) | The array that stores the rect grid data |

## Example Pipelines ##

None.

## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users