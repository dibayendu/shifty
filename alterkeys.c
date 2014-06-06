// http://osxbook.com/book/bonus/chapter2/alterkeys/
// alterkeys.c
// http://osxbook.com
//
// Complile using the following command line:
//     gcc -Wall -o alterkeys alterkeys.c -framework ApplicationServices
//
// You need superuser privileges to create the event tap, unless accessibility
// is enabled. To do so, select the "Enable access for assistive devices"
// checkbox in the Universal Access system preference pane.

#include <ApplicationServices/ApplicationServices.h>
#define TRUE 1
#define FALSE 0

int shiftPressed = 0;
int lastKeyShift = FALSE;

// This callback will be invoked every time there is a keystroke.
//
CGEventRef
myCGEventCallback(CGEventTapProxy proxy, CGEventType type,
                  CGEventRef event, void *refcon)
{

    // Paranoid sanity check.
    if ((type != kCGEventKeyDown) &&
            (type != kCGEventKeyUp) &&
            (type != kCGEventFlagsChanged))
        return event;

    // The incoming keycode.
    CGKeyCode keycode = (CGKeyCode)CGEventGetIntegerValueField(event, kCGKeyboardEventKeycode);
    //CGEventFlags currentFlag;

    // two events are fired, shift down and shift up
    // shift key pressed
    if ((keycode == (CGKeyCode)56) || (keycode == (CGKeyCode)60))
    {
        //currentFlag = CGEventGetFlags(event)&kCGEventFlagMaskShift;
        // printf("event flag: %llu\n", currentFlag);
        // printf("shift pressed\n");
        if (lastKeyShift == TRUE)
        {
            shiftPressed = TRUE;
        }
        else
        {
            lastKeyShift = TRUE;
            shiftPressed = FALSE;
        }
    }
    else
    {
        if (shiftPressed == TRUE)
        {
        //currentFlag = CGEventGetFlags(event)&kCGEventFlagMaskShift;
            CGEventSetFlags(event, kCGEventFlagMaskShift);
            shiftPressed = FALSE;
        }
        //currentFlag = CGEventGetFlags(event)&kCGEventFlagMaskShift;
        //printf("event flag: %llu\n", currentFlag);
        //printf("keycode   : %d\n", keycode);
        lastKeyShift = FALSE;
    }
    // printf("shift pressed => %d\n", shiftPressed);

    // Set the modified keycode field in the event.
    CGEventSetIntegerValueField(
        event, kCGKeyboardEventKeycode, (int64_t)keycode);

    // We must return the event for it to be useful.
    return event;
}

int
main(void)
{
    CFMachPortRef      eventTap;
    CGEventMask        eventMask;
    CFRunLoopSourceRef runLoopSource;

    // Create an event tap. We are interested in key presses.
    // CGEventTypes.h, also with shift and command keys
    eventMask = ((1 << kCGEventKeyDown) | (1 << kCGEventKeyUp)) | (1 << kCGEventFlagsChanged);
    eventTap = CGEventTapCreate(kCGSessionEventTap, kCGHeadInsertEventTap, 0, eventMask, myCGEventCallback, NULL);
    if (!eventTap) {
        fprintf(stderr, "failed to create event tap\n");
        exit(1);
    }

    // Create a run loop source.
    runLoopSource = CFMachPortCreateRunLoopSource(
                        kCFAllocatorDefault, eventTap, 0);

    // Add to the current run loop.
    CFRunLoopAddSource(CFRunLoopGetCurrent(), runLoopSource,
                       kCFRunLoopCommonModes);

    // Enable the event tap.
    CGEventTapEnable(eventTap, true);

    // Set it all running.
    CFRunLoopRun();

    // In a real program, one would have arranged for cleaning up.

    exit(0);
}
