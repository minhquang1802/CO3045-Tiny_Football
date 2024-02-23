#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <cmath>

using namespace std;

// Kích thước cửa sổ
const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;
const int PLAYER_RADIUS = 15;

// Kích thước sân bóng
const int FIELD_WIDTH = 1100;
const int FIELD_HEIGHT = 600;

const int FIELD_X = (WINDOW_WIDTH - FIELD_WIDTH) / 2; // từ bên trái đến cạnh trái sân bóng
const int FIELD_Y = (WINDOW_HEIGHT - FIELD_HEIGHT - 30); //từ đỉnh xuống cạnh trên sân bóng

// giới hạn FPS
const int FPS = 60;
const int frameDelay = 1000 / FPS;

// khung thành
const int GOAL_WIDTH = 30;
const int GOAL_HEIGHT = 150;
const int GOAL_Y = (FIELD_HEIGHT - GOAL_HEIGHT) / 2;

SDL_Rect goal1 = {FIELD_X-20, FIELD_Y + GOAL_Y, GOAL_WIDTH, GOAL_HEIGHT};
SDL_Rect goal2 = {FIELD_X + FIELD_WIDTH - GOAL_WIDTH + 20, FIELD_Y + GOAL_Y, GOAL_WIDTH, GOAL_HEIGHT};

Uint32 frameStart;
int frameTime;

struct Player
{
    int x, y;
    int dx, dy;
    float speed;
    SDL_Color color;
};

void movePlayer1(Player &player, Player &otherPlayer, const Uint8 *keys) 
{
    int newX = player.x;
    int newY = player.y;

    if (keys[SDL_SCANCODE_W] && player.y - player.speed >= FIELD_Y + PLAYER_RADIUS - 30) newY -= player.speed;
    if (keys[SDL_SCANCODE_S] && player.y + player.speed <= FIELD_Y + FIELD_HEIGHT - PLAYER_RADIUS + 30) newY += player.speed;
    if (keys[SDL_SCANCODE_A] && player.x - player.speed >= FIELD_X + PLAYER_RADIUS -30) newX -= player.speed;
    if (keys[SDL_SCANCODE_D] && player.x + player.speed <= FIELD_X + FIELD_WIDTH - PLAYER_RADIUS + 30) newX += player.speed;

    int dx = newX - otherPlayer.x;
    int dy = newY - otherPlayer.y;
    int distance = sqrt(dx*dx + dy*dy);

    if (distance >= PLAYER_RADIUS * 2) 
    {
        player.x = newX;
        player.y = newY;
    }
}

void movePlayer2(Player &player, Player &otherPlayer, const Uint8 *keys) 
{
    int newX = player.x;
    int newY = player.y;

    if (keys[SDL_SCANCODE_UP] && player.y - player.speed >= FIELD_Y + PLAYER_RADIUS - 30) newY -= player.speed;
    if (keys[SDL_SCANCODE_DOWN] && player.y + player.speed <= FIELD_Y + FIELD_HEIGHT - PLAYER_RADIUS + 30) newY += player.speed;
    if (keys[SDL_SCANCODE_LEFT] && player.x - player.speed >= FIELD_X + PLAYER_RADIUS - 30) newX -= player.speed;
    if (keys[SDL_SCANCODE_RIGHT] && player.x + player.speed <= FIELD_X + FIELD_WIDTH - PLAYER_RADIUS + 30) newX += player.speed;

    int dx = newX - otherPlayer.x;
    int dy = newY - otherPlayer.y;
    int distance = sqrt(dx*dx + dy*dy);

    if (distance >= PLAYER_RADIUS * 2) 
    {
        player.x = newX;
        player.y = newY;
    }
}

void moveBall(Player &ball) 
{
    int newX = ball.x + ball.dx * ball.speed;
    int newY = ball.y + ball.dy * ball.speed;

    // Kiểm tra va chạm với biên
    if (newX - PLAYER_RADIUS < FIELD_X || newX + PLAYER_RADIUS > FIELD_X + FIELD_WIDTH) 
        ball.dx *= -1; // Reverse horizontal direction
    else 
        ball.x = newX;

    if (newY - PLAYER_RADIUS < FIELD_Y || newY + PLAYER_RADIUS > FIELD_Y + FIELD_HEIGHT)
        ball.dy *= -1; // Reverse vertical direction
    else
        ball.y = newY;

    // Giảm tốc độ bóng theo thời gian
    ball.speed *= 0.95;

    // Dừng hẳn bóng nếu tốc độ nhỏ hơn 0.08
    if (ball.speed < 0.08)
        ball.speed = 0;
}

void drawCircle(SDL_Renderer *renderer, int centerX, int centerY, int radius) 
{
    for (int w = 0; w < radius * 2; w++) 
    {
        for (int h = 0; h < radius * 2; h++) 
        {
            int dx = radius - w;
            int dy = radius - h;
            if ((dx*dx + dy*dy) <= (radius * radius)) 
            {
                SDL_RenderDrawPoint(renderer, centerX + dx, centerY + dy);
            }
        }
    }
}

void drawPlayer(SDL_Renderer *renderer, Player &player)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    drawCircle(renderer, player.x, player.y, PLAYER_RADIUS + 3);

    SDL_SetRenderDrawColor(renderer, player.color.r, player.color.g, player.color.b, player.color.a);
    drawCircle(renderer, player.x, player.y, PLAYER_RADIUS);
}

bool checkCollision(Player &player, Player &ball) 
{
    int dx = player.x - ball.x;
    int dy = player.y - ball.y;
    int distance = sqrt(dx*dx + dy*dy);

    if (distance < PLAYER_RADIUS * 2) 
    {
        // Tốc độ khi bóng va chạm với người chơi
        ball.speed = 4;
        return true;
    }

    return false;
}

bool checkGoal(SDL_Rect &goal, Player &ball) 
{
    if (ball.x - PLAYER_RADIUS < goal.x + goal.w &&
        ball.x + PLAYER_RADIUS > goal.x &&
        ball.y - PLAYER_RADIUS < goal.y + goal.h &&
        ball.y + PLAYER_RADIUS > goal.y) 
    {
        return true;
    }
    return false;
}


int main(int argc, char** argv)
{   
    // Khởi tạo window
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow("Tiny Football", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    Player player1 = {WINDOW_WIDTH / 4, WINDOW_HEIGHT / 2, 0, 0, 4, {232,109,82,255}};
    Player player2 = {WINDOW_WIDTH * 3 / 4, WINDOW_HEIGHT / 2, 0, 0, 4, {85,137,227,255}};

    Player ball = {WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, 0, 0, 2, {255, 255, 255, 255}};

    // Khởi tạo SDL_ttf
    if (TTF_Init() == -1) 
    {
        cout << "TTF_Init: " << TTF_GetError() << endl;
        exit(2);
    }

    // Load font
    TTF_Font* font = TTF_OpenFont("./font/AldotheApache.ttf", 24);
    if (font == NULL) 
    {
        cout << "TTF_OpenFont: " << TTF_GetError() << endl;
    }

    bool isRunning = true;
    int player2Score =0, player1Score = 0;

    Uint32 gameDuration = 60;

    Uint32 start = SDL_GetTicks();

    while (isRunning) 
    {
        frameStart = SDL_GetTicks();

        SDL_Event event;
        while (SDL_PollEvent(&event)) 
        {
            if (event.type == SDL_QUIT) 
            {
                isRunning = false;
            }
        }

        const Uint8 *keys = SDL_GetKeyboardState(NULL);
        movePlayer1(player1, player2, keys);
        movePlayer2(player2, player1, keys);
        moveBall(ball);

        if (checkCollision(player1, ball)) 
        {
            int dx = ball.x - player1.x;
            int dy = ball.y - player1.y;
            int distance = sqrt(dx*dx + dy*dy);

            // Normalize the direction vector (dx, dy) and scale it by the ball's speed
            ball.dx = ball.speed * dx / distance;
            ball.dy = ball.speed * dy / distance;
        }
        if (checkCollision(player2, ball)) 
        {
            int dx = ball.x - player2.x;
            int dy = ball.y - player2.y;
            int distance = sqrt(dx*dx + dy*dy);

            // Normalize the direction vector (dx, dy) and scale it by the ball's speed
            ball.dx = ball.speed * dx / distance;
            ball.dy = ball.speed * dy / distance;
        }

        if (checkGoal(goal1, ball)) 
        {
            player2Score++;
            // Thiết lập lại bóng
            ball.x = WINDOW_WIDTH / 2;
            ball.y = WINDOW_HEIGHT / 2;
            ball.dx = 0;
            ball.dy = 0;
            // Thiết lập vị trí người chơi
            player1.x = WINDOW_WIDTH / 4;
            player1.y = WINDOW_HEIGHT / 2;
            player2.x = WINDOW_WIDTH * 3 / 4;
            player2.y = WINDOW_HEIGHT / 2;
        }
        if (checkGoal(goal2, ball)) 
        {
            player1Score++;
            // Thiết lập lại bóng
            ball.x = WINDOW_WIDTH / 2;
            ball.y = WINDOW_HEIGHT / 2;
            ball.dx = 0;
            ball.dy = 0;
            // Thiết lập vị trí người chơi
            player1.x = WINDOW_WIDTH / 4;
            player1.y = WINDOW_HEIGHT / 2;
            player2.x = WINDOW_WIDTH * 3 / 4;
            player2.y = WINDOW_HEIGHT / 2;
        }
        SDL_RenderClear(renderer);

        // Vẽ sân bóng
        SDL_SetRenderDrawColor(renderer, 255,255,255,255);
        SDL_Rect field_border = {FIELD_X-3, FIELD_Y-3, FIELD_WIDTH+6, FIELD_HEIGHT+6};
        SDL_RenderFillRect(renderer, &field_border);

        SDL_SetRenderDrawColor(renderer, 151,180,145,255);
        SDL_Rect field = {FIELD_X, FIELD_Y, FIELD_WIDTH, FIELD_HEIGHT};
        SDL_RenderFillRect(renderer, &field);


        //vẽ player và ball
        drawPlayer(renderer, player1);
        drawPlayer(renderer, player2);
        drawPlayer(renderer, ball);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(renderer, &goal1);
        SDL_RenderFillRect(renderer, &goal2);

        // Màu background
        SDL_SetRenderDrawColor(renderer, 113,139,91,255);

        // Create a string with the scores
        char score_text[50];
        sprintf(score_text, "Player 1: %d - Player 2: %d", player1Score, player2Score);

        // Render the text to a surface
        SDL_Color textColor = {0, 0, 0, 255};
        SDL_Surface* textSurface = TTF_RenderText_Solid(font, score_text, textColor);

        SDL_Texture* text = SDL_CreateTextureFromSurface(renderer, textSurface);
        int text_width = textSurface->w;
        int text_height = textSurface->h;
        SDL_FreeSurface(textSurface);

        // Vị trí bảng điểm
        SDL_Rect textRect = {WINDOW_WIDTH / 2 - text_width / 2, 30, text_width, text_height};

        // Render the text
        SDL_RenderCopy(renderer, text, NULL, &textRect);

        // Get the current time
        Uint32 now = SDL_GetTicks();

        // Calculate the elapsed time in seconds
        Uint32 elapsed = (now - start) / 1000;

        // Thời gian còn lại
        Uint32 remaining = gameDuration - elapsed;

        // Convert the remaining time to a string
        string timerText = "Time remaining: " + std::to_string(remaining) + " seconds";

        // Render the timer text
        SDL_Surface* surfaceMessage = TTF_RenderText_Solid(font, timerText.c_str(), textColor);
        SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

        SDL_Rect Message_rect; //create a rect
        Message_rect.x = 5;  //controls the rect's x coordinate 
        Message_rect.y = 5; // controls the rect's y coordinte
        Message_rect.w = 200; // controls the width of the rect
        Message_rect.h = 30; // controls the height of the rect

        // Render the message to the window
        SDL_RenderCopy(renderer, Message, NULL, &Message_rect);

        SDL_FreeSurface(surfaceMessage);

        if (remaining <= 0)
        {
            isRunning = false;
            // Determine the winner
            string message;
            if (player1Score > player2Score)
            {
                message = "Player 1 wins!";
            }
            else if (player2Score > player1Score)
            {
                message = "Player 2 wins!";
            }
            else
            {
                message = "It's a draw!";
            }

            // Hiển thị kết quả
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION,
                                    "Game Over",
                                    message.c_str(),
                                    NULL);
        
        }

        SDL_RenderPresent(renderer);

        // Don't forget to destroy the texture
        SDL_DestroyTexture(text);
        SDL_DestroyTexture(Message);

        //chỉnh frametime
        frameTime = SDL_GetTicks() - frameStart;

        if (frameDelay > frameTime) 
        {
            SDL_Delay(frameDelay - frameTime);
        }
    }

    TTF_CloseFont(font);
    TTF_Quit();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();

    return 0;
}