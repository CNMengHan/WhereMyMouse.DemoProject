# WhereMyMouse

A lightweight and customizable Windows desktop application that visually highlights your mouse cursor with a dynamic, speed-sensitive circle. This tool is designed to help users quickly locate their mouse on screen, especially useful for presentations, screen recordings, or multi-monitor setups.

一个轻量级且可定制的Windows桌面应用程序，通过一个动态的、对鼠标速度实时调整的圆来视觉化地突出显示您的鼠标光标。此工具旨在帮助用户快速在屏幕上定位鼠标，特别适用于演示、屏幕录制或多显示器设置。

## Features / 功能

*   **Dynamic Circle Size**: The circle's diameter changes based on mouse movement speed, expanding when the mouse moves fast and shrinking when it's slow or still.
    *   **动态圆大小**: 圆的直径根据鼠标移动速度而变化，鼠标快速移动时放大，慢速或静止时缩小。
*   **Always-on-Top & Click-Through**: The application window stays on top of other applications and allows mouse clicks to pass through to underlying windows.
    *   **始终置顶与点击穿透**: 应用程序窗口始终置于其他应用程序之上，并允许鼠标点击穿透到下层窗口。
*   **Customizable Appearance**: Easily adjust parameters like maximum/minimum circle diameter, border thickness, and color.
    *   **可定制外观**: 轻松调整最大/最小圆直径、边框粗细和颜色等参数。
*   **Smooth Animation**: Fluid scaling and positioning of the circle for a seamless user experience.
    *   **平滑动画**: 圆的平滑缩放和定位，提供无缝的用户体验。

## Technology Stack / 技术栈

*   **Language / 语言**: C++
*   **Frameworks & APIs / 框架与API**:
    *   **Windows API (Win32 API)**: For core window management, message handling, timers, and mouse input.
    *   **GDI+**: For advanced 2D graphics rendering, including anti-aliased circle drawing.

## Installation / 安装

1.  **Clone the repository / 克隆仓库**:
    ```bash
    git clone https://github.com/CNMengHan/WhereMyMouse.DemoProject.git
    ```
2.  **Open in Visual Studio / 在Visual Studio中打开**:
    Open `WindowsProject1.sln` with Visual Studio 2019 or later.
    使用Visual Studio 2019或更高版本打开 `WindowsProject1.sln`。
3.  **Build the project / 构建项目**:
    Build the solution in `Release` or `Debug` mode. The executable will be generated in `x64/Release` or `x64/Debug`.
    在 `Release` 或 `Debug` 模式下构建解决方案。可执行文件将生成在 `x64/Release` 或 `x64/Debug` 目录下。

## Usage / 使用

1.  Run the `WhereMyMouse1.0.0.exe` (or `CircleFollowyourMouse2.0.exe` if you rename it) executable from the `x64/Release` (or `x64/Debug`) directory.
    从 `x64/Release` (或 `x64/Debug`) 目录运行 `WhereMyMouse1.0.0.exe` 可执行文件。
2.  The transparent circle will appear around your mouse cursor, dynamically adjusting its size.
    透明圆将出现在您的鼠标光标周围，并动态调整其大小。
3.  To close the application, you may need to end the process via Task Manager, or implement a tray icon/hotkey for graceful exit.
    要关闭应用程序，您可能需要通过任务管理器结束进程。

## Configuration / 配置

You can modify the following parameters in `WindowsProject1.cpp` to customize the application:

您可以在 `WindowsProject1.cpp` 中修改以下参数以自定义应用程序：

```cpp
#define CIRCLE_MAX_DIAMETER 1000 // 圆的最大直径（像素）。
#define CIRCLE_MIN_DIAMETER 1    // 圆的最小直径（像素）。
#define PEN_THICKNESS 1          // 圆边框的粗细（像素）。
#define TRANSPARENT_COLOR RGB(0, 0, 1) // 窗口的透明色键。
#define MICROSOFT_BLUE RGB(0, 120, 215) // 圆边框的颜色。
#define MOUSE_TIMER_INTERVAL 10  // 定时器触发间隔（毫秒）。
#define MOUSE_SPEED_THRESHOLD 1000 // 鼠标速度阈值（像素/秒）。
#define RESIZE_STEP 2            // 缩放步长（像素）。
```

## Contributing / 贡献

Contributions are welcome! Please feel free to open issues or submit pull requests.

欢迎贡献！请随时提交问题或拉取请求。

## License / 许可证

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details. <!-- Placeholder for LICENSE file -->

本项目采用MIT许可证 - 详情请参阅 [LICENSE](LICENSE) 文件。
