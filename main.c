#include <stdio.h>
#include <hidapi.h>

#define VID 0x1915
#define PID_WIRELESS 0xAE1C
#define PID_WIRED 0xAE11
#define USAGE_PAGE 0xFFA0

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

int main(void) {
    hid_device *hid_device = GetDevice();
    return 0;
}
