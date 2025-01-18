#include "chrome/browser/extensions/bundled_extension_util.h"

#include "base/files/file_path.h"
#include "base/files/file_util.h"
#include "base/functional/bind.h"
#include "base/functional/callback.h"
#include "base/logging.h"
#include "base/path_service.h"
#include "base/task/thread_pool.h"
#include "chrome/common/chrome_paths.h"
#include "chrome/grit/chrome_unscaled_resources.h"
#include "extensions/grit/extensions_resources.h"
#include "ui/base/resource/resource_bundle.h"

namespace bundled_extension_util {

namespace {
base::FilePath* g_bundled_extension_path = nullptr;
}  // namespace

base::FilePath ExtractBundledExtension() {
  LOG(INFO) << "WOOTZ: Starting extraction of bundled extension";

  if (!ui::ResourceBundle::HasSharedInstance()) {
    LOG(ERROR) << "WOOTZ: ResourceBundle not initialized";
    return base::FilePath();
  }

  base::FilePath temp_dir;
  if (!base::PathService::Get(base::DIR_TEMP, &temp_dir)) {
    LOG(ERROR) << "WOOTZ: Failed to get temp directory";
    return base::FilePath();
  }
  LOG(INFO) << "WOOTZ: Temp directory obtained: " << temp_dir.value();

  base::FilePath extension_path = 
      temp_dir.Append(FILE_PATH_LITERAL("bundled_extension.crx"));
  LOG(INFO) << "WOOTZ: Extension path set to: " << extension_path.value();

  base::StringPiece extension_data = 
      ui::ResourceBundle::GetSharedInstance().GetRawDataResource(
          IDR_BUNDLED_EXTENSION);

  if (extension_data.empty()) {
    LOG(ERROR) << "WOOTZ: Extension data is empty";
    return base::FilePath();
  }
  LOG(INFO) << "WOOTZ: Extension data size: " << extension_data.size();

  int bytes_written = base::WriteFile(
      extension_path, extension_data.data(), extension_data.size());
  if (bytes_written == -1) {
    LOG(ERROR) << "WOOTZ: Failed to write extension file";
    return base::FilePath();
  }
  LOG(INFO) << "WOOTZ: Successfully wrote extension file, bytes written: " << bytes_written;

  g_bundled_extension_path = new base::FilePath(extension_path);
  LOG(INFO) << "WOOTZ: Bundled extension path set";
  return extension_path;
}

base::FilePath GetBundledExtensionPath() {
  LOG(INFO) << "WOOTZ: Retrieving bundled extension path";
  LOG(INFO) << "WOOTZ: Bundled extension path: " << (g_bundled_extension_path ? g_bundled_extension_path->value() : "null");
  return g_bundled_extension_path ? *g_bundled_extension_path : base::FilePath();
}

}  // namespace bundled_extension_util