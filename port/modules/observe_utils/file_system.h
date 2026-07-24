#pragma once

#include <filesystem>
#include <vector>

namespace observe
{
  typedef std::filesystem::path File;

  bool replaceFileWithData(const File &file, const unsigned char *data, size_t size);
  bool replaceFileWithText(const File &file, const std::string &text);
  bool hasWriteAccess(const File &file);
  bool fileExists(const File &file);
  bool isDirectory(const File &file);
  bool appendTextToFile(const File &file, const std::string &text);
  std::unique_ptr<unsigned char[]> loadFileData(const File &file, size_t &size);
  std::string loadFileAsString(const File &file);

  File hostExecutable();
  File appDataDirectory();
  File userDocumentsDirectory();
  File createTemporaryFile(const std::string &extension);
  void createDirectory(const File &file);

  std::string fileName(const File &file);
  std::string fileStem(const File &file);
  std::string hostName();
  std::vector<File> searchForFiles(const File &directory, const std::string &regex);
  std::vector<File> searchForDirectories(const File &directory, const std::string &regex);
}