#include <stdio.h>
#include <hidapi.h>

#ifdef _WIN32

#include <Windows.h>

#else

#include <unistd.h>

#endif

#define VID 0x1915
#define PID_WIRELESS 0xAE1C
#define PID_WIRED 0xAE11
#define USAGE_PAGE 0xFFA0

struct BatteryInfo {
    int iBattery;
    int bCharging;
    int bFullCharge;
    int bOnline;
};

hid_device *GetDevice() {
    struct hid_device_info *psDevs = NULL;
    struct hid_device_info *psCurDev = NULL;
    hid_device *phDevice = NULL;

    // Enumerate wireless devices
    psDevs = hid_enumerate(VID, PID_WIRELESS);
    if (!psDevs) {
        // If no wireless device, try wired devices
        psDevs = hid_enumerate(VID, PID_WIRED);
    }

    if (!psDevs) {
        printf("The specified device cannot be found.\n");
        return NULL;
    }

    psCurDev = psDevs;
    while (psCurDev) {
        if (psCurDev->usage_page == USAGE_PAGE) {
            phDevice = hid_open_path(psCurDev->path);
            if (phDevice) {
                printf("Device found with name %s and opened\n", psCurDev->path);
                break;
            } else {
                printf("Failed to open device path.");
            }
        }
        psCurDev = psCurDev->next;
    }

    hid_free_enumeration(psDevs);
    return phDevice;
}

struct BatteryInfo GetBattery() {
    struct BatteryInfo sInfo = {0, 0, 0, 0};
    hid_device *phDevice = GetDevice();

    if (!phDevice) {
        printf("Failed to get device\n");
        return sInfo;
    }

    unsigned char acReport[32] = {0};
    acReport[0] = 5;  // Report ID
    acReport[1] = 21;
    acReport[4] = 1;

    printf("Sending report\n");
    for (int i = 0; i < 32; i++) {
        printf("%02x ", acReport[i]);
    }
    printf("\n");
    int iRes = hid_send_feature_report(phDevice, acReport, sizeof(acReport));
    if (iRes == -1) {
        printf("Failed to send feature report\n");
        hid_close(phDevice);
        return sInfo;
    }
#ifdef _WIN32
    Sleep(1000);
#else
    sleep(1);
#endif

    unsigned char acResponse[32] = {0};
    acResponse[0] = 5;  // Set the report ID in the buffer

    iRes = hid_get_feature_report(phDevice, acResponse, sizeof(acResponse));
    if (iRes == -1) {
        printf("Failed to get feature report\n");
        hid_close(phDevice);
        return sInfo;
    }

    printf("Received report, bytes read: %d\n", iRes);

    // Print the raw response for debugging
    printf("Raw response:\n");
    for (int i = 0; i < iRes; i++) {
        printf("%02x ", acResponse[i]);
    }
    printf("\n");

    if (iRes >= 13) {  // Ensure we have enough data
        sInfo.iBattery = acResponse[9];
        sInfo.bCharging = acResponse[10] != 0;
        sInfo.bFullCharge = acResponse[11] != 0;
        sInfo.bOnline = acResponse[12] != 0;

    } else {
        printf("Insufficient data in response\n");
    }

    hid_close(phDevice);
    return sInfo;
}

int main() {
    if (hid_init() != 0) {
        printf("Failed to initialize hidapi");
        return 1;
    }

    struct BatteryInfo sInfo = GetBattery();
    printf("Battery: %d\n", sInfo.iBattery);
    printf("Charging: %d\n", sInfo.bCharging);
    printf("Full charge: %d\n", sInfo.bFullCharge);
    printf("Online: %d\n", sInfo.bOnline);

    hid_exit();
    return 0;
}
