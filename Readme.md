# BulletEye v1.0 Server

**Repository:** `https://github.com/shoaibhassan2/BulletEye`

![BulletEye Banner](https://raw.githubusercontent.com/shoaibhassan2/BulletEye/main/assets/banner/banner.png)

A comprehensive Remote Administration Tool (RAT) server built in C for educational purposes. BulletEye provides a robust graphical user interface on Windows for managing and interacting with multiple remote clients simultaneously. It features a rich set of capabilities ranging from remote file exploration to keylogging, all wrapped in a secure, multi-threaded server application.

---

## ⚠️ Important Disclaimer

This application is designed for **educational purposes ONLY**, to demonstrate advanced C programming and networking concepts.

Any use of this software for malicious activities, including but not limited to creating backdoors, unauthorized system access, or launching denial-of-service attacks, is strictly prohibited, unethical, and illegal. The author is not responsible for any misuse of this program.

---

## ✨ Features

BulletEye is packed with features that provide extensive control and monitoring capabilities.

### **Connection & Server Management**
- **Multi-Server Support:** Create, manage, and monitor multiple server instances on different IPs and ports from a single interface.
- **Start/Stop/Remove Servers:** Easily control the state of each server instance through a context menu.
- **Client Management:** View a detailed list of all connected clients.
- **Rich Client Information:** Instantly view client IP, geographic country, unique ID, username, OS version, CPU/GPU models, RAM amount, installed antivirus, and the currently active window.

### **Remote Client Control**
- **Remote File Explorer:**
    - **Full CRUD Operations:** Browse, upload, download, rename, and delete files and folders.
    - **Remote Execution:** Execute files on the client machine with a confirmation prompt.
    - **Drag & Drop Upload:** Easily upload files by dragging them into the file explorer window.
    - **File System Encryption:** Encrypt and decrypt files on the remote file system.
    - **Navigation History:** Includes back/forward navigation for easy folder browsing.
- **Keylogger:**
    - Remotely start and stop the keylogger on the client.
    - Download the captured keylog file to the server.
    - Securely delete the keylog file from the client machine.
- **Connection Control:**
    - **Disconnect:** Forcibly disconnect a client.
    - **Restart:** Send a command to restart the client's connection.
    - **Shutdown:** Send a command to terminate the client process.

### **Advanced GUI & User Experience**
- **Modern Tabbed Interface:** Separate, organized tabs for Server Management, Connected Clients, and Logs.
- **Real-time Logging:** A comprehensive logging view that displays all server events, client connections, and errors.
- **Log Management:** Logs can be cleared, copied to the clipboard, or saved to a text file. Auto-scrolling is supported.
- **Dark Mode:** Features a sleek dark theme for comfortable use in low-light environments.
- **System Tray Notifications:** Receive notifications for critical events like client connections and disconnections.
- **Crash Handler:** Integrated crash reporting that generates a detailed minidump and report upon an unexpected crash, ensuring stability and easier debugging.

### **Networking & Security**
- **Secure Communication:** All communication between the server and client is encrypted using AES-128 and compressed with XPRESS_HUFF to ensure data privacy and efficiency.
- **Configurable Network Settings:** Fine-tune network performance by adjusting maximum connections, connection timeout, and buffer sizes.
- **Bandwidth Throttling:** Limit the upload and download bandwidth for each client to manage network usage.

### **Built-in Tools**
- **Icon Changer:** Easily change the icon of any `.exe` file.
- **Icon Extractor:** Extract the icon from an existing `.exe` or `.dll` file and save it as an `.ico` file.


## 🛠️ Getting Started

### Prerequisites
- A Windows operating system.
- A C compiler, such as **GCC (MinGW-w64)** or **MSVC (Visual Studio)**.
- The **cJSON** library for JSON parsing. [2, 3]
- The **darkmode.dll** library for dark mode support.

### Building from Source

1.  **Clone the repository:**
    ```bash
    git clone https://github.com/shoaibhassan2/BulletEye.git
    cd BulletEye
    ```

2.  **Install Dependencies:**
    - Place the `cJSON.c` and `cJSON.h` files into a library directory accessible by your compiler.
    - Ensure `darkmode.dll` is available in your build or system path.

3.  **Compile the project:**
    You can use a resource compiler to compile `res/resource.rc` and then link it with the compiled C source files.

    **Example using GCC (MinGW-w64):**
    ```bash
    # First, compile the resource file
    windres res/resource.rc -O coff -o res.o

    # Then, compile all C source files and link them with the resource object and libraries
    gcc src/**/*.c src/*.c -o BulletEyeServer.exe res.o -Iinclude -Llibs -lcjson -lws2_32 -lcomctl32 -lgdi32 -lshell32 -lole32 -lshlwapi -luxtheme -lwinmm -ldbghelp -mwindows
    ```
    *(Note: This command is a template and may need adjustments based on your specific project structure and compiler setup.)*

---

## 🚀 Usage

1.  Run the compiled `BulletEyeServer.exe`.
2.  Navigate to the **Manage Server** tab.
3.  Enter the IP address and port you want the server to listen on.
4.  Click the **"Add Server"** button.
5.  Right-click on the newly added server in the list and select **"Start Server"**.
6.  The server status will change to "Listening".
7.  Once a client connects, it will appear in the **Connected Clients** tab.
8.  Right-click on a client to access remote features like the File Explorer and Keylogger.

---

## 🗺️ Project Structure

The project is organized into a clean and modular structure to separate concerns.

```
BulletEye/
├── include/              # Header files
│   ├── crash_handler/
│   ├── gui/
│   │   └── file_explorer/
│   ├── helpers/
│   ├── server/
│   └── service/
├── src/                  # Source code files
│   ├── crash_handler/    # Crash reporting and minidump generation
│   ├── gui/              # All GUI components (windows, pages, controls)
│   │   └── file_explorer/ # Remote file explorer implementation
│   ├── helpers/          # Utility functions (icon changing, etc.)
│   ├── server/           # Core server logic, networking, and management
│   └── service/          # Services like encryption, compression, notifications
├── res/                  # Resources (icons, bitmaps, manifest, .rc file)
└── assets/               # External assets like audio and icons
```

---

## 🚧 Future Work & To-Do List

This project has a solid foundation, but there's always room for improvement and new features.

-   **Implement Builder & Binder:**
    -   Complete the functionality in the "Builder" tab to configure and build the client executable.
    -   Implement the "Binder" tab to bind the client executable with a legitimate file.
-   **Expand Remote Features:**
    -   **Remote Desktop:** Add a feature to view and control the client's screen.
    -   **Remote Shell:** Provide a remote command-line interface.
    -   **Process Manager:** View and manage running processes on the client.
    -   **Webcam/Microphone Access:** Capture video and audio from the client.
-   **Enhance Security:**
    -   Replace the hardcoded AES key with a secure key exchange mechanism (e.g., RSA).
    -   Integrate a more robust, well-known encryption library like OpenSSL.
-   **Improve GUI:**
    -   Add graphical representations of network statistics.
    -   Show client desktop thumbnails in the client list.
-   **Client Development:**
    -   Client Code added and need improvement.
-   **Cross-Platform Support:**
    -   Refactor the code to support a Linux-based server and/or client.

---

## 📜 License

This project is licensed under the MIT License. Additionally, it utilizes third-party libraries which are licensed separately.

### **BulletEye Project License**

**MIT License**

Copyright (c) 2025 Shoaib Hassan

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

---

### **Third-Party Licenses**

This project incorporates code from the following third-party libraries. You must adhere to their respective licenses.

-   **cJSON**
    -   **License:** MIT License
    -   **Copyright:** Copyright (c) 2009-2017 Dave Gamble and cJSON contributors.
    -   **Source:** [https://github.com/DaveGamble/cJSON](https://github.com/DaveGamble/cJSON)

-   **darkmodelib**
    -   **License:** MIT License or Mozilla Public License, version 2.0
    -   **Copyright:** Copyright (c) 2020-2024 Richard K. Szabo
    -   **Source:** [https://github.com/ozone10/darkmodelib](https://github.com/ozone10/darkmodelib)

---

## 👨‍💻 Author & Contact

-   **Author:** Shoaib Hassan
-   **GitHub:** [@shoaibhassan2](https://github.com/shoaibhassan2)
-   **Telegram:** [@ReverserPython](https://t.me/ReverserPython)