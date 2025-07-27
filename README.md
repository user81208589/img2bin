# Grid Image Editor (Qt + OpenCV)

## Giới thiệu
Ứng dụng này cho phép:
- Vẽ và chỉnh sửa grid nhị phân.
- Xuất dữ liệu grid ra dạng hex.
- Hỗ trợ hai kích thước canvas:
  - **96×22**
  - **96×31**
- Hiển thị grid overlay.

## Yêu cầu
- **Qt 6.x (MinGW 64-bit)**
- **OpenCV build bằng MinGW 64-bit** (cùng version compiler với Qt)


## Cách build
1. Mở Qt Creator.
2. Tạo project mới hoặc mở file `.pro`.
3. Đảm bảo `.pro` có cấu hình:
   ```pro
   QT += widgets
   CONFIG += c++17 windows
   CONFIG -= console
   TEMPLATE = app

   SOURCES += main.cpp

   RC_ICONS = app.ico

   INCLUDEPATH += C:/opencv-install/include
   LIBS += -LC:/opencv-install/x64/mingw/lib \
       -lopencv_core455 \
       -lopencv_imgproc455 \
       -lopencv_highgui455 \
       -lopencv_imgcodecs455
   ```
4. Build ở **Release mode** để tránh lỗi DLL Debug.
5. Sau khi build:
   ```
   windeployqt release\img2bin.exe
   ```

## Chạy chương trình
- Double-click file `.exe` trong thư mục `release`.
- Hoặc chạy trực tiếp từ Qt Creator.

## Sử dụng
- **Open Image**: Mở ảnh bất kỳ.
- **Switch Canvas**: Chuyển giữa 96×22 và 96×31.
- **Slider Block/Const**: Điều chỉnh tham số adaptive threshold.
- **Opacity**: Thay đổi độ mờ ảnh nền.
- **Clear**: Xóa toàn bộ grid.
- **Export Hex**: Xuất dữ liệu grid.
- **Copy**: Copy dữ liệu hex vào clipboard.


