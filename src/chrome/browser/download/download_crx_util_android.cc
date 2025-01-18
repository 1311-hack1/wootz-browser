// Copyright 2012 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Download code which handles CRX files (extensions, themes, apps, ...).

#include "chrome/browser/download/download_crx_util.h"
#include "extensions/common/extension.h"
#include "components/download/public/common/download_item.h"
#include "net/http/http_response_headers.h"
#include "content/public/browser/web_contents.h"
#include "content/public/browser/download_item_utils.h"
#include "base/strings/string_util.h"

namespace download_crx_util {

bool IsExtensionDownload(const download::DownloadItem& download_item) {
    LOG(INFO) << "Extensions: IsExtensionDownload checking:";

    std::string content_disposition = download_item.GetContentDisposition();
    LOG(INFO) << "Extensions: Content disposition: " << content_disposition;

    std::string url = download_item.GetURL().spec();
    LOG(INFO) << "Extensions: Download URL: " << url;

    // First check URL directly for trusted sources
    const char* TRUSTED_SOURCES[] = {
        "github.com/wootzapp/ext-store",
        "github.com/itskartike910/extensions",
        "raw.githubusercontent.com/itskartike910/extensions",
        "raw.githubusercontent.com/wootzapp/ext-store",
        "drive.usercontent.google.com",  // Temporary: Remove in future
    };

    // Direct URL check first
    for (const char* source : TRUSTED_SOURCES) {
        if (url.find(source) != std::string::npos) {
            LOG(INFO) << "Extensions: Found trusted source in URL: " << source;
            if (base::EndsWith(url, ".crx", base::CompareCase::INSENSITIVE_ASCII)) {
                LOG(INFO) << "Extensions: Detected trusted CRX download";
                return true;
            }
        }
    }

    // Check if from extension store
    content::WebContents* web_contents = 
        content::DownloadItemUtils::GetWebContents(&download_item);
    if (web_contents) {
        GURL page_url = web_contents->GetLastCommittedURL();
        LOG(INFO) << "Extensions: Page URL: " << page_url.spec();

        if (page_url.SchemeIs("chrome-extension")) {
            LOG(INFO) << "Extensions: Download initiated from chrome-extension";
            if (base::EndsWith(url, ".crx", base::CompareCase::INSENSITIVE_ASCII)) {
                LOG(INFO) << "Extensions: Allowing CRX download from extension";
                return true;
            }
        }
    }

    // Special handling for Google Drive (temporary)
    if (url.find("drive.usercontent.google.com") != std::string::npos &&
        content_disposition.find(".crx") != std::string::npos) {
        LOG(INFO) << "Extensions: Detected Google Drive CRX download";
        return true;
    }

    LOG(INFO) << "Extensions: Not a CRX file or not from trusted source";
    return false;
}

bool IsTrustedExtensionDownload(Profile* profile,
                               const download::DownloadItem& item) {
    std::string url = item.GetURL().spec();
    
    // Check URL directly
    const char* TRUSTED_SOURCES[] = {
        "github.com/wootzapp/ext-store",
        "github.com/itskartike910/extensions",
        "raw.githubusercontent.com/itskartike910/extensions",
        "raw.githubusercontent.com/wootzapp/ext-store",
        "drive.usercontent.google.com",  // Temporary: Remove in future
        "chrome-extension://",
        "chrome://extensions"
    };
    
    for (const char* source : TRUSTED_SOURCES) {
        if (url.find(source) != std::string::npos) {
            LOG(INFO) << "Extensions: Trusting extension from source: " << source;
            return true;
        }
    }

    // Check if from extension store
    content::WebContents* web_contents = 
        content::DownloadItemUtils::GetWebContents(&item);
    if (web_contents && web_contents->GetLastCommittedURL().SchemeIs("chrome-extension")) {
        LOG(INFO) << "Extensions: Trusting extension from extension store";
        return true;
    }
    
    return false;
}

}  // namespace download_crx_util