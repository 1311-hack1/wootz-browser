#ifndef CHROME_BROWSER_ANDROID_EXTENSION_FILE_UTIL_H_
#define CHROME_BROWSER_ANDROID_EXTENSION_FILE_UTIL_H_

#include "base/files/file_path.h"

namespace extension_file_util {

// Copies bundled extension from APK assets to device storage
bool CopyBundledExtensionToStorage(const base::FilePath& asset_path,
                                 const base::FilePath& target_path);


// Initiates the extension installation flow
void InitiateExtensionInstall(Profile* profile,
                             const base::FilePath& crx_path);

}  // namespace extension_file_util

#endif  // CHROME_BROWSER_ANDROID_EXTENSION_FILE_UTIL_H_ 