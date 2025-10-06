# 🎮 GameProgrammingASM3 — Cocos2d-x 3.17.2 Project

This is a **C++ game project** built using **Cocos2d-x 3.17.2** and **Visual Studio**.  
Everything needed to build and run the game is already included — no need to install Python 2.7 or configure the Cocos environment manually.

---

## 🧰 Requirements

Before opening the project, make sure you have these installed:

| Tool | Recommended Version | Notes |
|------|---------------------|-------|
| **Visual Studio** | 2019 or 2022 | Install the **Desktop development with C++** workload |
| **Windows SDK** | 10.0 or newer | Usually comes with Visual Studio |
| **Git** | Latest | To clone this repository |
| **C++ Redistributables** | 2012, 2013, 2015–2022 | Required for Cocos2d-x runtime DLLs |

---

## ⚙️ Setup Instructions

### 1️⃣ Clone the repository
Open a terminal (PowerShell, Git Bash, or CMD):

```bash
git clone https://github.com/<your-username>/GameProgrammingASM3.git
cd GameProgrammingASM3
```

> ✅ The repository already includes the `cocos2d/` engine folder, so you don’t need to install Python 2.7 or run any setup scripts.

---

### 2️⃣ Open the Visual Studio project

Navigate to:
```
GameProgrammingASM3/proj.win32/
```

Double-click:
```
GameProgrammingASM3.sln
```

This will open the full project in **Visual Studio**.

---

### 3️⃣ Select build configuration

At the top of Visual Studio, set:

```
Configuration: Debug
Platform: Win32
```

> ⚠️ Cocos2d-x 3.17.2 is designed for **Win32 builds**.  
> Building for **x64** will cause linker or missing symbol errors.

---

### 4️⃣ Build and run the project

To **compile and launch the game**, press:

- ▶️ **F5** — Run with debugging  
- 🚀 **Ctrl + F5** — Run without debugging

If everything is set up correctly, a window will appear showing the default “Hello World” scene.

---

## 💡 Fixing Missing DLL Errors

If you see an error like:

```
The program can't start because MSVCR110.dll (or MSVCP140.dll) is missing
```

Do this:

1. Install the **Microsoft Visual C++ Redistributables**:
   - [VC++ 2012 Redistributable (x86)](https://www.microsoft.com/en-us/download/details.aspx?id=30679)
   - [VC++ 2013 Redistributable (x86)](https://www.microsoft.com/en-us/download/details.aspx?id=40784)
   - [VC++ 2015–2022 Redistributable (x86)](https://aka.ms/vs/17/release/vc_redist.x86.exe)

2. Restart Visual Studio and rebuild.

If the issue persists, ensure your **build target** is set to `Win32 (x86)` instead of `x64`.

---

## 📁 Project Structure

```
GameProgrammingASM3/
 ├── Classes/          # C++ game logic source files
 ├── Resources/        # Sprites, sounds, fonts, etc.
 ├── cocos2d/          # Cocos2d-x engine source (included)
 ├── proj.win32/       # Visual Studio project files (.sln, .vcxproj)
 └── CMakeLists.txt    # Optional CMake build config
```

---

## 👥 Team Guidelines

- ✅ Commit only **code and assets**
- 🚫 Do not commit:
  - `Debug/`, `Release/`, or `bin/`
  - `.vs/` folder
  - `*.suo`, `*.user`, `*.vcxproj.user`
- 🔀 Use Git branches for features or bug fixes
- 💬 Create Pull Requests for merging into `main`
- 🧩 Always test your build in **Debug (Win32)** before pushing

---

## 🧹 Common Issues

| Problem | Cause | Solution |
|----------|--------|-----------|
| **Missing DLLs** | Missing VC++ runtime | Install the redistributables listed above |
| **x64 linker errors** | Wrong build platform | Change to `Win32` |
| **Engine header not found** | Missing cocos2d folder | Make sure `cocos2d/` is included in the repo |
| **Black window or crash** | Resource path issue | Verify your assets are inside `Resources/` |

---

## 📜 License

This project uses **Cocos2d-x 3.17.2**, © Cocos2d-x contributors.  
Use is limited to educational and non-commercial purposes.
