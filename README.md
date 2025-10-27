# Snake Game 🐍

Đây là trò chơi **Rắn săn mồi** được viết bằng C++ và sử dụng thư viện đồ họa **SDL3**. Trò chơi có giao diện đồ họa thân thiện và cho phép người chơi chọn mức độ khó.

## 🎮 Hướng dẫn cài đặt

### 🔧 Yêu cầu
- Trình biên dịch `g++` hỗ trợ C++11 trở lên
- Đã cài thư viện `SDL3` (hoặc `SDL2` nếu code bạn dùng SDL2)
- Hệ điều hành: Linux, Windows, hoặc macOS

### 📦 Cài SDL3 (trên Ubuntu/Debian)
```bash
sudo apt update
sudo apt install libsdl3-dev
```

### 🛠️ Biên dịch mã nguồn
Mở terminal và chạy lệnh sau trong thư mục chứa file `main.cpp`:

```bash
    g++ -o snake main.cpp -I. -L. -lSDL3
```

## ▶️ Cách chơi

### ⚙️ Chạy chương trình
```bash
./snake
```

### 🕹️ Điều khiển
| Phím                   | Chức năng              |
|------------------------|------------------------|
| `W` hoặc `↑`           | Di chuyển lên          |
| `S` hoặc `↓`           | Di chuyển xuống        |
| `A` hoặc `←`           | Di chuyển sang trái    |
| `D` hoặc `→`           | Di chuyển sang phải    |
| `ESC`                  | Thoát trò chơi         |
| `Enter`                    | Chơi lại khi thua      |

### 🧠 Luật chơi
- Điều khiển rắn ăn thức ăn để ghi điểm.
- Tránh va vào tường và chính thân rắn.
- Trò chơi kết thúc khi rắn va chạm.
- Bạn có thể nhấn `Enter` để chơi lại hoặc `ESC` để thoát.

## 📌 Tính năng
- Giao diện đồ họa trực quan
- Chọn mức độ khó: `EASY`, `MEDIUM`, `HARD` bằng các phím 1,2,3 tương ứng.
- Hệ thống điểm số tăng dần theo thời gian
