/*
 * Your License should go here
 */
#pragma once

#include <QtCore/QString>

#define DREAM3D_STRING QString

/**
 * @brief This namespace is used to define some Constants for the plugin itself.
 */
namespace SimulationIOConstants
{
const QString SimulationIOPluginFile("SimulationIOPlugin");
const QString SimulationIOPluginDisplayName("SimulationIO");
const QString SimulationIOBaseName("SimulationIO");

namespace FilterGroups
{
const QString SimulationIOFilters("SimulationIO");
}

namespace DEFORMData
{
const DREAM3D_STRING StrokeX("Stroke-X");
const DREAM3D_STRING StrokeY("Stroke-Y");
const DREAM3D_STRING EffStrain("Eff. Strain");
const DREAM3D_STRING EffStrainRate("Eff. Strain Rate");
const DREAM3D_STRING RelativeDensity("Relative Density");
const DREAM3D_STRING DamageFactor("Damage Factor");
const DREAM3D_STRING Temperature("Temperature");
const DREAM3D_STRING XRStress("X_R Stress");
const DREAM3D_STRING YZStress("Y_Z Stress");
const DREAM3D_STRING ZThetaStress("Z_Theta Stress");
const DREAM3D_STRING XYRZStress("XY_RZ Stress");
const DREAM3D_STRING MeanStress("Mean Stress");
const DREAM3D_STRING EffStress("Effective Stress");
const DREAM3D_STRING MaxPrincStress("Maximum Principle Stress.");
const DREAM3D_STRING XRStrain("X_R Strain");
const DREAM3D_STRING YZStrain("Y_Z Strain");
const DREAM3D_STRING ZThetaStrain("Z_Theta Strain");
const DREAM3D_STRING XYRZStrain("XY_RZ Strain");
const DREAM3D_STRING MaxPrincStrain("Max prn strain");
const DREAM3D_STRING XRStrainRate("X_R Str-rt");
const DREAM3D_STRING YZStrainRate("Y_Z Str-rt");
const DREAM3D_STRING ZThetaStrainRate("Z_Theta Str-rt");
const DREAM3D_STRING XYRZStrainRate("XY_RZ Str-rt");
const DREAM3D_STRING MaxPrincStrainRate("Max prn str-rt");
const DREAM3D_STRING Step("Step #");
const DREAM3D_STRING Time("Time");
const DREAM3D_STRING RXCoord("R_X Coord.");
const DREAM3D_STRING ZYCoord("Z_Y Coord.");
const DREAM3D_STRING PointNum("Point #");
} // namespace DEFORMData

} // namespace SimulationIOConstants

/**
 * @brief Use this namespace to define any custom GUI widgets that collect FilterParameters
 * for a filter. Do NOT define general reusable widgets here.
 */
namespace FilterParameterWidgetType
{
/* const QString SomeCustomWidget("SomeCustomWidget"); */
}
