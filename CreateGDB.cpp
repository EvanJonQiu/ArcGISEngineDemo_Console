#include "CreateGDB.h"
#include "PathUtilities.h"

#include <iostream>

HRESULT CreateGDB(void) {
  HRESULT hr = S_OK;
  
  const char * fullpath = "/home/admin/Documents/test/localGDB.gdb";
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
  
  bstrFullPath = path;
  bstrFullPath.Append("/");
  bstrFullPath.Append(name);
  std::wcout << L"fullpath: " << (BSTR)bstrFullPath << std::endl;

  IWorkspaceFactoryPtr ipWorkspaceFactory(CLSID_FileGDBWorkspaceFactory);
  IWorkspaceNamePtr ipWorkspaceName;
  hr = ipWorkspaceFactory->Create(path, name, NULL, 0, &ipWorkspaceName);
  if (FAILED(hr) || ipWorkspaceName == 0) {
    std::cerr << "Create gdb failed" << std::endl;
    return E_FAIL;
  } else {
    std::cerr << "create gdb success" << std::endl;
  }

  INamePtr ipName = (INamePtr)ipWorkspaceName;
  IUnknownPtr ipUnknown;
  hr = ipName->Open(&ipUnknown);
  if (FAILED(hr) || ipUnknown == 0) {
    std::cerr << "open created gdb failed" << std::endl;
    return E_FAIL;
  } else {
    std::cerr << "open created gdb success" << std::endl;
  }

  IWorkspacePtr ipWorkspace = (IWorkspacePtr)ipUnknown;
  if (ipWorkspace == 0) {
    std::cerr << "convert to IWorkspace false" << std::endl;
    return E_FAIL;
  } else {
    std::cerr << "convert to IWorkspace success" << std::endl;
  }

  IFeatureWorkspacePtr ipFeatureWorkspace(ipWorkspace);
  
  // create spatial reference
  ISpatialReferenceFactory2Ptr ipSpaRefFact2(CLSID_SpatialReferenceEnvironment);
  ISpatialReferencePtr ipSpaRef;
  hr = ipSpaRefFact2->CreateSpatialReference(esriSRProjCS_WGS1984WebMercatorMajorAuxSphere, &ipSpaRef);
  if (FAILED(hr) || ipSpaRef == 0) {
    std::cerr << "create SP failed." << std::endl;
    return E_FAIL;
  } else {
    std::cout << "create sp success." << std::endl;
  }

  // create feature data set
  IFeatureDatasetPtr ipOtherFeatureDataset;
  hr = ipFeatureWorkspace->CreateFeatureDataset(CComBSTR("test"), ipSpaRef, &ipOtherFeatureDataset);
  if (FAILED(hr) || ipOtherFeatureDataset == 0) {
    std::cerr << "create fature set failed." << std::endl;
    return E_FAIL;
  } else {
    std::cout << "create feature set success." << std::endl;
  }

  return hr;
}
