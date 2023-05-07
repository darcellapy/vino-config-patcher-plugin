// Nova is real I think
// Note: used some of Inkay, the Pretendo Network plugin source code as a base and to make use of the notfications. Credit to Pretendo for developing Inkay

#include <wups.h>
#include <optional>
#include <utils/logger.h>
#include "config.h"
#include <notifications/notifications.h>
#include "Notification.h"
#include <stdio.h>
#include <string>
#include <coreinit/filesystem_fsa.h>
#include <coreinit/ios.h>
#include <mocha/mocha.h>

WUPS_PLUGIN_NAME("Vino Config Patcher");
WUPS_PLUGIN_DESCRIPTION("LatteU TVii patcher");
WUPS_PLUGIN_VERSION("v1.0");
WUPS_PLUGIN_AUTHOR("Glitchii");
WUPS_PLUGIN_LICENSE("GPL");

WUPS_USE_STORAGE("vcp");

#define VINO_CONFIG_PATH "/vol/storage_mlc01/sys/title/00050030/1001310a/content/vino_config.txt"
#define VINO_CONFIG_BACKUP_PATH "/vol/storage_mlc01/sys/title/00050030/1001310a/content/vino_config_original.txt"
#define VINO_CONFIG_PATCH_PATH "/vino_config_patch.txt"

int gClient = -1;

bool backUpExists() {
    FSADirectoryHandle handle = -1;
    if (FSAOpenDir(gClient, VINO_CONFIG_BACKUP_PATH, &handle) != FS_ERROR_OK) {
        return false;
    }
    FSACloseDir(gClient, handle);
    return true;
}

bool patchExists() {
    FSADirectoryHandle handle = -1;
    if (FSAOpenDir(gClient, VINO_CONFIG_PATCH_PATH, &handle) != FS_ERROR_OK) {
        return false;
    }
    FSACloseDir(gClient, handle);
    return true;
}

bool isBackedUp() {
    if (backUpExists() == true) {
        return true;
    } else {
        return false;
    }
    return NULL;
}

bool ranAlready() {
    if (patchExists() == true) {
        return true;
    } else {
        return false;
    }
    return NULL;
}

bool ranAlready2() {
    if (patchExists() == true) {
        return true;
    } else {
        return false;
    }
    return NULL;
}

INITIALIZE_PLUGIN() {
    WHBLogUdpInit();
    WHBLogCafeInit();
    Config::Init();
    if (Mocha_InitLibrary() != MOCHA_RESULT_SUCCESS) {
        DEBUG_FUNCTION_LINE("Fatal error, Mocha_InitLibrary failed :(");
        return;
    }
    FSAInit();
    gClient = FSAAddClient(NULL);
    if (gClient == 0) {
        DEBUG_FUNCTION_LINE("Fatal error, failed to add FSAClient :(");
        return;
    }
    if (Mocha_UnlockFSClientEx(gClient) != MOCHA_RESULT_SUCCESS) {
        FSADelClient(gClient);
        DEBUG_FUNCTION_LINE("Fatal error, failed to add FSAClient :(");
        return;
    }
    if (NotificationModule_InitLibrary() != NOTIFICATION_MODULE_RESULT_SUCCESS) {
        DEBUG_FUNCTION_LINE("NotificationModule_InitLibrary failed");
    }
    if (Config::connect_to_latte) {	
	// check if original Vino config was backed up
	if (isBackedUp() == false) {
            DEBUG_FUNCTION_LINE("vino_config.txt not backed up... Backing up vino_config.txt...");
            StartNotificationThread("vino_config.txt not backed up... Backing up vino_config.txt...");
            
            FSARename(gClient, VINO_CONFIG_PATH, VINO_CONFIG_BACKUP_PATH);
	    
            DEBUG_FUNCTION_LINE("Backed up vino_config.txt!");
            StartNotificationThread("Backed up vino_config.txt!");
        } else {
            return;
        }
        if (ranAlready() == true) {
            FSARename(gClient, VINO_CONFIG_PATCH_PATH, VINO_CONFIG_PATH);

            DEBUG_FUNCTION_LINE("vino_config.txt has been successfully patched to use LatteU!");
            StartNotificationThread("vino_config.txt has been successfully patched to use LatteU!");
        } else {
            return;
        }
    } else {
        if (isBackedUp() == false) {
            DEBUG_FUNCTION_LINE("vino_config.txt not backed up... Backing up vino_config.txt...");
            StartNotificationThread("vino_config.txt not backed up... Backing up vino_config.txt...");
            
            FSARename(gClient, VINO_CONFIG_PATH, VINO_CONFIG_BACKUP_PATH);
	    
            DEBUG_FUNCTION_LINE("Backed up vino_config.txt!");
            StartNotificationThread("Backed up vino_config.txt!");
        } else {
            return true;
        }
        if (ranAlready2() == false) {
            FSARename(gClient, VINO_CONFIG_PATH, VINO_CONFIG_PATCH_PATH);
            DEBUG_FUNCTION_LINE("vino_config.txt has been successfully restored to original!");
        } else {
            return;
        }
        StartNotificationThread("Note: LatteU vino_config.txt patch is disabled!");
    }
}

DEINITIALIZE_PLUGIN() {
    WHBLogUdpDeinit();
    Mocha_DeInitLibrary();
    NotificationModule_DeInitLibrary();
}

ON_APPLICATION_START() {
    WHBLogUdpInit();
    WHBLogCafeInit();
    DEBUG_FUNCTION_LINE("VCI: hihi\n");
    if (!Config::connect_to_latte) {
        DEBUG_FUNCTION_LINE("VCI: Vino patch skipped.");
        return;
    }
    return;
}

ON_APPLICATION_ENDS() {
    FSARename(gClient, VINO_CONFIG_PATH, VINO_CONFIG_PATCH_PATH);
    FSARename(gClient, VINO_CONFIG_PATH, VINO_CONFIG_BACKUP_PATH);
    DEBUG_FUNCTION_LINE("Restored original vino_config.txt!");
    FSACloseFile(gClient, NULL);
    DEBUG_FUNCTION_LINE("VCI: shutting down...\n");
    StopNotificationThread();
    DEBUG_FUNCTION_LINE("Unmount storage_mlc");
    FSAFlushVolume(gClient, "/vol/storage_mlc01");
    FSADelClient(gClient);
    DEBUG_FUNCTION_LINE("VCI: goodbye!");
}