<meta property="og:title" content="BNM Explorer" />
<meta property="og:description" content="A web-based runtime inspector and debugger for IL2CPP Unity games on Android/VR/Mobile." />
<meta property="og:image" content="Uc5uSAT.png" />
<meta name="theme-color" content="#5865F2">

<div align="center">
    <img src="logo.png" width="15%" alt="logo">
    <h1 align="center">BNM Explorer</h1>
    <p>A web-based runtime inspector and debugger for IL2CPP Unity games on Android/VR/Mobiles.</p>
    <a href="https://github.com/SilentErased/BNMExplorer/releases/latest"><b>✦ Download latest release ✦</b></a>
</div>

<br>

<div align="center">
    <img src="Uc5uSAT.png" alt="demonstration" width="100%">
</div>

### Features
* **Web-Based UI**: Accessible via any web browser on your PC or phone. No heavy desktop client required.
* **Assembly Explorer**: Browse loaded assemblies, namespaces, classes, methods, and fields in a `dnSpy`-like interface.
* **Scene Hierarchy**: Real-time view of the Unity Scene. Create, delete, and manage GameObjects dynamically.
* **Inspector**: 
  * Edit `Transform` (Position, Rotation, Scale).
  * View and modify public/private fields and properties.
  * Supports `int`, `float`, `bool`, `string`, `Vector3`, `Color`, and more.
* **Method Invoker**: Call static or instance methods directly from the browser with custom arguments.
* **Instance Tracking**: Find live instances of any class in memory.
* **Component System**: Add or remove components at runtime.

### Key Advantages
BNM Explorer runs an embedded HTTP server directly inside the game process, allowing for direct memory manipulation without external debugging overhead.

### Requirements
* Patch the target game and insert our native mod.

---

### Connection Guide

Once the native mod is injected, follow these steps to access the interface:

#### 1. Identify Device IP
Execute the following command to retrieve your device's local IP address:
```bash
adb shell ip addr show wlan0
```

Locate the line starting with `inet` (e.g., `inet 192.168.1.101/24`).

> [!IMPORTANT]
> Ensure both your host machine and the target device are connected to the same Wi-Fi network.

#### 2. Access the Interface

Open your web browser and navigate to the following address:
`http://<DEVICE_IP>:8080`

**Example:** `http://192.168.1.101:8080`