#include "AbaqusFileWriter.h"

#include <QtCore/QDir>
#include <QtCore/QTextStream>

#include "SIMPLib/Math/SIMPLibMath.h"

#include "SimulationIO/SimulationIOFilters/Utility/EntriesHelper.h"

bool AbaqusFileWriter::write(const ImageGeom& imageGeom, const DataArray<int32_t>& featureIds, const DataArray<int32_t>& cellPhases, const DataArray<float>& cellEulerAngles,
                             const DynamicTableData& matConst, const QString& outputPath, const QString& filePrefix, const QString& jobName, int32_t numDepvar, int32_t numMatConst,
                             int32_t numUserOutVar)
{
  SizeVec3Type dims = imageGeom.getDimensions();
  FloatVec3Type spacing = imageGeom.getSpacing();
  FloatVec3Type origin = imageGeom.getOrigin();

  auto featureIdsData = featureIds.getPointer(0);
  auto cellPhasesData = cellPhases.getPointer(0);
  auto cellEulerAnglesData = cellEulerAngles.getPointer(0);

  // Find total number of grain ids

  auto maxElement = std::max_element(featureIds.begin(), featureIds.end());

  if(maxElement == featureIds.end())
  {
    return false;
  }

  int32_t maxGrainId = *maxElement;

  // Create files

  QString nodesFileName = filePrefix + "_nodes.inp";
  QString elemsFileName = filePrefix + "_elems.inp";
  QString sectsFileName = filePrefix + "_sects.inp";
  QString elsetFileName = filePrefix + "_elset.inp";
  QString masterFileName = filePrefix + ".inp";

  QString nodesFilePath = outputPath + QDir::separator() + nodesFileName;
  QString elemsFilePath = outputPath + QDir::separator() + elemsFileName;
  QString sectsFilePath = outputPath + QDir::separator() + sectsFileName;
  QString elsetFilePath = outputPath + QDir::separator() + elsetFileName;
  QString masterFilePath = outputPath + QDir::separator() + masterFileName;

  QFile nodesFile(nodesFilePath);

  if(!nodesFile.open(QIODevice::OpenModeFlag::WriteOnly))
  {
    QString ss = QObject::tr("Error writing ABAQUS nodes file '%1'").arg(nodesFilePath);
    return false;
  }

  QFile elemsFile(elemsFilePath);

  if(!elemsFile.open(QIODevice::OpenModeFlag::WriteOnly))
  {
    QString ss = QObject::tr("Error writing ABAQUS connectivity file '%1'").arg(elemsFilePath);
    return false;
  }

  QFile sectsFile(sectsFilePath);

  if(!sectsFile.open(QIODevice::OpenModeFlag::WriteOnly))
  {
    QString ss = QObject::tr("Error writing ABAQUS sections file '%1'").arg(sectsFilePath);
    return false;
  }

  QFile elsetFile(elsetFilePath);

  if(!elsetFile.open(QIODevice::OpenModeFlag::WriteOnly))
  {
    QString ss = QObject::tr("Error writing ABAQUS element file '%1'").arg(elsetFilePath);
    return false;
  }

  QFile masterFile(masterFilePath);

  if(!masterFile.open(QIODevice::OpenModeFlag::WriteOnly))
  {
    QString ss = QObject::tr("Error writing ABAQUS input file '%1'").arg(masterFilePath);
    return false;
  }

  QTextStream masterStream(&masterFile);

  masterStream << "*Heading\n";
  masterStream << jobName << "\n";
  masterStream << QString("** Job name : %1\n").arg(jobName);
  masterStream << "*Preprint, echo = NO, model = NO, history = NO, contact = NO\n";
  masterStream << "**\n";
  masterStream << QString("*Include, Input = %1\n").arg(nodesFileName);
  masterStream << QString("*Include, Input = %1\n").arg(elemsFileName);
  masterStream << QString("*Include, Input = %1\n").arg(sectsFileName);
  masterStream << QString("*Include, Input = %1\n").arg(elsetFileName);
  masterStream << "**\n";

  int32_t ne_x = dims[0];
  int32_t ne_y = dims[1];
  int32_t ne_z = dims[2];

  int32_t nnode_x = ne_x + 1;
  int32_t nnode_y = ne_y + 1;
  int32_t nnode_z = ne_z + 1;

  QTextStream nodesStream(&nodesFile);

  nodesStream << "*NODE, NSET=ALLNODES\n";

  nodesStream.setRealNumberNotation(QTextStream::RealNumberNotation::FixedNotation);
  nodesStream.setRealNumberPrecision(3);

  for(int32_t k = 0; k < nnode_z; k++)
  {
    for(int32_t j = 0; j < nnode_y; j++)
    {
      for(int32_t i = 0; i < nnode_x; i++)
      {
        int32_t index = k * nnode_x * nnode_y + j * nnode_x + i;

        float value1 = origin[0] + (i * spacing[0]);
        float value2 = origin[1] + (j * spacing[1]);
        float value3 = origin[2] + (k * spacing[2]);

        nodesStream << QString("%1, %2, %3, %4\n").arg(index + 1).arg(value1, 0, 'f', 3).arg(value2, 0, 'f', 3).arg(value3, 0, 'f', 3);
      }
    }
  }

  // notifyStatusMessage("Finished Writing ABAQUS Nodes File");

  QTextStream elemsStream(&elemsFile);

  elemsStream.setRealNumberNotation(QTextStream::RealNumberNotation::FixedNotation);
  elemsStream.setRealNumberPrecision(3);

  elemsStream << "*ELEMENT, TYPE=C3D8R, ELSET=ALLELEMENTS\n";

  for(int32_t k = 0; k < ne_z; k++)
  {
    for(int32_t j = 0; j < ne_y; j++)
    {
      for(int32_t i = 0; i < ne_x; i++)
      {
        int32_t eindex = k * ne_x * ne_y + j * ne_x + i;
        int32_t index = k * nnode_x * nnode_y + j * nnode_x + i + 1;

        int32_t value1 = index;
        int32_t value2 = index + 1;
        int32_t value3 = index + nnode_x + 1;
        int32_t value4 = index + nnode_x;
        int32_t value5 = index + nnode_x * nnode_y;
        int32_t value6 = index + 1 + nnode_x * nnode_y;
        int32_t value7 = index + nnode_x + nnode_x * nnode_y + 1;
        int32_t value8 = index + nnode_x + nnode_x * nnode_y;

        elemsStream << QString("%1, %2, %3, %4, %5, %6, %7, %8, %9\n").arg(eindex + 1).arg(value1).arg(value2).arg(value3).arg(value4).arg(value5).arg(value6).arg(value7).arg(value8);
      }
    }
  }

  // notifyStatusMessage("Finished Writing ABAQUS Elements Connectivity File");

  std::vector<int32_t> phaseId(maxGrainId, 0);

  std::vector<float> orient(maxGrainId * 3, 0);

  int32_t totalPoints = imageGeom.getNumberOfElements();

  for(int32_t grainId = 1; grainId <= maxGrainId; grainId++)
  {
    for(int32_t i = 0; i < totalPoints; i++)
    {
      if(featureIdsData[i] == grainId)
      {
        phaseId[grainId - 1] = cellPhasesData[i];
        orient[(grainId - 1) * 3] = cellEulerAnglesData[i * 3] * 180.0 * SIMPLib::Constants::k_1OverPi;
        orient[(grainId - 1) * 3 + 1] = cellEulerAnglesData[i * 3 + 1] * 180.0 * SIMPLib::Constants::k_1OverPi;
        orient[(grainId - 1) * 3 + 2] = cellEulerAnglesData[i * 3 + 2] * 180.0 * SIMPLib::Constants::k_1OverPi;
      }
    }
  }

  QTextStream elsetStream(&elsetFile);

  for(int32_t voxelId = 1; voxelId <= maxGrainId; voxelId++)
  {
    size_t elementPerLine = 0;
    elsetStream << QString("*Elset, elset=Grain%1_Phase%2_set\n").arg(voxelId).arg(phaseId[voxelId - 1]);

    for(int32_t i = 0; i < totalPoints; i++)
    {
      if(featureIdsData[i] == voxelId)
      {
        if(elementPerLine != 0) // no comma at start
        {
          if((elementPerLine % 16) != 0u) // 16 per line
          {
            elsetStream << ", ";
          }
          else
          {
            elsetStream << ",\n";
          }
        }
        elsetStream << static_cast<uint64_t>(i + 1);
        elementPerLine++;
      }
    }
    elsetStream << "\n";
  }

  // notifyStatusMessage("Finished Writing ABAQUS Element Sets File");

  masterStream.setRealNumberNotation(QTextStream::RealNumberNotation::FixedNotation);
  masterStream.setRealNumberPrecision(3);

  std::vector<std::vector<double>> MatConst = matConst.getTableData();

  for(int32_t i = 1; i <= maxGrainId; i++)
  {
    masterStream << QString("*Material, name = Grain%1_Phase%2_set\n").arg(i).arg(phaseId[i - 1]);
    masterStream << "*Depvar\n";
    masterStream << numDepvar << "\n";
    masterStream << QString("*User Material, constants = %1\n").arg(numMatConst + 5);
    masterStream << QString("%1, %2, %3, %4, %5").arg(i).arg(phaseId[i - 1]).arg(orient[(i - 1) * 3], 0, 'f', 3).arg(orient[(i - 1) * 3 + 1], 0, 'f', 3).arg(orient[(i - 1) * 3 + 2], 0, 'f', 3);

    auto func = [&MatConst](int32_t i) { return QString::number(MatConst[i][0], 'f', 3); };

    EntriesHelper::writeEntries(masterStream, func, numMatConst, 8, ", ", "\n", 5);

    masterStream << "\n";
    masterStream << "*User Output Variables\n";
    masterStream << numUserOutVar << "\n";
  }

  // We are now defining the sections, which is for each grain

  QTextStream sectsStream(&sectsFile);

  for(int32_t grain = 1; grain <= maxGrainId; grain++)
  {
    sectsStream << QString("*Solid Section, elset=Grain%1_Phase%2_set, material=Grain%3_Phase%4_mat\n").arg(grain).arg(phaseId[grain - 1]).arg(grain).arg(phaseId[grain - 1]);
  }

  // notifyStatusMessage("Finished Writing ABAQUS Sections File");

  return true;
}
