# 🖼️ PixelForge – Image Processing Workshop

A desktop image editor built with **C++**, **OOP** and **Qt**, featuring **18 image filters**, live preview, and full undo/redo history.

> 🎓 CS213 – Object Oriented Programming · Assignment 1
> Faculty of Computing and Artificial Intelligence (FCAI), Cairo University · 2026–2027
> 👨‍🏫 Supervised by **Dr. Mohammad El-Ramly**

🎥 **[Watch the demo video](https://drive.google.com/file/d/111sBMX-dty5qfrL2v_PnDpPEMFxap0tR/view?usp=drive_link)**

<!-- Add a screenshot of the app here: -->
<!-- ![PixelForge screenshot](docs/screenshot.png) -->

---

## ✨ Features

- 📂 Open and save **PNG, JPG, JPEG and BMP** images
- 🎛️ **18 filters**, each with its own adjustable settings
- 👀 **Live preview** while moving sliders
- ↩️ **Undo / Redo** and a clickable **edit history**
- 🔍 **Compare** with the original, plus **Fit / 100% / zoom**
- 🔎 Filter **search** box

## 🎨 Filters

| # | Filter | # | Filter | # | Filter |
|---|--------|---|--------|---|--------|
| 01 | Grayscale | 07 | Brightness | 13 | Wano Light |
| 02 | Black & White | 08 | Resize | 14 | Retro TV |
| 03 | Invert | 09 | Merge | 15 | Purple Night |
| 04 | Frame | 10 | Edges | 16 | Infrared |
| 05 | Flip | 11 | Crop | 17 | Skew |
| 06 | Rotate | 12 | Blur | 18 | Oil Paint |

## 🛠️ Tech Stack

- **Language:** C++17
- **GUI:** Qt 6 (Widgets)
- **Build system:** CMake (MinGW)
- **Image I/O:** `Image_Class.h` with `stb_image` / `stb_image_write` (provided by the course)
- **Version control:** Git & GitHub

## 📁 Project Structure

```
├── main.cpp            # App entry point
├── mainwindow.h/.cpp   # GUI: layout, preview, history, filter options
├── filters.h           # All image filters (team implementation)
├── Image_Class.h       # Course image library
├── stb_image*.h        # Image loading / writing
└── CMakeLists.txt
```

## 🚀 Getting Started

### Requirements
- Qt 6 with the **MinGW 64-bit** kit
- CMake 3.16+

### Build & Run
```bash
git clone <your-repo-url>
cd <your-repo-folder>
cmake -B build -DCMAKE_PREFIX_PATH="C:/Qt/6.x.x/mingw_64"
cmake --build build
```

Or open the folder in **CLion**, set the CMake option
`-DCMAKE_PREFIX_PATH=C:/Qt/6.x.x/mingw_64`, and press **Run**.

> 💡 If Qt DLLs are not found at runtime, add `C:\Qt\6.x.x\mingw_64\bin` to your `PATH`.

## 🕹️ How to Use

1. Click **Open image** and pick a picture.
2. Choose a filter from the right panel and adjust its settings.
3. Press **Apply filter** (a live preview is shown first).
4. Use **Undo / Redo** or the history strip to move between steps.
5. Click **Save image** to export the result.

## 📄 License

Educational project for CS213 at FCAI – Cairo University.
