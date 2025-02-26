//
//  main.cpp
//  Eject Remap
//
//  Created by Daan Vissers on 26/02/2025.
//

#include <iostream>
#include <IOKit/hid/IOHIDManager.h>
#include <CoreFoundation/CoreFoundation.h>

// Eject key usage pair
#define HID_USAGE_CONSUMER_EJECT 0xB8
#define HID_USAGE_CONSUMER_VOLUME_UP 0xE9

IOHIDManagerRef hidManager;

// Function to send a HID event (simulating Volume Up key press)
void sendHIDEvent(uint16_t usage) {
    // Here, we would use CGEvent or another API to send the event
    std::cout << "Simulating key press: " << std::hex << usage << std::endl;
}

// Callback function for HID events
void HIDKeyboardCallback(void* context, IOReturn result, void* sender, IOHIDValueRef value) {
    IOHIDElementRef element = IOHIDValueGetElement(value);
    uint32_t usagePage = IOHIDElementGetUsagePage(element);
    uint32_t usage = IOHIDElementGetUsage(element);
    
    if (usagePage == 0x0C && usage == HID_USAGE_CONSUMER_EJECT) { // Consumer Page & Eject Key
        std::cout << "Eject Key Pressed! Remapping to Volume Up...\n";
        sendHIDEvent(HID_USAGE_CONSUMER_VOLUME_UP);
    }
}

// Initialize HID Manager
void setupHIDManager() {
    hidManager = IOHIDManagerCreate(kCFAllocatorDefault, kIOHIDOptionsTypeNone);
    
    // Matching dictionary for Consumer Control devices
    CFMutableDictionaryRef matchDict = CFDictionaryCreateMutable(kCFAllocatorDefault, 0,
        &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
    int usagePage = 0x0C; // Consumer Page
    CFNumberRef pageNumber = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &usagePage);
    CFDictionarySetValue(matchDict, CFSTR(kIOHIDDeviceUsagePageKey), pageNumber);
    CFRelease(pageNumber);

    IOHIDManagerSetDeviceMatching(hidManager, matchDict);
    CFRelease(matchDict);

    // Register input callback
    IOHIDManagerRegisterInputValueCallback(hidManager, HIDKeyboardCallback, nullptr);
    IOHIDManagerScheduleWithRunLoop(hidManager, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
    IOHIDManagerOpen(hidManager, kIOHIDOptionsTypeNone);
}

int main() {
    std::cout << "Listening for Eject key events on macOS...\n";
    
    setupHIDManager();

    // Keep the program running
    CFRunLoopRun();
    
    return 0;
}
