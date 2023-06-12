#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <sstream>

using namespace std;
using namespace sf;

#define Width 600
#define Height 300

struct Position
{
    int x;
    int y;
};

int main(void)
{
    Font arial;
    arial.loadFromFile("arial.ttf");
    RenderWindow window(VideoMode(Width, Height), "Project Game");
    window.setFramerateLimit(60);
                        
    
    //이미지 불러오기
    Texture t1;
    Texture t2;
    t1.loadFromFile("");
    t2.loadFromFile("");
}

