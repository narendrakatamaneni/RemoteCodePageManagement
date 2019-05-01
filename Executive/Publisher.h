#pragma once
///////////////////////////////////////////////////////////////////////////
// CodePublisher.h : defines facade/executive for OOD Pr1 S19            //
// ver 1.0                                                               //
// Author        :Narendra Katamaneni,CSE687 Syracuse University         //
///////////////////////////////////////////////////////////////////////////
/*
*  Package Operations:
* =======================
*  This package defines CodePublisher which uses different components of
*  CodeConverter, Display and DirectoryN to put together the entire 
*  functionality of the CodePublisher. The mentioned packages are indep-
*  ndent of each other and their only interaction is through this facade
*
*  Required Files:
* =======================
*  CodeUtilities.h
*  Converter.h 
*  Display.h 
*  Loader.h
*  IPublisher.h
*
*  Maintainence History:
* =======================
* ver 2.0 : 04 Apr 2019
* - added functionality for shim
*  ver 1.0 - 14 Feb 2019
*  - first release
*/

#include <string>
#include <vector>

#include "../Converter/Converter.h"
#include "../Display/Display.h"
#include "../Loader/Loader.h"
#include "../CodeUtilities/CodeUtilities.h"
#include "IPublisher.h"

class Publisher 
{
public:
	Publisher();
  bool processCommandLineArgs(int argc, char ** argv);
  
  void workingDirectory(const std::string& dir);
  const std::string& workingDirectory() const;

  void outputDirectory(const std::string& dir);
  const std::string& outputDirectory() const; 

  bool extractFiles();

  void publish();
  void publish(const std::string& file);
  void publish(const std::vector<std::string>& files);

  CodeUtilities::DisplayMode displayMode() const;
  virtual std::vector<std::string> SetCmd(int argc, char** argv);
  virtual std::vector<std::string> GetConvertFiles();
  virtual std::vector<std::string> CollectConvertedFiles() ;
  virtual bool convertFiles(std::string file);
  
   
private:
  CodeUtilities::ProcessCmdLine *pcl_;
  CodeConverter cconv_;
  Display display_;

  std::string dirIn_; 
  std::string dirOut_;

  std::vector<std::string> files_;
  //static std::vector<std::string> convertedfiles_;
  //std::vector<std::string> convertedfiles_;
};

