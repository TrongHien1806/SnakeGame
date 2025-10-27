#include <SDL3/SDL.h>
#include <iostream>
#include <vector>
#include <ctime>
#include <random>
#include <deque>
#include <string>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int GRID_SIZE = 20;
// const int SNAKE_SPEED = 30; // Số lần cập nhật mỗi giây
static int SNAKE_SPEED = 10;
// Định nghĩa hướng di chuyển
enum Direction {
    UP, RIGHT, DOWN, LEFT
};
// Thêm sau phần khai báo enum Direction
enum Difficulty {
    EASY, MEDIUM, HARD
};

// Cấu hình tốc độ cho từng mức độ khó
const int SPEED_EASY = 7;    // Cập nhật 7 lần/giây
const int SPEED_MEDIUM = 12; // Cập nhật 12 lần/giây
const int SPEED_HARD = 18;   // Cập nhật 18 lần/giây

// Cấu hình tăng tốc độ sau mỗi lần ăn (ms)
const int SPEED_INCREASE_EASY = 3;    // Giảm 3ms mỗi lần ăn
const int SPEED_INCREASE_MEDIUM = 5;  // Giảm 5ms mỗi lần ăn
const int SPEED_INCREASE_HARD = 7;    // Giảm 7ms mỗi lần ăn

// Tốc độ tối đa (updateInterval nhỏ nhất) cho mỗi mức độ
const int MIN_UPDATE_INTERVAL_EASY = 100;    // Không nhanh hơn 100ms
const int MIN_UPDATE_INTERVAL_MEDIUM = 70;   // Không nhanh hơn 70ms
const int MIN_UPDATE_INTERVAL_HARD = 40;     // Không nhanh hơn 40ms

// Cấu trúc vị trí
struct Position {
    int x, y;
    
    bool operator==(const Position& other) const {
        return x == other.x && y == other.y;
    }
};

// Lớp con rắn
class Snake {
private:
    std::deque<Position> body;
    Direction direction;
    bool growing;

public:
    Snake(int x, int y) {
        direction = RIGHT;
        growing = false;
        
        // Khởi tạo thân rắn với 3 đốt
        body.push_back({x, y});
        body.push_back({x - GRID_SIZE, y});
        body.push_back({x - 2 * GRID_SIZE, y});
    }
    
    void setDirection(Direction dir) {
        // Tránh quay đầu 180 độ
        if ((direction == UP && dir != DOWN) ||
            (direction == DOWN && dir != UP) ||
            (direction == LEFT && dir != RIGHT) ||
            (direction == RIGHT && dir != LEFT)) {
            direction = dir;
        }
    }
      void move(Difficulty difficulty) {
        // Lấy vị trí đầu rắn
        Position head = body.front();
        
        // Tính toán vị trí đầu mới dựa trên hướng
        switch (direction) {
            case UP:    head.y -= GRID_SIZE; break;
            case DOWN:  head.y += GRID_SIZE; break;
            case LEFT:  head.x -= GRID_SIZE; break;
            case RIGHT: head.x += GRID_SIZE; break;
        }
        
        // Xử lý khi rắn đi ra ngoài màn hình dựa trên mức độ khó
        if (difficulty == EASY) {
            // Ở chế độ EASY, rắn có thể xuyên tường
            if (head.x < 0) 
                head.x = SCREEN_WIDTH - GRID_SIZE;
            else if (head.x >= SCREEN_WIDTH) 
                head.x = 0;
            
            if (head.y < 0) 
                head.y = SCREEN_HEIGHT - GRID_SIZE;
            else if (head.y >= SCREEN_HEIGHT) 
                head.y = 0;
        }
        // Ở MEDIUM và HARD, rắn sẽ dừng lại ở tường (kiểm tra va chạm được thực hiện riêng)
        
        // Thêm đầu mới vào thân rắn
        body.push_front(head);
        
        // Nếu không phải đang phát triển, xóa đuôi
        if (!growing) {
            body.pop_back();
        } else {
            growing = false;
        }
    }
    void grow() {
        growing = true;
    }
      // Kiểm tra va chạm với tường
    bool checkWallCollision(Difficulty difficulty) const {
        // Ở chế độ EASY, rắn có thể xuyên tường
        if (difficulty == EASY) {
            return false;
        }
        
        // Ở chế độ MEDIUM và HARD, rắn sẽ va chạm với tường
        Position head = body.front();
        return (head.x < 0 || head.x >= SCREEN_WIDTH || head.y < 0 || head.y >= SCREEN_HEIGHT);
    }
    
    // Kiểm tra va chạm với thân rắn
    bool checkSelfCollision() const {
        Position head = body.front();
        for (size_t i = 1; i < body.size(); ++i) {
            if (head == body[i]) {
                return true;
            }
        }
        return false;
    }
    
    // Kiểm tra va chạm với thức ăn
    bool checkFoodCollision(const Position& food) const {
        return body.front() == food;
    }
      // Vẽ rắn lên màn hình
    void render(SDL_Renderer* renderer, bool gameOver = false) const {
    // Debug log: in ra vị trí đầu rắn (chỉ khi không game over)
    // gameOver --> không log
        static int frameCount = 0;
        if (!gameOver && frameCount++ % 100 == 0) { // Log mỗi 100 frames để tránh spam console
            std::cout << "Snake head position: x=" << body.front().x << ", y=" << body.front().y 
                    << ", Snake size: " << body.size() << std::endl;
        }
        
        // Vẽ đầu rắn với màu khác
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Đầu màu xanh lá đậm hơn
        SDL_FRect headRect = {
            static_cast<float>(body.front().x),
            static_cast<float>(body.front().y),
            static_cast<float>(GRID_SIZE),
            static_cast<float>(GRID_SIZE)
        };
        SDL_RenderFillRect(renderer, &headRect);

        // Vẽ thân rắn
        SDL_SetRenderDrawColor(renderer, 0, 200, 0, 255); // Thân màu xanh lá nhạt hơn
        for (size_t i = 1; i < body.size(); ++i) {
            SDL_FRect rect = {
                static_cast<float>(body[i].x),
                static_cast<float>(body[i].y),
                static_cast<float>(GRID_SIZE),
                static_cast<float>(GRID_SIZE)
            };
            SDL_RenderFillRect(renderer, &rect);
        }
    }
      // Lấy kích thước rắn
    size_t size() const {
        return body.size();
    }
    
    // Lấy vị trí của phần thân
    Position getBodyPosition(size_t index) const {
        if (index < body.size()) {
            return body[index];
        }
        // Trả về vị trí không hợp lệ nếu index nằm ngoài phạm vi
        return {-1, -1};
    }
};

// Hàm tạo vị trí thức ăn ngẫu nhiên
Position generateFood(const Snake& snake) {
    static std::mt19937 rng(static_cast<unsigned int>(std::time(nullptr)));
    
    int maxX = SCREEN_WIDTH / GRID_SIZE - 1;
    int maxY = SCREEN_HEIGHT / GRID_SIZE - 1;
    
    std::uniform_int_distribution<int> distX(0, maxX);
    std::uniform_int_distribution<int> distY(0, maxY);
    
    Position food;
    bool valid;
    int attempts = 0;
    const int maxAttempts = 100; // Giới hạn số lần thử để tránh vòng lặp vô hạn
    
    // Tạo vị trí ngẫu nhiên cho thức ăn cho đến khi hợp lệ (không trùng với thân rắn)
    do {
        valid = true;
        food.x = distX(rng) * GRID_SIZE;
        food.y = distY(rng) * GRID_SIZE;
        
        // Kiểm tra xem thức ăn có nằm trên thân rắn không
        for (size_t i = 0; i < snake.size(); ++i) {
            Position pos = snake.getBodyPosition(i);
            if (food.x == pos.x && food.y == pos.y) {
                valid = false;
                break;
            }
        }
        
        attempts++;            
        if (attempts >= maxAttempts) {
            // Nếu không thể tìm vị trí sau nhiều lần thử, đặt vị trí xa khỏi rắn
            food.x = (snake.getBodyPosition(0).x + SCREEN_WIDTH / 2) % SCREEN_WIDTH;
            food.y = (snake.getBodyPosition(0).y + SCREEN_HEIGHT / 2) % SCREEN_HEIGHT;
            food.x = (food.x / GRID_SIZE) * GRID_SIZE;
            food.y = (food.y / GRID_SIZE) * GRID_SIZE;
            std::cout << "Reached max attempts, placing food at x=" << food.x << ", y=" << food.y << std::endl;
            break;
        }
    } while (!valid);
    
    std::cout << "Food created at: x=" << food.x << ", y=" << food.y << " after " << attempts << " attempts" << std::endl;
    return food;
}

// Hàm để thiết lập tốc độ dựa trên mức độ khó
void setDifficultySettings(Difficulty difficulty, Uint32& updateInterval) {
    switch (difficulty) {
        case EASY:
            SNAKE_SPEED = SPEED_EASY;
            updateInterval = 1000 / SNAKE_SPEED;
            std::cout << "Difficulty: EASY - Initial speed: " << SNAKE_SPEED << " updates/sec" << std::endl;
            break;
        case MEDIUM:
            SNAKE_SPEED = SPEED_MEDIUM;
            updateInterval = 1000 / SNAKE_SPEED;
            std::cout << "Difficulty: MEDIUM - Initial speed: " << SNAKE_SPEED << " updates/sec" << std::endl;
            break;
        case HARD:
            SNAKE_SPEED = SPEED_HARD;
            updateInterval = 1000 / SNAKE_SPEED;
            std::cout << "Difficulty: HARD - Initial speed: " << SNAKE_SPEED << " updates/sec" << std::endl;
            break;
    }
}

// Hàm để lấy giới hạn tốc độ dựa trên mức độ khó
int getMinUpdateInterval(Difficulty difficulty) {
    switch (difficulty) {
        case EASY:    return MIN_UPDATE_INTERVAL_EASY;
        case MEDIUM:  return MIN_UPDATE_INTERVAL_MEDIUM;
        case HARD:    return MIN_UPDATE_INTERVAL_HARD;
        default:      return MIN_UPDATE_INTERVAL_EASY;
    }
}

// Hàm để lấy mức tăng tốc độ dựa trên mức độ khó
int getSpeedIncrease(Difficulty difficulty) {
    switch (difficulty) {
        case EASY:    return SPEED_INCREASE_EASY;
        case MEDIUM:  return SPEED_INCREASE_MEDIUM;
        case HARD:    return SPEED_INCREASE_HARD;
        default:      return SPEED_INCREASE_EASY;
    }
}

// Hàm hiển thị và xử lý menu chọn mức độ khó
bool showDifficultyMenu(SDL_Renderer* renderer, Difficulty& selectedDifficulty) {
    bool menuActive = true;
    SDL_Event menuEvent;
    
    std::cout << "Select difficulty:" << std::endl;
    std::cout << "1 - EASY: Slower speed, gentle acceleration" << std::endl;
    std::cout << "2 - MEDIUM: Medium speed, moderate acceleration" << std::endl;
    std::cout << "3 - HARD: Fast speed, quick acceleration" << std::endl;
    std::cout << "Press ESC to exit the menu and quit the game." << std::endl;
    
    while (menuActive) {
        // Xử lý sự kiện trong menu
        while (SDL_PollEvent(&menuEvent)) {
            if (menuEvent.type == SDL_EVENT_QUIT) {
                return false; // Thoát game
            } else if (menuEvent.type == SDL_EVENT_KEY_DOWN) {
                switch (menuEvent.key.key) {
                    case SDLK_1:
                    case SDLK_KP_1:
                        selectedDifficulty = EASY;
                        menuActive = false;
                        break;
                    case SDLK_2:
                    case SDLK_KP_2:
                        selectedDifficulty = MEDIUM;
                        menuActive = false;
                        break;
                    case SDLK_3:
                    case SDLK_KP_3:
                        selectedDifficulty = HARD;
                        menuActive = false;
                        break;
                    case SDLK_ESCAPE:
                        return false; // Thoát game
                }
            }
        }
        
        // Vẽ menu
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        
        // Vẽ các lựa chọn menu
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        
        // Vẽ "EASY" option
        SDL_FRect easyRect = {
            static_cast<float>(SCREEN_WIDTH / 4),
            static_cast<float>(SCREEN_HEIGHT / 4),
            static_cast<float>(SCREEN_WIDTH / 2),
            static_cast<float>(SCREEN_HEIGHT / 10)
        };
        if (selectedDifficulty == EASY) {
            SDL_RenderFillRect(renderer, &easyRect);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        } else {
            SDL_RenderRect(renderer, &easyRect);
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        }
        
        // Vẽ "MEDIUM" option
        SDL_FRect mediumRect = {
            static_cast<float>(SCREEN_WIDTH / 4),
            static_cast<float>(SCREEN_HEIGHT / 4 + SCREEN_HEIGHT / 8),
            static_cast<float>(SCREEN_WIDTH / 2),
            static_cast<float>(SCREEN_HEIGHT / 10)
        };
        if (selectedDifficulty == MEDIUM) {
            SDL_RenderFillRect(renderer, &mediumRect);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        } else {
            SDL_RenderRect(renderer, &mediumRect);
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        }
        
        // Vẽ "HARD" option
        SDL_FRect hardRect = {
            static_cast<float>(SCREEN_WIDTH / 4),
            static_cast<float>(SCREEN_HEIGHT / 4 + 2 * SCREEN_HEIGHT / 8),
            static_cast<float>(SCREEN_WIDTH / 2),
            static_cast<float>(SCREEN_HEIGHT / 10)
        };
        if (selectedDifficulty == HARD) {
            SDL_RenderFillRect(renderer, &hardRect);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        } else {
            SDL_RenderRect(renderer, &hardRect);
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        }
        
        SDL_RenderPresent(renderer);
        SDL_Delay(16); // ~60 FPS
    }
    
    return true; // Tiếp tục game
}

int main(int argc, char* argv[]) {    
    std::cout << "=== SNAKE GAME WITH SDL3 ===" << std::endl;
    std::cout << "Controls: Use arrow keys or WASD to move the snake" << std::endl;
    std::cout << "         After Game Over: Press ENTER to restart" << std::endl;
    std::cout << "         Press ESC to exit game" << std::endl;
    std::cout << "==============================" << std::endl;
    
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    Difficulty currentDifficulty = EASY; // Mặc định bắt đầu ở mức độ dễ
    // Khởi tạo SDL3
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "SDL không khởi tạo được! Lỗi: " << SDL_GetError() << std::endl;
        return -1;
    }

    // Tạo cửa sổ với SDL3 (không cần SDL_WINDOW_SHOWN, cửa sổ sẽ hiển thị theo mặc định)
    window = SDL_CreateWindow("Snake Game", SCREEN_WIDTH, SCREEN_HEIGHT, 0); // 0 có thể thay bằng các cờ khác nếu cần
    // Đặt vị trí cửa sổ vào giữa màn hình
    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

    if (!window) {
        std::cout << "Không tạo được cửa sổ! Lỗi: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }
    
    // Tạo renderer với SDL3 (không cần cờ renderer nữa, chỉ cần window và tên renderer hoặc NULL)
    renderer = SDL_CreateRenderer(window, NULL); // NULL để sử dụng renderer mặc định
    if (!renderer) {
        std::cout << "Không tạo được renderer! Lỗi: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // Hiển thị menu chọn mức độ khó
    if (!showDifficultyMenu(renderer, currentDifficulty)) {
        // Nếu người chơi chọn thoát từ menu, thoát game
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 0;
    }
    
    // Thiết lập tốc độ ban đầu dựa trên mức độ khó đã chọn
    Uint32 updateInterval;
    setDifficultySettings(currentDifficulty, updateInterval);

    // Khởi tạo rắn tại vị trí giữa màn hình
    Snake snake(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
      // Tạo thức ăn đầu tiên
    Position food = generateFood(snake);
    std::cout << "First food created at: x=" << food.x << ", y=" << food.y << std::endl;
    
    // Khởi tạo các biến cho game loop
    bool isRunning = true;
    bool gameOver = false;
    SDL_Event event;    // Biến theo dõi thời gian và tốc độ của rắn
    Uint32 lastUpdateTime = SDL_GetTicks();
    Uint32 frameStartTime;
      // Biến điểm số và màu sắc
    int score = 0;
    int hueShift = 0; // Biến để tạo hiệu ứng đổi màu
    
    while (isRunning) {
        // Đặt thời điểm bắt đầu frame
        frameStartTime = SDL_GetTicks();
        
        // Xử lý sự kiện
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                isRunning = false;
            }            else if (event.type == SDL_EVENT_KEY_DOWN) {                if (!gameOver) {                    switch (event.key.key) {
                        case SDLK_UP:
                        // case SDLK_w:
                        case SDLK_W:
                            snake.setDirection(UP);
                            break;
                        case SDLK_DOWN:
                        // case SDLK_s:
                        case SDLK_S:
                            snake.setDirection(DOWN);
                            break;
                        case SDLK_LEFT:
                        // case SDLK_a:
                        case SDLK_A:
                            snake.setDirection(LEFT);
                            break;
                        case SDLK_RIGHT:
                        // case SDLK_d:
                        case SDLK_D:
                            snake.setDirection(RIGHT);
                            break;
                        case SDLK_ESCAPE:
                            isRunning = false;
                            std::cout << "Exiting game!" << std::endl;
                            break;
                    }                }else if (event.key.key == SDLK_RETURN) {
                    // Khởi động lại trò chơi khi nhấn Enter sau khi game over
                    std::cout << "==== RESTARTING GAME ====" << std::endl;
                    
                    // Hiển thị menu chọn mức độ khó
                    if (!showDifficultyMenu(renderer, currentDifficulty)) {
                        isRunning = false;
                        continue;
                    }
                    
                    snake = Snake(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
                    food = generateFood(snake);
                    gameOver = false;
                    score = 0;
                    
                    // Thiết lập tốc độ ban đầu dựa trên mức độ khó đã chọn
                    setDifficultySettings(currentDifficulty, updateInterval);
                    std::cout << "Game has been restarted with difficulty: ";
                    switch (currentDifficulty) {
                        case EASY: std::cout << "EASY"; break;
                        case MEDIUM: std::cout << "MEDIUM"; break;
                        case HARD: std::cout << "HARD"; break;
                    }
                    std::cout << std::endl;
                }
            }
        }        // Cập nhật trạng thái trò chơi
        Uint32 currentTime = SDL_GetTicks();
        if (!gameOver && currentTime - lastUpdateTime >= updateInterval) {
            // Di chuyển rắn
            snake.move(currentDifficulty);
            
            // Kiểm tra va chạm với tường
            if (snake.checkWallCollision(currentDifficulty)) {
                gameOver = true;
                std::cout << "Game over: Wall collision!" << std::endl;
            }
            
            // Kiểm tra va chạm với thân rắn
            if (snake.checkSelfCollision()) {
                gameOver = true;
                std::cout << "Game over: Self collision!" << std::endl;
            }
            
            // Kiểm tra va chạm với thức ăn
            if (snake.checkFoodCollision(food)) {                
                snake.grow();
                food = generateFood(snake);
                score++;
                std::cout << "Food eaten! Score: " << score << std::endl;
                
                // Tăng tốc độ theo mức độ khó
                int minInterval = getMinUpdateInterval(currentDifficulty);
                int speedIncrease = getSpeedIncrease(currentDifficulty);
                
                if (updateInterval > minInterval) {
                    updateInterval -= speedIncrease;
                    if (updateInterval < minInterval) {
                        updateInterval = minInterval;
                    }
                    std::cout << "Speed increased! Update every " << updateInterval << "ms" << std::endl;
                }
            }
            
            lastUpdateTime = currentTime;
        }        // Vẽ lại màn hình
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Nền đen
        SDL_RenderClear(renderer);
          // Vẽ lưới (tùy chọn)
        SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
        for (int x = 0; x < SCREEN_WIDTH; x += GRID_SIZE) {
            SDL_RenderLine(renderer, x, 0, x, SCREEN_HEIGHT);
        }
        for (int y = 0; y < SCREEN_HEIGHT; y += GRID_SIZE) {
            SDL_RenderLine(renderer, 0, y, SCREEN_WIDTH, y);
        }
        // Vẽ tường cho chế độ MEDIUM và HARD
        if (currentDifficulty == MEDIUM || currentDifficulty == HARD) {
            // Màu của tường: Xám cho MEDIUM, Đỏ gạch cho HARD
            if (currentDifficulty == MEDIUM) {
                SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255); // Xám
            } else {
                SDL_SetRenderDrawColor(renderer, 180, 60, 60, 255); // Đỏ gạch
            }
            
            // Vẽ tường trên
            SDL_FRect topWall = {0, 0, static_cast<float>(SCREEN_WIDTH), static_cast<float>(GRID_SIZE/2)};
            SDL_RenderFillRect(renderer, &topWall);
            
            // Vẽ tường dưới
            SDL_FRect bottomWall = {0, static_cast<float>(SCREEN_HEIGHT - GRID_SIZE/2), 
                                   static_cast<float>(SCREEN_WIDTH), static_cast<float>(GRID_SIZE/2)};
            SDL_RenderFillRect(renderer, &bottomWall);
            
            // Vẽ tường trái
            SDL_FRect leftWall = {0, 0, static_cast<float>(GRID_SIZE/2), static_cast<float>(SCREEN_HEIGHT)};
            SDL_RenderFillRect(renderer, &leftWall);
            
            // Vẽ tường phải
            SDL_FRect rightWall = {static_cast<float>(SCREEN_WIDTH - GRID_SIZE/2), 0, 
                                  static_cast<float>(GRID_SIZE/2), static_cast<float>(SCREEN_HEIGHT)};
            SDL_RenderFillRect(renderer, &rightWall);
        }
        
        // Vẽ thức ăn với màu đỏ đơn giản
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_FRect foodRect = {
            static_cast<float>(food.x),
            static_cast<float>(food.y),
            static_cast<float>(GRID_SIZE),
            static_cast<float>(GRID_SIZE)
        };
        SDL_RenderFillRect(renderer, &foodRect);
          // Vẽ rắn
        snake.render(renderer, gameOver);
        
        // Vẽ điểm số
        // (Trong SDL3 không có hàm vẽ text trực tiếp, trong game thực tế bạn nên sử dụng SDL_ttf)
        if (gameOver) {
            // Hiển thị Game Over
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            
            // Vẽ viền chữ "GAME OVER"
            SDL_FRect gameOverRect = {
                static_cast<float>(SCREEN_WIDTH / 4),
                static_cast<float>(SCREEN_HEIGHT / 3),
                static_cast<float>(SCREEN_WIDTH / 2),
                static_cast<float>(SCREEN_HEIGHT / 6)
            };
            SDL_RenderRect(renderer, &gameOverRect);
            
            // Vẽ "ENTER to restart" bằng hình chữ nhật
            SDL_FRect restartRect = {
                static_cast<float>(SCREEN_WIDTH / 3),
                static_cast<float>(SCREEN_HEIGHT / 2),
                static_cast<float>(SCREEN_WIDTH / 3),
                static_cast<float>(SCREEN_HEIGHT / 10)
            };
            SDL_RenderRect(renderer, &restartRect);        } else {
            // Hiển thị mức độ khó
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_FRect difficultyRect = {
                static_cast<float>(SCREEN_WIDTH - 100),
                static_cast<float>(10),
                80.0f,
                20.0f
            };
            SDL_RenderRect(renderer, &difficultyRect);
            
            // Tô màu khác nhau cho từng mức độ
            switch (currentDifficulty) {
                case EASY:
                    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 128); // Xanh lá cho EASY
                    break;
                case MEDIUM:
                    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 128); // Vàng cho MEDIUM
                    break;
                case HARD:
                    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 128); // Đỏ cho HARD
                    break;
            }
            
            // Tô màu cho phần bên trong
            SDL_FRect difficultyFillRect = {
                static_cast<float>(SCREEN_WIDTH - 98),
                static_cast<float>(12),
                76.0f,
                16.0f
            };
            SDL_RenderFillRect(renderer, &difficultyFillRect);
            
            // Hiển thị điểm số bằng cách vẽ một hình chữ nhật đại diện cho điểm
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
            for (int i = 0; i < score; i++) {
                SDL_FRect scoreRect = {
                    static_cast<float>(10 + i * 15),
                    static_cast<float>(10),
                    10.0f,
                    10.0f
                };
                SDL_RenderFillRect(renderer, &scoreRect);
            }
        }
          SDL_RenderPresent(renderer);

        // Tính toán thời gian đã trôi qua trong frame và chờ để đạt 60 FPS
        Uint32 frameTime = SDL_GetTicks() - frameStartTime;
        if (frameTime < 16) { // 16ms ~ 60 FPS
            SDL_Delay(16 - frameTime);
        }
    }    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
