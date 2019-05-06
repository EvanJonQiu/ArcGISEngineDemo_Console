#include "Convert2Shape.h"
#include "PathUtilities.h"

#include <iostream>

HRESULT Convert2GDBFeatureClass(void) {
  HRESULT hr = S_OK;

  const char* fullPathToCoverage = "/home/admin/Documents/Road/chinaroad.shp";
  CComBSTR bstrCoveragePath;
  CComBSTR bstrCoverageName;
  hr = GetParentDirFromFullPath(fullPathToCoverage, &bstrCoveragePath, false);
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

  // gdb
  const char * fullpath = "/home/admin/Documents/test/example.gdb";
  CComBSTR path;
  CComBSTR name;
  CComBSTR bstrFullPath;
  hr = GetParentDirFromFullPath(fullpath, &path);
  if (FAILED(hr) || path.Length() <= 0) {
    std::cerr << "Couldn't get path" << std::endl;
    return E_FAIL;
  } else {
    std::wcout << L"path: " << (BSTR)path << std::endl;
  }
  hr = GetFileFromFullPath(fullpath, &name);
  if (FAILED(hr) || name.Length() <= 0) {
    std::cerr << "Couldn't get name" << std::endl;
    return E_FAIL;
  } else {
    std::wcout << L"name: " << (BSTR)name << std::endl;
  }
  
  IWorkspaceNamePtr ipFileGDBWorkName(CLSID_WorkspaceName);
  hr = ipFileGDBWorkName->put_WorkspaceFactoryProgID(CComBSTR(L"esriDataSourcesFile.FileGDBWorkspaceFactory"));
  if (FAILED(hr)) {
    std::cerr << "Get IF of gdb workspace factory failed" << std::endl;
    return hr;
  }
  hr = ipFileGDBWorkName->put_PathName(CComBSTR(fullpath));
  if (FAILED(hr)) {
    std::cerr << "set gdb path failed" << std::endl;
    return hr;
  }
  
  IFeatureClassNamePtr ipDestFeatureClassName(CLSID_FeatureClassName);
  ((IDatasetNamePtr)ipDestFeatureClassName)->put_Name(CComBSTR("test"));
  ((IDatasetNamePtr)ipDestFeatureClassName)->putref_WorkspaceName(ipFileGDBWorkName);

  IFeatureDataConverterPtr ipFeatureDataConverter(CLSID_FeatureDataConverter);
  IEnumInvalidObjectPtr ipEnumInvalidObject;
  hr = ipFeatureDataConverter->ConvertFeatureClass(ipSrcFeatureClassName, 0, 0, ipDestFeatureClassName,
                                                   0, 0, 0, 1000, 0, &ipEnumInvalidObject);
  return hr;
}

HRESULT Convert2GDBFeatureDataset(void) {
  HRESULT hr = S_OK;

  const char* fullPathToCoverage = "/home/admin/Documents/Road/chinaroad.shp";
  CComBSTR bstrCoveragePath;
  CComBSTR bstrCoverageName;
  hr = GetParentDirFromFullPath(fullPathToCoverage, &bstrCoveragePath, false);
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

  // gdb
  const char * fullpath = "/home/admin/Documents/test/example.gdb";
  CComBSTR path;
  CComBSTR name;
  CComBSTR bstrFullPath;
  hr = GetParentDirFromFullPath(fullpath, &path);
  if (FAILED(hr) || path.Length() <= 0) {
    std::cerr << "Couldn't get path" << std::endl;
    return E_FAIL;
  } else {
    std::wcout << L"path: " << (BSTR)path << std::endl;
  }
  hr = GetFileFromFullPath(fullpath, &name);
  if (FAILED(hr) || name.Length() <= 0) {
    std::cerr << "Couldn't get name" << std::endl;
    return E_FAIL;
  } else {
    std::wcout << L"name: " << (BSTR)name << std::endl;
  }
  
  IWorkspaceNamePtr ipFileGDBWorkName(CLSID_WorkspaceName);
  hr = ipFileGDBWorkName->put_WorkspaceFactoryProgID(CComBSTR(L"esriDataSourcesFile.FileGDBWorkspaceFactory"));
  if (FAILED(hr)) {
    std::cerr << "Get IF of gdb workspace factory failed" << std::endl;
    return hr;
  }
  hr = ipFileGDBWorkName->put_PathName(CComBSTR(fullpath));
  if (FAILED(hr)) {
    std::cerr << "set gdb path failed" << std::endl;
    return hr;
  }
  
  IFeatureDatasetNamePtr ipDestFeaDSClsName(CLSID_FeatureDatasetName);
  ((IDatasetNamePtr)ipDestFeaDSClsName)->put_Name(CComBSTR("test"));
  ((IDatasetNamePtr)ipDestFeaDSClsName)->putref_WorkspaceName(ipFileGDBWorkName);

  IFeatureClassNamePtr ipDestFeatureClassName(CLSID_FeatureClassName);
  ((IDatasetNamePtr)ipDestFeatureClassName)->put_Name(CComBSTR("aaaa"));
  ((IDatasetNamePtr)ipDestFeatureClassName)->putref_WorkspaceName(ipFileGDBWorkName);

  IFeatureDataConverterPtr ipFeatureDataConverter(CLSID_FeatureDataConverter);
  IEnumInvalidObjectPtr ipEnumInvalidObject;
  hr = ipFeatureDataConverter->ConvertFeatureClass(ipSrcFeatureClassName, 0, ipDestFeaDSClsName, ipDestFeatureClassName,
                                                   0, 0, 0, 1000, 0, &ipEnumInvalidObject);
  return hr;
}

