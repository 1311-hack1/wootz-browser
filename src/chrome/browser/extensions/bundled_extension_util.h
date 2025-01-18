#ifndef CHROME_BROWSER_EXTENSIONS_BUNDLED_EXTENSION_UTIL_H_
#define CHROME_BROWSER_EXTENSIONS_BUNDLED_EXTENSION_UTIL_H_

#include "base/files/file_path.h"

namespace bundled_extension_util {

// Extracts the bundled extension from resources to a temporary location
// Returns the path where the extension was extracted
base::FilePath ExtractBundledExtension();

// Gets the stored path to the bundled extension
base::FilePath GetBundledExtensionPath();

}  // namespace bundled_extension_util

#endif  // CHROME_BROWSER_EXTENSIONS_BUNDLED_EXTENSION_UTIL_H_