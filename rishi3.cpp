#include <raylib.h>
#include <iostream>
#include <string>
#include <ctime>
#include <cstdlib>
#include <cstring>
#include <vector> 

using namespace std;

Texture2D background;
Sound buttonClickSound;
void showMainMenu();

class Difficulty_Level {     
protected:
    const char** samples; 
    int num_of_samples;   

public:
    Difficulty_Level(const char** sample_array, int sample_count) {
        samples = sample_array;
        num_of_samples = sample_count;
    }

    const char* getRandomSample() {
        int index = rand() % num_of_samples;        
        return samples[index];
    }
    ~Difficulty_Level() {}                  
};

class Beginner : public Difficulty_Level {
private:
    static const char* beginner_samples[];

public:
    Beginner() : Difficulty_Level(beginner_samples, 5) {}
};

const char* Beginner::beginner_samples[] = {
    "The quick brown fox jumps over the lazy dog.",
    "Practice makes perfect.",
    "Hello world! Welcome to the typing speed test.",
    "Programming in C++ can be fun and educational.",
    "This is a simple typing speed test program."
};

class Intermediate : public Difficulty_Level {
private:
    static const char* intermediate_samples[];

public:
    Intermediate() : Difficulty_Level(intermediate_samples, 5) {}
};

const char* Intermediate::intermediate_samples[] = {
    "The rain in Spain stays mainly in the plain.",
    "A journey of a thousand miles begins with a single step.",
    "To be or not to be, that is the question.",
    "All that glitters is not gold.",
    "The only thing we have to fear is fear itself."
};

class Advanced : public Difficulty_Level {
private:
    static const char* advanced_samples[];

public:
    Advanced() : Difficulty_Level(advanced_samples, 5) {}
};

const char* Advanced::advanced_samples[] = {
    "In the middle of difficulty lies opportunity.",
    "Success is not final, failure is not fatal.",
    "It is our choices that show what we truly are.",
    "The best way to predict the future is to create it.",
    "Life is what happens when you're busy making other plans."
};

class TypingTest {
private:
    string userInput;
    bool typingFinished;

public:
    TypingTest() : typingFinished(false) {}

    void drawTypingScreen(const char* testText) {
        ClearBackground(RAYWHITE);
        
        DrawText("Typing Test", 700, 50, 40, DARKBLUE); 
        DrawText(testText, 400, 200, 30, BLACK); 
        DrawRectangle(500, 400, 600, 50, LIGHTGRAY);  
        DrawText(userInput.c_str(), 500, 410, 23, BLACK); 
    }
    
    void handleKeyBoard() {
        int key = GetCharPressed();
        while(key > 0) {
            if(key >= 32 && key <= 125) { 
                userInput += (char)key; 
            }
            key = GetCharPressed();             // Get the next character
        }
        if(IsKeyPressed(KEY_BACKSPACE) && !userInput.empty()) {
            userInput.pop_back(); 
        }
    }
    
    time_t startTest(const char* testText) {
        userInput.clear();
        typingFinished = false;
        time_t startTime = time(nullptr);  

        while(!typingFinished && !WindowShouldClose()) {
            BeginDrawing();

            drawTypingScreen(testText);

            if(IsKeyPressed(KEY_ENTER)) {
                typingFinished = true;           // Finish typing on Enter
            }
            handleKeyBoard();
            
            EndDrawing();
        }
        return startTime;
    }
    string getUserInput() const { 
        return userInput;
    }
};

class Results : public TypingTest {
public:
    static int countWords(const char* text) {
        int wordCount = 0;
        bool inWord = false;

        for(; *text; ++text) {
            if(isspace(*text)) {
                inWord = false;
            } 
            else if(!inWord) {
                inWord = true;
                wordCount++;
            }
        }
        return wordCount;
    }
    static void drowResultScreen(float wpm, float accuracy, int timeTaken) {
        DrawText("Test Completed!", 600, 100, 40, DARKBLUE); 
        DrawText(TextFormat("Words Per Minute: %.2f", wpm), 580, 250, 30, DARKGREEN); 
        DrawText(TextFormat("Accuracy: %.2f%%", accuracy), 580, 300, 30, DARKGREEN); 
        DrawText(TextFormat("Time Taken: %d seconds", timeTaken), 580, 350, 30, DARKGREEN); 
        DrawText("Press 'M' to return to the main menu.", 520, 500, 30, DARKGRAY); 
    }
    static void showWrongWords(const string& userInput, const char* testText) {
        const int startX = 530;
        const int startY = 430;
        const int charSpacing = 13; 
        int xPos = startX;

        for(size_t i=0; i < strlen(testText); ++i) {
            Color textColor = BLACK; 

            if(i < userInput.size()) {
                textColor = (testText[i] == userInput[i]) ? GREEN : RED;
            }

            DrawText(TextFormat("%c", testText[i]), xPos, startY, 25, textColor);
            xPos += charSpacing;
        }
    }   
    static void showResultScreen(const string& userInput, const char* testText, float wpm, float accuracy, int timeTaken) {
        bool viewResults = true;
        
        while(viewResults && !WindowShouldClose()) {
            BeginDrawing();
            ClearBackground(RAYWHITE);
            
            drowResultScreen(wpm, accuracy, timeTaken);
            showWrongWords(userInput, testText);

            if(IsKeyPressed(KEY_M)) {
                viewResults = false;                          // Exit the results screen
            }
            EndDrawing();
        }
    }

    static void calculateAndShowResults(const string& userInput, const char* testText, time_t startTime, time_t endTime) {
    int elapsedTime = (int)difftime(endTime, startTime);  
        int correctChars = 0;

        for(size_t i=0; i < userInput.size() && i < strlen(testText); i++) {
            if(userInput[i] == testText[i]) {
                correctChars++;
            }
        }
        float accuracy = (float)correctChars/strlen(testText) *100.0f;

        int wordCount = countWords(testText);

        float wpm = 0.0f;
        if (elapsedTime > 0) {
            float minutes = elapsedTime/60.0f;     // Convert seconds to minutes
            wpm = wordCount/minutes;               // Calculate WPM
        }
        showResultScreen(userInput, testText, wpm, accuracy, elapsedTime);
    }
};

class showLevel : public Results {
public:
    void drawDifficultyLevels() {
        DrawText("Select Difficulty Level", 570, 150, 50, MAROON); 
        DrawRectangle(650, 300, 300, 70, LIGHTGRAY); 
        DrawText("Beginner", 735, 320, 30, BLACK);  
        DrawRectangle(650, 400, 300, 70, LIGHTGRAY);
        DrawText("Intermediate", 700, 420, 30, BLACK); 
        DrawRectangle(650, 500, 300, 70, LIGHTGRAY);  
        DrawText("Advanced", 725, 520, 30, BLACK);   
    }

    Difficulty_Level* handleMouseInput() {
        Vector2 mouse = GetMousePosition();

        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if(mouse.x >= 650 && mouse.x <= 950 && mouse.y >= 300 && mouse.y <= 370) {
                return new Beginner();
            }
            else if(mouse.x >= 650 && mouse.x <= 950 && mouse.y >= 400 && mouse.y <= 470) {
                return new Intermediate(); 
            }
            else if(mouse.x >= 650 && mouse.x <= 950 && mouse.y >= 500 && mouse.y <= 570) {
                return new Advanced(); 
            }
        }
        return nullptr;
    }

    void showDifficultyLevels() {
        bool difficultyRunning = true;
        Difficulty_Level* selectedDifficulty = nullptr;

        while(difficultyRunning && !WindowShouldClose()) {
            BeginDrawing();
            ClearBackground(RAYWHITE);

            drawDifficultyLevels();                   // Draw the difficulty levels
            selectedDifficulty = handleMouseInput();  // Handle mouse input

            if (selectedDifficulty != nullptr) {
                difficultyRunning = false;            // Exit loop if difficulty is selected
            }
            EndDrawing();
        }
                                         
        if(selectedDifficulty != nullptr) {           // Set the selected test text and start the typing test
            const char* testText = selectedDifficulty->getRandomSample();
            time_t startTime = startTest(testText); 

            time_t endTime = time(nullptr);
            calculateAndShowResults(getUserInput(), testText, startTime, endTime); 

            delete selectedDifficulty;
        }
    }
};

class Words {
public:
    string text;
    float x, y, speed;

    Words(const string& word, float posX, float posY, float fallspeed) : text(word), x(posX), y(posY), speed(fallspeed) {}

    void Move(float deltaTime)  {
        y += speed * deltaTime;
    }

    bool IsOffScreen(int screenHeight) const  {
        return y > screenHeight;
    }
    ~Words() {}                          // Virtual destructor for polymorphism
    
};

class Game {  
protected:
    int screenWidth, screenHeight, score, highestScore;
    bool gameOver;

public:
    Game(int width, int height) 
        : screenWidth(width), screenHeight(height), score(0), highestScore(0), gameOver(false) {}

    virtual void UpdateRunningScreen() = 0;
    virtual void DrawRunningGameScreen() const = 0;
    virtual void Reset() = 0;
    
};

class TypingGame : public Game {
private:
    string userInput;
    Words* currentWord;  

    Sound gameOverSound, startGameSound;

public:
    TypingGame(int width, int height, Sound gameOverSnd, Sound startSnd) 
        : Game(width, height), gameOverSound(gameOverSnd), startGameSound(startSnd) {
        srand(static_cast<unsigned>(time(nullptr)));
        PlaySound(startGameSound);
        SpawnNewWord();
    }

    void SpawnNewWord() {
        const vector<string> wordList = {"hello", "world", "typing", "raylib", "game", "speed", "test", "destroy"};
        float posX = static_cast<float>(rand() % (screenWidth - 100));
        float fallSpeed = 100.0f + score * 10;
        currentWord = new Words(wordList[rand() % wordList.size()], posX, 0.0f, fallSpeed);
    }
    
    void handleKeyBoard() {
        int key = GetCharPressed();
        while(key > 0) {
            if(key >= 32 && key <= 126) {
                userInput += static_cast<char>(key);
            }
            key = GetCharPressed();
        }
        if(IsKeyPressed(KEY_BACKSPACE) && !userInput.empty()) {
            userInput.pop_back();
        }
    }

    void UpdateRunningScreen() override {
        if (gameOver) return;

        float deltaTime = GetFrameTime();
        currentWord->Move(deltaTime);                  // update the speed

        if(currentWord->IsOffScreen(screenHeight)) {   // if word falls down the game over
            PlaySound(gameOverSound);
            gameOver = true;
            if(score > highestScore) highestScore = score; 
        }
        handleKeyBoard();
        
        if(userInput == currentWord->text) {           // update the userinput
            score++;                                   // update score
            userInput = "";
            delete currentWord;
            SpawnNewWord();
        }
    }

    void DrawRunningGameScreen() const override {
        
        BeginDrawing();
        ClearBackground(BLACK);

            if(gameOver) {
                DrawText("GAME OVER!", screenWidth/2-100, screenHeight/2-30, 50, RED);
                DrawText(("Final Score: " + to_string(score)).c_str(), screenWidth/2-100, screenHeight/2+30, 30, WHITE);
                DrawText(("Highest Score: " + to_string(highestScore)).c_str(), screenWidth/2-100, screenHeight/2+70, 30, YELLOW);
                DrawText("Press [R] to Restart", screenWidth/2-100, screenHeight/2+120, 20, LIGHTGRAY);
                DrawText("Press [M] to Turn Off", screenWidth/2-100, screenHeight/2+150, 20, LIGHTGRAY);
            } 
            else {
                DrawText(currentWord->text.c_str(), currentWord->x, static_cast<int>(currentWord->y), 20, WHITE);
                DrawText(("Input: " + userInput).c_str(), 10, screenHeight - 50, 30, GOLD);
                DrawText(("Score: " + to_string(score)).c_str(), 10, 10, 30, GREEN);
                DrawText(("Highest Score: " + to_string(highestScore)).c_str(), screenWidth - 350, 10, 30, SKYBLUE);
            }
            EndDrawing();
    }

    void Reset() override {
        userInput = "";
        score = 0;
        gameOver = false;
        delete currentWord;
        SpawnNewWord();
        PlaySound(startGameSound);
    }

    void RestartOnInput() {
        if(gameOver && IsKeyPressed(KEY_R)) {
            Reset();
        }
    }
        
    void showGameScreen() {
    
        while(!WindowShouldClose()) {
           UpdateRunningScreen();
           RestartOnInput();
           DrawRunningGameScreen();
            if(IsKeyPressed(KEY_M)) {
                UnloadSound(gameOverSound);
                UnloadSound(startGameSound);
                showMainMenu();                
            }
        }
        CloseWindow();
    }

    ~TypingGame() {
        delete currentWord; 
    }
};


void drowMainMenu() {
    
    DrawText("Main Menu", 650, 100, 60, DARKBLUE);
    DrawRectangle(590, 300, 420, 70, DARKGREEN);  
    DrawText("Typing Speed Test", 610, 310, 40, BLACK);
    DrawRectangle(650, 400, 300, 70, DARKGREEN); 
    DrawText("Game", 750, 420, 40, BLACK); 
}

void showMainMenu() {
    while (!WindowShouldClose()) {
        bool typingSpeedSelected = false;
        bool gameSelected = false;

        while (!typingSpeedSelected && !gameSelected && !WindowShouldClose()) {
            BeginDrawing();
            ClearBackground(RAYWHITE);
            
            DrawTexture(background, 0, 0, WHITE);
 
            drowMainMenu();

            Vector2 mouse = GetMousePosition();

            if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                PlaySound(buttonClickSound);
                if(mouse.x >= 590 && mouse.x <= 1010 && mouse.y >= 300 && mouse.y <= 370) {
                    typingSpeedSelected = true;
                } 
                else if (mouse.x >= 650 && mouse.x <= 950 && mouse.y >= 400 && mouse.y <= 470) {
                    gameSelected = true;
                }
            }
            EndDrawing();
        }
        if(typingSpeedSelected) {              // Navigate to the appropriate screen
            showLevel obj;
            obj.showDifficultyLevels();
        } 
        else if (gameSelected) {
            const int screenWidth = 1600;
            const int screenHeight = 1000;
            InitWindow(screenWidth, screenHeight, "Typing Speed Test Game");
            SetTargetFPS(60);
            
            Sound gameOverSound = LoadSound("gameover.wav");
            Sound startGameSound = LoadSound("game2.wav");

            TypingGame* game = new TypingGame(screenWidth, screenHeight, gameOverSound, startGameSound);
            game->showGameScreen();
            
        }

    }
}

int main() {
    
    InitWindow(1600, 900, "Typing Speed Test");
    InitAudioDevice();
    SetTargetFPS(60);

    // Load background image
    background = LoadTexture("bgres.png");       // Replace with your background image file

    // Load sound effects
    buttonClickSound = LoadSound("button.wav");
    
    showMainMenu();                              // Start with the main menu
    
    UnloadSound(buttonClickSound);               // Unload sound effects
    UnloadTexture(background);

    CloseAudioDevice();
    CloseWindow();

    return 0;
}




