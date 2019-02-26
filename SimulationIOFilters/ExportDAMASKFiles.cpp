/*
 * Your License or Copyright can go here
 */

#include "ExportDAMASKFiles.h"

#include "SIMPLib/Common/Constants.h"

#include <QtCore/QDir>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DataContainerSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"
#include "SIMPLib/FilterParameters/IntFilterParameter.h"
#include "SIMPLib/FilterParameters/IntVec3FilterParameter.h"
#include "SIMPLib/FilterParameters/OutputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/OutputPathFilterParameter.h"
#include "SIMPLib/FilterParameters/PreflightUpdatedValueFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Math/GeometryMath.h"
#include "SIMPLib/Math/SIMPLibMath.h"
#include "SIMPLib/Utilities/FileSystemPathHelper.h"

#include "SimulationIO/SimulationIOConstants.h"
#include "SimulationIO/SimulationIOVersion.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ExportDAMASKFiles::ExportDAMASKFiles() 
: m_OutputPath("")
, m_GeometryFileName("")
, m_HomogenizationIndex(1)
, m_FeatureIdsArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::FeatureIds)
, m_CellPhasesArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::Phases)
, m_CellEulerAnglesArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::EulerAngles)

{
  initialize();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ExportDAMASKFiles::~ExportDAMASKFiles() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExportDAMASKFiles::initialize()
{
  setErrorCondition(0);
  setWarningCondition(0);
  setCancel(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExportDAMASKFiles::setupFilterParameters()
{
  FilterParameterVector parameters;

  parameters.push_back(SIMPL_NEW_OUTPUT_PATH_FP("Output Path ", OutputPath, FilterParameter::Parameter, ExportDAMASKFiles,"*" ,"*" ));
  parameters.push_back(SIMPL_NEW_STRING_FP("Geometry File Name", GeometryFileName, FilterParameter::Parameter, ExportDAMASKFiles));

  parameters.push_back(SIMPL_NEW_INTEGER_FP("Homogenization Index", HomogenizationIndex, FilterParameter::Parameter, ExportDAMASKFiles));

  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req =
      DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 1, AttributeMatrix::Type::Cell, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Feature Ids", FeatureIdsArrayPath, FilterParameter::RequiredArray, ExportDAMASKFiles, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req =
      DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Float, 3, AttributeMatrix::Type::Cell, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Euler Angles", CellEulerAnglesArrayPath, FilterParameter::RequiredArray, ExportDAMASKFiles, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req =
      DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 1, AttributeMatrix::Type::Cell, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Phases", CellPhasesArrayPath, FilterParameter::RequiredArray, ExportDAMASKFiles, req));
  }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExportDAMASKFiles::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setOutputPath(reader->readString("OutputPath", getOutputPath()));
  setGeometryFileName(reader->readString("GeometryFileName", getGeometryFileName()));
  setCellEulerAnglesArrayPath(reader->readDataArrayPath("CellEulerAnglesArrayPath", getCellEulerAnglesArrayPath()));
  setCellPhasesArrayPath(reader->readDataArrayPath("CellPhasesArrayPath", getCellPhasesArrayPath()));
  setFeatureIdsArrayPath(reader->readDataArrayPath("FeatureIdsArrayPath", getFeatureIdsArrayPath()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExportDAMASKFiles::dataCheck()
{
  setErrorCondition(0);
  setWarningCondition(0);  


  //  FileSystemPathHelper::CheckOutputFile(this, "Output File Path", getOutputPath(), true);

  getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, getFeatureIdsArrayPath().getDataContainerName());

  QVector<DataArrayPath> dataArrayPaths;

  QVector<size_t> cDims(1, 1);
  
  m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeatureIdsArrayPath(),
                                                                                                        cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_FeatureIdsPtr.lock())                                                                         /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
    {
      m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0);
    } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0)
    {
      dataArrayPaths.push_back(getFeatureIdsArrayPath());
    }
  
  m_CellPhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getCellPhasesArrayPath(),
													cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_CellPhasesPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
    {
      m_CellPhases = m_CellPhasesPtr.lock()->getPointer(0);
    } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0)
    {
      dataArrayPaths.push_back(getCellPhasesArrayPath());
    }
  
  cDims[0] = 3;
  m_CellEulerAnglesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getCellEulerAnglesArrayPath(),
													   cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_CellEulerAnglesPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
    {
      m_CellEulerAngles = m_CellEulerAnglesPtr.lock()->getPointer(0);
    } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0)
    {
      dataArrayPaths.push_back(getCellEulerAnglesArrayPath());
    }
  
  getDataContainerArray()->validateNumberOfTuples<AbstractFilter>(this, dataArrayPaths);
    
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExportDAMASKFiles::preflight()
{
  // These are the REQUIRED lines of CODE to make sure the filter behaves correctly
  setInPreflight(true); // Set the fact that we are preflighting.
  emit preflightAboutToExecute(); // Emit this signal so that other widgets can do one file update
  emit updateFilterParameters(this); // Emit this signal to have the widgets push their values down to the filter
  dataCheck(); // Run our DataCheck to make sure everthing is setup correctly
  emit preflightExecuted(); // We are done preflighting this filter
  setInPreflight(false); // Inform the system this filter is NOT in preflight mode anymore.
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExportDAMASKFiles::execute()
{
  initialize();
  dataCheck();

  // Check Output Path
  QDir dir;
  if(!dir.mkpath(m_OutputPath))
    {
      QString ss = QObject::tr("Error creating parent path '%1'").arg(m_OutputPath);
      setErrorCondition(-1);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return;
    }
  //
  //
  QString geomFile = m_OutputPath + QDir::separator() + m_GeometryFileName + ".geom";
  QString matFile = m_OutputPath + QDir::separator() + "material.config";
  FILE* geomf = fopen(geomFile.toLatin1().data(), "wb");
  FILE* matf = fopen(matFile.toLatin1().data(), "wb");
  //
  //
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getFeatureIdsArrayPath().getDataContainerName());
  
  size_t dims[3] = {0, 0, 0};
  std::tie(dims[0], dims[1], dims[2]) = m->getGeometryAs<ImageGeom>()->getDimensions();
  float res[3] = {0.0f, 0.0f, 0.0f};
  m->getGeometryAs<ImageGeom>()->getResolution(res);
  float origin[3] = {0.0f, 0.0f, 0.0f};
  m->getGeometryAs<ImageGeom>()->getOrigin(origin);
  float size[3] = {0.0f, 0.0f, 0.0f};

  for(int32_t i = 0; i < 3; i++)
    {
      size[i] = dims[i]*res[i];
    }

  int32_t totalPoints = m->getGeometryAs<ImageGeom>()->getNumberOfElements();

  fprintf(geomf,"6       header\n");
  fprintf(geomf,"Generted from DREAM.3D\n");
  fprintf(geomf,"grid    a %zu    b %zu    c %zu\n",dims[0], dims[1], dims[2]);
  fprintf(geomf,"size    x %.3f    y %.3f    z %.3f\n",size[0], size[1], size[2]);
  fprintf(geomf,"origin    x %.3f    y %.3f    z %.3f\n",origin[0], origin[1], origin[2]);
  fprintf(geomf,"homogenization  %d\n", m_HomogenizationIndex);
  fprintf(geomf,"microstructures 0\n");

  int32_t entriesPerLine = 0;
  for(int32_t i = 1; i <= totalPoints; i++)
    {
      if(entriesPerLine != 0) // no comma at start
	{
	  if((entriesPerLine % 10) != 0) // 10 per line
	    {
	      fprintf(geomf, " ");
	    }
	  else
	    {
	      fprintf(geomf, "\n");
	      entriesPerLine = 0;
	    }
	}
      fprintf(geomf,"%10d", i);
      entriesPerLine++;
    }

  fprintf(matf,"<texture>\n");
  for(int32_t i = 0; i < totalPoints; i++)
    {
      fprintf(matf,"[point%d]\n",i+1);
      fprintf(matf,"(gauss) phi1 %.3f   Phi %.3f    phi2 %.3f \n", m_CellEulerAngles[i * 3] * 180.0 * SIMPLib::Constants::k_1OverPi, m_CellEulerAngles[i * 3 + 1] * 180.0 * SIMPLib::Constants::k_1OverPi, m_CellEulerAngles[i * 3 + 2] * 180.0 * SIMPLib::Constants::k_1OverPi );  
    }

  fprintf(matf,"<microstructure>\n");
  for(int32_t i = 0; i < totalPoints; i++)
    {
      fprintf(matf,"[point%d]\n",i+1);
      fprintf(matf,"(constituent)   phase %d texture %d \n", m_CellPhases[i], i+1);
    }

  //
  //
  fclose(geomf);
  fclose(matf);
  //
  //
  if(getErrorCondition() < 0) { return; }
  //
  if (getCancel()) { return; }
  //
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer ExportDAMASKFiles::newFilterInstance(bool copyFilterParameters) const
{
  ExportDAMASKFiles::Pointer filter = ExportDAMASKFiles::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ExportDAMASKFiles::getCompiledLibraryName() const
{ 
  return SimulationIOConstants::SimulationIOBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ExportDAMASKFiles::getBrandingString() const
{
  return "SimulationIO";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ExportDAMASKFiles::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream <<  SimulationIO::Version::Major() << "." << SimulationIO::Version::Minor() << "." << SimulationIO::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ExportDAMASKFiles::getGroupName() const
{ 
  return SIMPL::FilterGroups::Unsupported; 
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ExportDAMASKFiles::getSubGroupName() const
{ 
  return "SimulationIO"; 
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ExportDAMASKFiles::getHumanLabel() const
{ 
  return "Export DAMASK Files"; 
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QUuid ExportDAMASKFiles::getUuid()
{
  return QUuid("{7c58e612-d7d6-5ec7-806b-cce0c1c211a3}");
}

