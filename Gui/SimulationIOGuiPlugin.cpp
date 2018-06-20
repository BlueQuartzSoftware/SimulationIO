

#include "SimulationIOGuiPlugin.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SimulationIOGuiPlugin::SimulationIOGuiPlugin()
: SimulationIOPlugin()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SimulationIOGuiPlugin::~SimulationIOGuiPlugin() = default;

#include "SimulationIO/Gui/FilterParameterWidgets/RegisterKnownFilterParameterWidgets.cpp"
