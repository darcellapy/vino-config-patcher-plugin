// Nova is real I think
// Note: used some of Inkay, the Pretendo Network plugin source code as a base and to make use of the notfications. Credit to Pretendo for developing Inkay
#include <coreinit/filesystem.h>
#include <coreinit/debug.h>
#include <sysapp/switch.h>
#include <vpad/input.h>

#include <wups.h>
#include <notifications/notifications.h>

#include "utils/logger.h"
#include "config.h"
#include "Notification.h"

WUPS_PLUGIN_NAME("Vino Config Patcher");
WUPS_PLUGIN_DESCRIPTION("TVii config patcher");
WUPS_PLUGIN_VERSION("v1.1");
WUPS_PLUGIN_AUTHOR("Glitchii and Fangal");
WUPS_PLUGIN_LICENSE("GPLv2");

WUPS_USE_STORAGE("vcp");
WUPS_USE_WUT_DEVOPTAB();

#define VINO_CONFIG_PATH "/vol/content/vino_config.txt"
#define VINO_CONFIG_SD_PATH "/vol/external01/TVii/vino_config.txt"

FSMountSource mSource;
char mPath[128]= "";

INITIALIZE_PLUGIN() {
    WHBLogUdpInit();
    WHBLogCafeInit();
    Config::Init();
    
    if (NotificationModule_InitLibrary() != NOTIFICATION_MODULE_RESULT_SUCCESS) {
        DEBUG_FUNCTION_LINE("NotificationModule_InitLibrary failed :(");
    }

    if (Config::connect_to_latte) {
        StartNotificationThread("TVii patch enabled");
    }
    else {
        StartNotificationThread("TVii patch disabled");
    }
}

DEINITIALIZE_PLUGIN() {
    WHBLogUdpDeinit();
    NotificationModule_DeInitLibrary();
}

ON_APPLICATION_START() {
    WHBLogUdpInit();
    WHBLogCafeInit();
    
    DEBUG_FUNCTION_LINE("VCP: hihi");
}

DECL_FUNCTION(int32_t, _SYSSwitchTo, SysAppPFID pfid) {
    SysAppPFID uPfid = pfid;
    
    VPADStatus status;
    VPADReadError err;

    VPADRead(VPAD_CHAN_0, &status, 1, &err);
    
    if (pfid == SYSAPP_PFID_DOWNLOAD_MANAGEMENT) {
        if (Config::replace_download_management) {
            if (!(status.hold & VPAD_BUTTON_ZL)) {
                uPfid = SYSAPP_PFID_TVII;
            }
        }
    }

    return real__SYSSwitchTo(uPfid);
}

DECL_FUNCTION(int32_t, _SYSSwitchToOverlayFromHBM, SysAppPFID pfid) {
    SysAppPFID uPfid = pfid;
    
    VPADStatus status;
    VPADReadError err;

    VPADRead(VPAD_CHAN_0, &status, 1, &err);
    
    if (pfid == SYSAPP_PFID_DOWNLOAD_MANAGEMENT) {
        if (Config::replace_download_management) {
            if (!(status.hold & VPAD_BUTTON_ZL)) {
                uPfid = SYSAPP_PFID_TVII;
            }
        }
    }

    return real__SYSSwitchToOverlayFromHBM(uPfid);
}

DECL_FUNCTION(FSStatus, FSOpenFile, FSClient *client, FSCmdBlock *block, const char *path, const char *mode, FSFileHandle *handle, FSErrorFlag errorMask) {
    if (Config::connect_to_latte) {
        if (strcmp(VINO_CONFIG_PATH, path) == 0) {
            // Applets need to mount the SD card to access files on it :P
            FSGetMountSource(client, block, FS_MOUNT_SOURCE_SD, &mSource, FS_ERROR_FLAG_ALL);
            FSMount(client, block, &mSource, mPath, sizeof(mPath), FS_ERROR_FLAG_ALL);
            FSStatus res = real_FSOpenFile(client, block, VINO_CONFIG_SD_PATH, mode, handle, errorMask);
            if (res != FS_STATUS_OK) {
                OSFatal("--------------- Error ---------------\n\n"\
                        "Error loading vino_config.txt\n\nPlease check if this file is in the correct directory:\n"\
                        "sd:/TVii/vino_config.txt");
            }

            return res;
        }
    }

    return real_FSOpenFile(client, block, path, mode, handle, errorMask);
}

WUPS_MUST_REPLACE(_SYSSwitchTo, WUPS_LOADER_LIBRARY_SYSAPP, _SYSSwitchTo);
WUPS_MUST_REPLACE_PHYSICAL_FOR_PROCESS(_SYSSwitchToOverlayFromHBM, 0x2E47373C, 0x0E47373C, WUPS_FP_TARGET_PROCESS_HOME_MENU);
WUPS_MUST_REPLACE_FOR_PROCESS(FSOpenFile, WUPS_LOADER_LIBRARY_COREINIT, FSOpenFile, WUPS_FP_TARGET_PROCESS_TVII);
