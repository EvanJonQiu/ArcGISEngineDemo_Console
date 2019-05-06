#include "Convert2Shape.h"
#include "PathUtilities.h"

#include <iostream>

HRESULT Convert2Shape(void) {
  const char* fullPathToCoverage = "/home/admin/Documents/Road/chinaroad.shp";
  CComBSTR bstrCoveragePath;
  CComBSTR bstrCoverageName;
  HRESULT hr = GetParentDirFromFullPath(fullPathToCoverage, &bstrCoveragePath, false);
  if (FAILED(hr) || bstrCoveragePath.Length() == 0) 
  {
    std::cerr << "Error parsing path to coverage" << std::endl;
    return hr;
  }
  hr = GetFileFromFullPath(fullPathToCoverage, &bstrCoverageName);
  if (FAILED(hr) || bstrCoverageName.Length() == 0) 
  {
    std::cerr << "Error parsing path to coverage" << std::endl;
    return hr;
  }

  IWorkspaceFactoryPtr ipSrcWorkspaceFactor(CLSID_ShapefileWorkspaceFactory);
  IWorkspacePtr ipSrcWorkspace;
  hr = ipSrcWorkspaceFactor->OpenFromFile(bstrCoveragePath, 0, &ipSrcWorkspace);
  if (FAILED(hr) || ipSrcWorkspace == 0)
  {
    std::cerr << "Couldn't find the path " << fullPathToCoverage << std::endl;
    if (FAILED(hr))
      return hr;
    else
      return E_FAIL;
  }

  IFeatureWorkspacePtr ipSrcFeatureWorkspace(ipSrcWorkspace);
  IFeatureClassPtr ipSrcFeatureClass;
  hr = ipSrcFeatureWorkspace->OpenFeatureClass(bstrCoverageName, &ipSrcFeatureClass);
  if (FAILED(hr) || ipSrcFeatureClass == 0) {
    std::cerr << "Couldn't open coverage\n";
    if (FAILED(hr)) 
      return hr;
    else
      return E_FAIL;
  }

  IDatasetPtr ipSrcDataset(ipSrcFeatureClass);
  INamePtr ipSrcName;
  ipSrcDataset->get_FullName(&ipSrcName);
  IFeatureClassNamePtr ipSrcFeatureClassName(ipSrcName);

  // Parse full path to output into directory and file name
  const char* fullPathToShapefile = "/home/admin/Documents/Road2/other.shp";
  CComBSTR bstrShapefilePath;
  CComBSTR bstrShapefileName;
  hr = GetParentDirFromFullPath(fullPathToShapefile, &bstrShapefilePath, false);
  if (FAILED(hr) || bstrShapefilePath.Length() == 0) 
  {
    std::cerr << "Error parsing path to shapefile" << std::endl;
    return hr;
  }
  hr = GetFileFromFullPath(fullPathToShapefile, &bstrShapefileName);
  if (FAILED(hr) || bstrShapefileName.Length() == 0) 
  {
    std::cerr << "Error parsing path to shapefile" << std::endl;
    return hr;
  }

  IWorkspaceNamePtr ipShapeWorkName(CLSID_WorkspaceName);
  ipShapeWorkName->put_WorkspaceFactoryProgID(CComBSTR(L"esriDataSourcesFile.ShapefileWorkspaceFactory"));
  ipShapeWorkName->put_PathName(bstrShapefilePath);
  IFeatureClassNamePtr ipShapeFeatClassName(CLSID_FeatureClassName);
  ((IDatasetNamePtr)ipShapeFeatClassName)->put_Name(bstrShapefileName);
  ((IDatasetNamePtr)ipShapeFeatClassName)->putref_WorkspaceName(ipShapeWorkName);

  IFeatureDataConverterPtr ipFeatDataConv(CLSID_FeatureDataConverter);
  IEnumInvalidObjectPtr ipEnumInvalidObject;
  hr = ipFeatDataConv->ConvertFeatureClass(ipSrcFeatureClassName, 0, 0, ipShapeFeatClassName,
    0, 0, 0, 1000, 0, &ipEnumInvalidObject);
  return hr;
}
