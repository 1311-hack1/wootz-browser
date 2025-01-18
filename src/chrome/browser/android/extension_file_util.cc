#include "chrome/browser/android/extension_file_util.h"

#include "base/files/file_util.h"
#include "chrome/browser/download/download_crx_util.h"
#include "content/public/browser/browser_thread.h"

namespace extension_file_util {

namespace {

const char kExtensionMimeType[] = "application/x-chrome-extension";

}  // namespace

bool CopyBundledExtensionToStorage(const base::FilePath& asset_path,
                                 const base::FilePath& target_path) {
  // Ensure target directory exists
  base::FilePath target_dir = target_path.DirName();
  if (!base::DirectoryExists(target_dir) &&
      !base::CreateDirectory(target_dir)) {
    LOG(ERROR) << "Failed to create directory: " << target_dir.value();
    return false;
  }

  // Copy the file
  if (!base::CopyFile(asset_path, target_path)) {
    LOG(ERROR) << "Failed to copy extension from " << asset_path.value()
               << " to " << target_path.value();
    return false;
  }

  return true;
}


void InitiateExtensionInstall(Profile* profile,
                             const base::FilePath& crx_path) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  
  auto mock_item = CreateMockDownloadItem(crx_path);
  
  // Create and configure the installer
  scoped_refptr<extensions::CrxInstaller> installer =
      download_crx_util::CreateCrxInstaller(profile, *mock_item);
      
  // Start the installation
  installer->InstallCrx(crx_path);
}

}  // namespace extension_file_util 