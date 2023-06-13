#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <sstream>

#include <random>

using namespace std;
using namespace sf;

#define Width 1200
#define Height 600


struct Position
{
    int x;
    int y;
};

int getRandomNumber(int min, int max) 
{
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> distrib(min, max);
    return distrib(gen);
}


bool checkCollisionCircle(const sf::CircleShape& player, const sf::CircleShape& obstacle) 
{
    sf::Vector2f playerPosition = player.getPosition();
    sf::Vector2f obstaclePosition = obstacle.getPosition();

    // 플레이어와 장애물의 중심 사이의 거리
    float distance = std::sqrt(std::pow(playerPosition.x - obstaclePosition.x, 2) + std::pow(playerPosition.y - obstaclePosition.y, 2));

    // 플레이어와 장애물의 반지름
    float playerRadius = player.getRadius()/2;
    float obstacleRadius = obstacle.getRadius()/2;

    return distance < playerRadius + (obstacleRadius/4);
}

// spriteToCircle 함수 정의
sf::CircleShape spriteToCircle(const sf::Sprite& sprite) 
{
    sf::FloatRect spriteBounds = sprite.getGlobalBounds();
    sf::Vector2f spritePosition = sprite.getPosition();
  
    float spriteRadius = std::min(spriteBounds.width, spriteBounds.height) / 8.351f;

    sf::CircleShape circle(spriteRadius);
    circle.setOrigin(spriteRadius, spriteRadius);
    circle.setPosition(spritePosition);

    return circle;
}

std::vector<sf::CircleShape> spritesToCircles(const std::vector<sf::Sprite>& sprites) 
{
    std::vector<sf::CircleShape> circles;
    circles.reserve(sprites.size());

    for (const auto& sprite : sprites) 
    {
        circles.push_back(spriteToCircle(sprite));
    }

    return circles;
}



int main(void)
{
    // SFML 윈도우
    RenderWindow window(VideoMode(Width, Height), "Open Run");
    window.setFramerateLimit(144); // 프레임
    // ---------------------------------------------------------------------------

    //폰트 설정
    Font arial;
    arial.loadFromFile("arial.ttf");

    // ---------------------------------------------------------------------------
    
     // 배경 이미지 로드
    const int numBackgrounds = 30; // 배경 이미지 개수
    std::vector<sf::Texture> b(numBackgrounds);
    for (int i = 0; i < numBackgrounds; ++i) 
    {
        std::stringstream ss;
        ss << "images/b" << (i + 1) << ".png";
        if (!b[i].loadFromFile(ss.str())) 
        {
            // 이미지 로드 실패
            std::cerr << "Failed to load background image: " << ss.str() << std::endl;
            return -1;
        }
    }

    sf::Sprite backgroundSprite(b[0]);

    // 배경 이미지 변경을 위한 변수들
    int currentBackgroundIndex = 0; // 현재 배경 이미지 인덱스
    int currentBackgroundScore = 2; // 현재 배경 이미지 점수


    // ---------------------------------------------------------------------------
    
    // Ground 이미지 로드
    sf::Texture groundTexture;
    if (!groundTexture.loadFromFile("images/G.png")) 
    {
        // 이미지 로드 실패
        std::cerr << "Failed to load Ground image!" << std::endl;
        return -1;
    }

    // Ground 스프라이트
    sf::Sprite groundSprite(groundTexture);

    // Ground 크기
    float scaleFactor = 2.5f;
    groundSprite.setScale(1.8*scaleFactor, scaleFactor);

    // Ground 위치
    int windowHeight = window.getSize().y - 17;
    groundSprite.setPosition(-15.f, windowHeight - groundTexture.getSize().y);

    // ---------------------------------------------------------------------------

    // 플레이어
    vector<Texture> textures; // 텍스처를 벡터로 저장
    for (int i = 1; i <= 8; i++) 
    {
        Texture texture;
        stringstream ss;
        ss << "images/r" << i << ".png";
        texture.loadFromFile(ss.str());
        textures.push_back(texture);
    }

    // 스프라이트
    vector<Sprite> playerFrames; 
    for (int i = 0; i < textures.size(); i++) 
    {
        Sprite sprite(textures[i]);
        //sprite.setScale(1.0f, 1.0f);
        playerFrames.push_back(sprite);
    }

    static const int PLAYER_Y_BOTTOM = Height - textures[0].getSize().y - 40; // 바닥 위치

    Position playerPos;
    playerPos.x = 100;
    playerPos.y = PLAYER_Y_BOTTOM;

    int index = 0;
    float frame = 0.f;
    float frameSpeed = 1.0f; // 이미지 변경 속도
    const int changeCount = 8; // 몇 프레임마다 변경

    // ---------------------------------------------------------------------------

    const int gravity = 5;     
    bool isJumping = false;     
    bool isBottom = true;    

    // ---------------------------------------------------------------------------

    vector<Texture> objTextures;
    vector<string> objImages = { "images/s1.png", "images/s2.png", "images/s3.png" };
    for (const auto& image : objImages) 
    {
        Texture texture;
        if (!texture.loadFromFile(image)) 
        {
            cerr << "Failed to load obj image: " << image << endl;
            return -1;
        }
        objTextures.push_back(texture);
    }

    vector<Sprite> objs;
    for (const auto& texture : objTextures) 
    {
        Sprite obj(texture);
        //obj.setScale(1.0f, 1.0f);
        objs.push_back(obj);
    }

    const int obstacleWidth = objTextures[0].getSize().x * 0.3f;
    const int obstacleHeight = objTextures[0].getSize().y;

    vector<Position> objPositions;
    for (int i = 0; i < 10; i++) 
    {
        Position position;
        position.x = Width + i * getRandomNumber(300, 800);
        position.y = PLAYER_Y_BOTTOM - obstacleHeight;
        objPositions.push_back(position);
    }

    Clock clock;

    // ---------------------------------------------------------------------------

    // 점수
    Font font;
    font.loadFromFile("arial.ttf");

    int score = 0; // 점수

    ostringstream sScore;
    sScore << score << " M";

    Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(30);
    scoreText.setFillColor(Color::Black);

    // ---------------------------------------------------------------------------

    //게임 시작

    Text GameStart;
    GameStart.setCharacterSize(50);
    GameStart.setFillColor(Color::Red);
    GameStart.setPosition(390, 220);
    GameStart.setFont(arial);
    GameStart.setString(" Start to press 'A' ");

    //게임 이름

    Text GameName;
    GameName.setCharacterSize(100);
    GameName.setFillColor(Color::White);
    GameName.setPosition(315, 100);
    GameName.setFont(arial);
    GameName.setString(" Open Run! ");

    //게임오버

    Text GameOver;
    GameOver.setCharacterSize(80);
    GameOver.setFillColor(Color::Red);
    GameOver.setPosition(390, 150);
    GameOver.setFont(arial);
    GameOver.setString("Game Over");

    Text GameOver2;
    GameOver2.setCharacterSize(30);
    GameOver2.setFillColor(Color::White);
    GameOver2.setPosition(460, 300);
    GameOver2.setFont(arial);
    GameOver2.setString(" Quit to press 'ESC' ");

    Text GameOver3;
    GameOver3.setCharacterSize(30);
    GameOver3.setFillColor(Color::White);
    GameOver3.setPosition(460, 350);
    GameOver3.setFont(arial);
    GameOver3.setString(" Restart to press 'S' ");

    //게임 인트로

    Text Intro1;
    Intro1.setCharacterSize(30);
    Intro1.setFillColor(Color::White);
    Intro1.setPosition(400, 100);
    Intro1.setFont(arial);
    Intro1.setString(" Input your Height / Weight ");

    int userW = 0;

    Text Intro2;
    Intro2.setCharacterSize(30);
    Intro2.setFillColor(Color::White);
    Intro2.setPosition(400, 150);
    Intro2.setFont(arial);
    Intro2.setString(" Weight : " + to_string(userW) + " KG");

    Text Intro3;
    Intro3.setCharacterSize(30);
    Intro3.setFillColor(Color::White);
    Intro3.setPosition(400, 200);
    Intro3.setFont(arial);
    Intro3.setString(" E : +10 / R : +1 / D : -10 / F : -1 ");

    Text Intro4;
    Intro4.setCharacterSize(30);
    Intro4.setFillColor(Color::White);
    Intro4.setPosition(400, 250);
    Intro4.setFont(arial);
    Intro4.setString(" Next To Press G ");

    int endK = 0;

    Text endS;
    endS.setCharacterSize(30);
    endS.setFillColor(Color::White);
    endS.setPosition(400, 450);
    endS.setFont(arial);
    endS.setString(" Result : " + to_string(endK) + " kcal");
    
    int state = 0;
    

    // ---------------------------------------------------------------------------

    while (window.isOpen()) 
    {
        window.clear();

        if (state == 0)
        {
            sf::Clock Iclock;
            bool keyReleasedE = true;
            bool keyReleasedR = true;
            bool keyReleasedD = true;
            bool keyReleasedF = true;
            bool keyReleasedG = true;
            while (window.isOpen())
            {
                sf::Event event;
                while (window.pollEvent(event))
                {
                    if (event.type == sf::Event::Closed)
                    {
                        window.close();
                    }
                }

                if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
                {
                    if (keyReleasedE)
                    {
                        userW += 10;
                        keyReleasedE = false;
                    }
                }
                else
                {
                    keyReleasedE = true;
                }

                if (sf::Keyboard::isKeyPressed(sf::Keyboard::R))
                {
                    if (keyReleasedR)
                    {
                        userW += 1;
                        keyReleasedR = false;
                    }
                }
                else
                {
                    keyReleasedR = true;
                }

                if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
                {
                    if (keyReleasedD)
                    {
                        userW -= 10;
                        keyReleasedD = false;
                    }
                }
                else
                {
                    keyReleasedD = true;
                }

                if (sf::Keyboard::isKeyPressed(sf::Keyboard::F))
                {
                    if (keyReleasedF)
                    {
                        userW -= 1;
                        keyReleasedF = false;
                    }
                }
                else
                {
                    keyReleasedF = true;
                }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::G))
                {
                    if (keyReleasedG)
                    {
                        state = 1; 
                        break;
                        keyReleasedG = false;
                    }
                }
                else
                {
                    keyReleasedG = true;
                }

                window.clear();
                window.draw(Intro1);
                Intro2.setString(" Weight : " + to_string(userW) + " KG");
                window.draw(Intro2);
                window.draw(Intro3);
                window.draw(Intro4);
                window.display();
            }

            
        }

        else if (state == 1)
        {
            window.draw(GameStart);
            window.draw(GameName);
        }
        else if (state == 3)
        {
            window.clear();
            window.draw(GameOver);

            sScore.str("");
            sScore << score << " M";
            scoreText.setString(sScore.str());
            scoreText.setPosition(570, 250);
            scoreText.setFillColor(Color::Red);

            if (score == 300)
            {
                if (40 <= userW && userW <= 49)
                {
                    endK = 55; 
                }
                else if (50 <= userW && userW <= 59)
                {
                    endK = 67;
                }
                else if (60 <= userW && userW <= 69)
                {
                    endK = 80;
                }
                else if (70 <= userW && userW <= 79)
                {
                    endK = 92;
                }
                else if (80 <= userW && userW <= 89)
                {
                    endK = 104;
                }
                else if (90 <= userW && userW <= 99)
                {
                    endK = 116;
                }
                else if (100 <= userW && userW <= 109)
                {
                    endK = 129;
                }

                endS.setCharacterSize(30);
                endS.setFillColor(Color::White);
                endS.setPosition(460, 400);
                endS.setString(" Result : " + to_string(endK) + " kcal");

                window.draw(endS);
            }

            window.draw(scoreText);
            window.draw(GameOver2);
            window.draw(GameOver3);

            if (Keyboard::isKeyPressed(Keyboard::Escape))
            {
                window.close();
            }

            if (Keyboard::isKeyPressed(Keyboard::S))
            {
                
                state = 1;
                score = 0;
                clock.restart();
                playerPos.y = PLAYER_Y_BOTTOM;
                isBottom = true;

                scoreText.setString(to_string(score) + " M");
            }

            // ---------------------------------------------------------------------------
        }
        else
        {
            // 점수 위치 설정
            scoreText.setPosition(10, 10);
            window.draw(scoreText);
            scoreText.setCharacterSize(30);
            scoreText.setFillColor(Color::Black);

            //점프 입력
            if (Keyboard::isKeyPressed(Keyboard::Space)) 
            {
                if (isBottom && !isJumping)    
                {
                    //make jumping stage;
                    isJumping = true;
                    isBottom = false;
                }
            }

            // 점프
            if (isJumping)
            {
                playerPos.y -= gravity; //점프 (중력을 -)
            }
            else
            {
                playerPos.y += gravity;
            }
            
            if (playerPos.y >= PLAYER_Y_BOTTOM)    //점프 최소점
            {
                playerPos.y = PLAYER_Y_BOTTOM;
                isBottom = true;
            }
            if (playerPos.y <= PLAYER_Y_BOTTOM - 100)    //점프 최대 높이
            {
                isJumping = false;
            }

            // ---------------------------------------------------------------------------
            
            //플레이어 프레임 변경
            frame += frameSpeed;
            if (frame > changeCount)
            {
                frame -= changeCount;
                ++index;
                if (index >= playerFrames.size()) 
                {
                    index = 0;
                }
            }
            // ---------------------------------------------------------------------------
            
            // 함수 선언
            sf::CircleShape spriteToCircle(const sf::Sprite & sprite);
            std::vector<sf::CircleShape> spritesToCircles(const std::vector<sf::Sprite>&sprites);
            bool checkCollisionCircle(const sf::CircleShape & player, const sf::CircleShape & obstacle);

            // ---------------------------------------------------------------------------

            //장애물
            for (int i = 0; i < objs.size(); i++) 
            {
                objs[i].setPosition(objPositions[i].x, objPositions[i].y + 52);
                objPositions[i].x -= 5;
                if (score >= 200)
                {
                    objPositions[i].x -= 6;
                }
                

                if (objPositions[i].x < -obstacleWidth) 
                {
                    objPositions[i].x = Width + getRandomNumber(300, 800);
                }

                if (checkCollisionCircle(spriteToCircle(playerFrames[index]), spriteToCircle(objs[i]))) 
                {
                    state++;
                    cout << "Game over!" << endl;
                    
                    objs.erase(objs.begin() + i);
                    objPositions.erase(objPositions.begin() + i);


                }
            }

            // ---------------------------------------------------------------------------
            
            // 시간 기반으로 점수 증가
            Time elapsedTime = clock.getElapsedTime();
            if (elapsedTime.asSeconds() >= 1.0f)
            {
                score += 5; // 점수 증가량 설정
                clock.restart();
            }

            // ---------------------------------------------------------------------------

            if (score == 300)
            {
                state = 3;
            }
            
            // ---------------------------------------------------------------------------
            
            // 배경 이미지 변경
            if (score >= currentBackgroundScore) 
            {
                currentBackgroundIndex = (currentBackgroundIndex + 1) % numBackgrounds;
                currentBackgroundScore += 10;
                backgroundSprite.setTexture(b[currentBackgroundIndex]);
            }

            // ---------------------------------------------------------------------------

            playerFrames[index].setPosition(playerPos.x, playerPos.y);

            scoreText.setString( to_string(score) + " M" );            

            // ---------------------------------------------------------------------------
            window.clear(Color::White);
            window.draw(backgroundSprite);
            window.draw(groundSprite);
            window.draw(playerFrames[index]);
            for (const auto& obj : objs) 
            {
                window.draw(obj);
            }
            window.draw(scoreText); 

            window.display();
        }

        window.display();

        Event e;
        while (window.pollEvent(e))
        {
            if (e.type == Event::Closed) 
            {
                window.close();
            }
            else if (e.type == Event::KeyPressed)
            {
                if (state == 1 && e.key.code == Keyboard::A)
                {
                    state = 2;
                }
            }
        }
    }

    return 0;
}
