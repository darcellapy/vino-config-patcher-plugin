// Taken from the Inkay plugin by Pretendo Network and modified. Original code credit goes to Pretendo.
// Modified by Nova (Glitchii)
#include "config.h"
#include "utils/logger.h"

#include <coreinit/title.h>
#include <sysapp/launch.h>

#include <wups.h>
#include <wups/config/WUPSConfigItemBoolean.h>
#include <wups/config/WUPSConfigItemStub.h>

bool Config::connect_to_latte = true;
bool Config::replace_download_management = false;
bool needRelaunch = false;

void Config::Init() {
    WUPSStorageError storageRes = WUPS_OpenStorage();
    if (storageRes != WUPS_STORAGE_ERROR_SUCCESS) {
        DEBUG_FUNCTION_LINE("Failed to open storage %s (%d)", WUPS_GetStorageStatusStr(storageRes), storageRes);
    }
    else {
        // Try to get value from storage
        if ((storageRes = WUPS_GetBool(nullptr, "connect_to_latte", &connect_to_latte)) == WUPS_STORAGE_ERROR_NOT_FOUND) {
            // Add the value to the storage if it's missing.
            if (WUPS_StoreBool(nullptr, "connect_to_latte", connect_to_latte) != WUPS_STORAGE_ERROR_SUCCESS) {
                DEBUG_FUNCTION_LINE("Failed to store bool");
            }
        }
        if ((storageRes = WUPS_GetBool(nullptr, "replace_download_management", &replace_download_management)) == WUPS_STORAGE_ERROR_NOT_FOUND) {
            // Add the value to the storage if it's missing.
            if (WUPS_StoreBool(nullptr, "replace_download_management", replace_download_management) != WUPS_STORAGE_ERROR_SUCCESS) {
                DEBUG_FUNCTION_LINE("Failed to store bool");
            }
        }
        else if (storageRes != WUPS_STORAGE_ERROR_SUCCESS) {
            DEBUG_FUNCTION_LINE("Failed to get bool %s (%d)", WUPS_GetStorageStatusStr(storageRes), storageRes);
        }

        // Close storage
        if (WUPS_CloseStorage() != WUPS_STORAGE_ERROR_SUCCESS) {
            DEBUG_FUNCTION_LINE("Failed to close storage");
        }
    }
}

static void connect_to_latte_changed(ConfigItemBoolean *item, bool new_value) {
    DEBUG_FUNCTION_LINE("New value in connect_to_latte_changed: %d", new_value);
    
    Config::connect_to_latte = new_value;
    needRelaunch = true;
    WUPS_StoreBool(nullptr, "connect_to_latte", Config::connect_to_latte);
}

static void replace_download_management_changed(ConfigItemBoolean *item, bool new_value) {
    DEBUG_FUNCTION_LINE("New value in replace_download_management_changed: %d", new_value);
    
    Config::replace_download_management = new_value;
    needRelaunch = true;
    WUPS_StoreBool(nullptr, "replace_download_management", Config::replace_download_management);
}

WUPS_GET_CONFIG() {
    // We open the storage so we can persist the configuration the user did.
    if (WUPS_OpenStorage() != WUPS_STORAGE_ERROR_SUCCESS) {
        DEBUG_FUNCTION_LINE("Failed to open storage");
        return 0;
    }

    WUPSConfigHandle config;
    WUPSConfig_CreateHandled(&config, "VCI");

    WUPSConfigCategoryHandle cat;
    WUPSConfig_AddCategoryByNameHandled(config, "Patching", &cat);

    WUPSConfigItemBoolean_AddToCategoryHandled(config, cat, "Vino Config Patch", "Patch your vino config to use LatteU", Config::connect_to_latte, &connect_to_latte_changed);
    WUPSConfigItemBoolean_AddToCategoryHandled(config, cat, "Replace Donwload Management", "Replace Download Management with TVii", Config::replace_download_management, &replace_download_management_changed);
    WUPSConfigItemStub_AddToCategoryHandled(config, cat, "Replace Info", "If enabled, to open Download Management hold \ue085 when loading.");

    return config;
}

WUPS_CONFIG_CLOSED() {
    // Save all changes
    if (WUPS_CloseStorage() != WUPS_STORAGE_ERROR_SUCCESS) {
        DEBUG_FUNCTION_LINE("Failed to close storage");
    }

    // Relaunch the currently running title
    if (needRelaunch) {
        _SYSLaunchTitleWithStdArgsInNoSplash(OSGetTitleID(), nullptr);
        needRelaunch = false;
    }
}
