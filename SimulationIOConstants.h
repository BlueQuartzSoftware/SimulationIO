/*
 * Your License should go here
 */
#pragma once

#include <QtCore/QString>

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
}

/**
* @brief Use this namespace to define any custom GUI widgets that collect FilterParameters
* for a filter. Do NOT define general reusable widgets here.
*/
namespace FilterParameterWidgetType
{
/* const QString SomeCustomWidget("SomeCustomWidget"); */
}
