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
#define VINO_CONFIG_PATCH_PATH "/vol/storage_mlc01/sys/title/00050030/1001310a/content/vino_config_patch.txt"
#define VINO_CONFIG_PATCH_SD_PATH "/vol/external01/vino_config_patch.txt"

int gClient = -1;

bool backUpExists() {
    FSAFileHandle handle = -1;
    if (FSAOpenDir(gClient, VINO_CONFIG_BACKUP_PATH, &handle) != FS_ERROR_OK) {
        return false;
    }
    FSACloseFile(gClient, handle);
    return true;
}

bool patchExists() {
    FSAFileHandle handle = -1;
    if (FSAOpenDir(gClient, VINO_CONFIG_PATCH_PATH, &handle) != FS_ERROR_OK) {
        return false;
    }
    FSACloseFile(gClient, handle);
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
}

INITIALIZE_PLUGIN() {
    WHBLogUdpInit();
    WHBLogCafeInit();
    Config::Init();
    if (Mocha_InitLibrary() != MOCHA_RESULT_SUCCESS) {
        DEBUG_FUNCTION_LINE("Fatal error, Mocha_InitLibrary failed :(");
    }
    FSAInit();
    gClient = FSAAddClient(NULL);
    if (gClient == 0) {
        DEBUG_FUNCTION_LINE("Fatal error, failed to add FSAClient :(");
    }
    if (Mocha_UnlockFSClientEx(gClient) != MOCHA_RESULT_SUCCESS) {
        FSADelClient(gClient);
        DEBUG_FUNCTION_LINE("Fatal error, failed to add FSAClient :(");
    }
    if (NotificationModule_InitLibrary() != NOTIFICATION_MODULE_RESULT_SUCCESS) {
        DEBUG_FUNCTION_LINE("NotificationModule_InitLibrary failed :(");
    } 

    if (Config::connect_to_latte) {	
	// check if original Vino config was backed up
	if (isBackedUp() == false) {
            try {
                DEBUG_FUNCTION_LINE("vino_config.txt not backed up... Backing up vino_config.txt...");
                StartNotificationThread("vino_config.txt not backed up... Backing up vino_config.txt...");
                FSARename(gClient, VINO_CONFIG_PATH, "vino_config_backup.txt");
                DEBUG_FUNCTION_LINE("Backed up vino_config.txt!");
                StartNotificationThread("Backed up vino_config.txt!");
            } catch (...) {
                DEBUG_FUNCTION_LINE("VCP: something went weird with the vino_config.txt being backed up\nif nothing appears to have gone wrong you may ignore this warning.")
            }
        } else {
            return;
        }
        if (ranAlready() == true) {
            try {
                FSARename(gClient, VINO_CONFIG_PATCH_PATH, "vino_config.txt");
                DEBUG_FUNCTION_LINE("vino_config.txt has been successfully patched to use LatteU!");
                StartNotificationThread("vino_config.txt has been successfully patched to use LatteU!");
            } catch (...) {
                DEBUG_FUNCTION_LINE("VCP: something went weird with the vino_config.txt being patched\nif nothing appears to have gone wrong you may ignore this warning.")
            }
        } else {
            try {
                FSARename(gClient, VINO_CONFIG_PATCH_SD_PATH, VINO_CONFIG_PATH);
                DEBUG_FUNCTION_LINE("vino_config.txt has been successfully patched to use LatteU!");
                StartNotificationThread("vino_config.txt has been successfully patched to use LatteU!");
            } catch (...) {
                DEBUG_FUNCTION_LINE("VCP: something went weird with the vino_config.txt being patched\nif nothing appears to have gone wrong you may ignore this warning.")
            }
        }
    } else {
        if (isBackedUp() == false) {
            DEBUG_FUNCTION_LINE("vino_config.txt not backed up... Backing up vino_config.txt...");
            StartNotificationThread("vino_config.txt not backed up... Backing up vino_config.txt...");    
            try {
                FSARename(gClient, VINO_CONFIG_PATH, "vino_config_backup.txt");
                DEBUG_FUNCTION_LINE("Backed up vino_config.txt!");
                StartNotificationThread("Backed up vino_config.txt!");
	    } catch (...) {
                DEBUG_FUNCTION_LINE("VCP: something went weird with the vino_config.txt being backed up\nif nothing appears to have gone wrong you may ignore this warning.")
            }
        } else {
            return;
        }
        if (ranAlready() == false) {
            try {
                FSARename(gClient, VINO_CONFIG_PATCH_PATH, "vino_config_patch.txt");
                DEBUG_FUNCTION_LINE("vino_config.txt has been successfully restored to original!");
            } catch (...) {
                DEBUG_FUNCTION_LINE("VCP: something went weird with the vino_config.txt being restored\nif nothing appears to have gone wrong you may ignore this warning.")
            }
        } else {
            try {
                FSARename(gClient, VINO_CONFIG_PATCH_SD_PATH, VINO_CONFIG_PATCH_PATH);
                DEBUG_FUNCTION_LINE("vino_config.txt has been successfully restored to original!");
                StartNotificationThread("vino_config.txt has been successfully restored to original!");
            } catch (...) {
                DEBUG_FUNCTION_LINE("VCP: something went weird with the vino_config.txt being restored\nif nothing appears to have gone wrong you may ignore this warning.")
            }
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
    DEBUG_FUNCTION_LINE("VCP: hihi\n");
    if (!Config::connect_to_latte) {
        DEBUG_FUNCTION_LINE("VCP: Vino patch skipped.");
        return;
    }
}

ON_APPLICATION_ENDS() {
    try {
        FSARename(gClient, VINO_CONFIG_PATH, "vino_config_patch.txt");
        FSARename(gClient, VINO_CONFIG_BACKUP_PATH, "vino_config_backup.txt");
        DEBUG_FUNCTION_LINE("Restored original vino_config.txt!");
    } catch (...) {
        DEBUG_FUNCTION_LINE("VCP: uh oh, something went weird with the restoration of the original vino_config.txt :/\n")
    }
    DEBUG_FUNCTION_LINE("VCP: shutting down...\n");
    StopNotificationThread();
    DEBUG_FUNCTION_LINE("Unmount storage_mlc");
    FSAFlushVolume(gClient, "/vol/storage_mlc01");
    FSADelClient(gClient);
}