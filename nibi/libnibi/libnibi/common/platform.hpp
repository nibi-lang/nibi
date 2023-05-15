#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace nibi {

//! \brief Platform class
class platform_c {
public:
  //! \brief Platform enum
  enum class platform_e { UNKNOWN, WINDOWS, LINUX, MACOS };
  platform_c() = delete;

  //! \brief Constructor
  //! \param include_dirs Include directories
  platform_c(std::vector<std::filesystem::path> &include_dirs);

  //! \brief Retrieve the nibi home path
  //! \return The nibi home path iff it exists
  std::optional<std::filesystem::path> get_nibi_path() const;

  //! \brief Locate a file
  //! \param file_name The file name
  //! \return The file path iff it exists somewhere
  std::optional<std::filesystem::path>
  locate_file(std::filesystem::path &file_name,
              std::filesystem::path &imported_from);

  //! \brief Locate a directory
  //! \param directory_name The directory name
  //! \return The directory path iff it exists somewhere
  std::optional<std::filesystem::path>
  locate_directory(std::string &directory_name);

  //! \brief Retrieve the platform string
  //! \return The platform string
  const char *get_platform_string() const;

private:
  platform_e _platform{platform_e::UNKNOWN};
  std::vector<std::filesystem::path> _include_dirs;
  std::optional<std::filesystem::path> _nibi_path{std::nullopt};
};

extern platform_c *global_platform;

extern bool
global_platform_init(std::vector<std::filesystem::path> include_dirs);

extern bool global_platform_destroy();

} // namespace nibi