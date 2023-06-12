#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <sstream>

using namespace std;
using namespace sf;

#define Width 600
#define Height 300

struct 
{
    int x;
    int y;
};
bool isAttacking = false; // 플레이어의 공격 여부를 나타내는 변수
    bool isCooling = false; // 플레이어의 쿨타임 여부를 나타내는 변수
    sf::RectangleShape attackRectangle; // 공격에 사용할 직사각형
    sf::Clock attackClock; // 공격 직사각형의 지속 시간을 측정하기 위한 시계
    sf::Clock cooldownClock; // 쿨타임을 측정하기 위한 시계
    const float cooldownDuration = 3.0f; // 쿨타임의 지속 시간


vector<Texture> textures;
for(int=1;i<=8;i++) {
    Texture texture;
    stringstream ss;
    ss << "images/r" << i <<".png";
    texture.loadFromFile(ss.str());
    textures.push_back(texture);
}

int main(void)
{
    Texture t1;
    Texture t2;
}

//이 커밋은 12:56에 만들어짐
