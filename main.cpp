#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#define K 30
#include "mirrors.h"
#include <cmath>
#define PI 3.14159265
#include <time.h>
#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds

sf::Vector2f V2iToV2f(sf::Vector2i a) { //Exchanger between Vector2i to Vector2f
    sf::Vector2f b;
    b.x = a.x; b.y = a.y;
    return b;
}

void rotate1(sf::Vector2f& x, int direction) { // Function that changes laser direction
    double sin1 = 0.0174524064372835128194189785163161924722527203071396426836124276;  // sin of 1 degree
    double cos1 = 0.9998476951563912391570115588139148516927403105831859396583207145;  // cos of 1 degree
    double helpx = x.x;
    double helpy = x.y;
    x.x = helpx * cos1 + helpy * (-(sin1 * direction));
    x.y = helpx * (sin1 * direction) + helpy * cos1;
}
// (180 - )
void rotate(sf::Vector2f& x, int direction, int degree) { // Something like the function above, but with one difference(degrees)
    double xa = degree * PI / 180.0;
    double sinn = sin(xa);
    double coss = cos(xa);
    double helpx = x.x;
    double helpy = x.y;
    x.x = helpx * coss + helpy * (-(sinn * direction));
    x.y = helpx * (sinn * direction) + helpy * coss;
}

void setMirror(const Mirror& mirrorParameters, sf::RectangleShape& mirror) { // mirrors[i], mirror
    mirror.setPosition(mirrorParameters.x, mirrorParameters.y);
    mirror.setRotation(mirrorParameters.angle);
    double a = mirrorParameters.length;
    double b = mirror.getSize().y;
    mirror.setSize(sf::Vector2f{static_cast<float>(a), static_cast<float>(b)});
    mirror.setOrigin(mirrorParameters.length / 2, mirror.getSize().y / 2);
}

/*bool mirrorIntersect(const Mirror &mirror, const sf::Sprite &rayRender, double &deltaX, double &deltaY){ // useless
    double rad = mirror.angle * PI / 180.0;
    deltaY  = sin(rad) * (mirror.length / 2);
    deltaX  = cos(rad) * (mirror.length / 2);
    return ((rayRender.getPosition().x > (mirror.x - deltaX))
            && (rayRender.getPosition().x < (mirror.x + deltaX))
            && (rayRender.getPosition().y > (mirror.y - deltaY))
            && (rayRender.getPosition().y < (mirror.y + deltaY)));
}*/

bool geometry(const Mirror& mirror, const sf::Sprite& rayRender, double eps) {
    double rad = mirror.angle * PI / 180.0; // 1.57
    double deltaY = fabs(sin(rad) * (mirror.length / 2));
    double deltaX = fabs(cos(rad) * (mirror.length / 2));
    double delta = 0.01;
    double posX = rayRender.getPosition().x;
    double posY = rayRender.getPosition().y;
    //std::cout << "abs = " << fabs(
    //		   (rayRender.getPosition().x - mirror.x) / deltaX
    //          -(rayRender.getPosition().y - mirror.y) / deltaY
     //  ) << '\n';
    if (deltaX < delta || deltaY < delta) {
        //std::cout << "deltaX " << deltaX << '\n';
        //std::cout << "deltaY " << deltaY << '\n';
        return ((posX > (mirror.x - deltaX)) && (posX < (mirror.x + deltaX)) && (posY > (mirror.y - deltaY)) && (posY < (mirror.y + deltaY)));
    }
    bool checkFabs = (fabs((posX - mirror.x) / deltaX - (posY - mirror.y) / deltaY) <= eps);
    return ((checkFabs && ((posX > (mirror.x - deltaX)) && (posX < (mirror.x + deltaX))))
        || (checkFabs && (posY > (mirror.y - deltaY)) && (posY < (mirror.y + deltaY)))
        );
}


// adding mirror by user
// 1. left click, saving mouse position, taking second position (*2)
// 2. finding rotation of mirror
// 3. finding length
// 4. adding to the mirrors vector
// ctrl + z - to delete last added mirror
// vector.pop_back();

int main() {
    sf::RenderWindow window(sf::VideoMode(1000, 1000), "LightRayLASER");

    sf::Texture laser;
    laser.loadFromFile("flashlight.png");
    sf::Texture ray;
    ray.loadFromFile("ray.png");
    sf::RectangleShape mirror(sf::Vector2f(100, 10));
    mirror.setFillColor(sf::Color::Blue);
    // sf::CircleShape test;
     //test.setFillColor(sf::Color::Black);
   //  test.setRadius(10);


    std::vector<Mirror> mirrors = { Mirror(600, 800, 10, 200), Mirror(800, 600, 45, 300) , Mirror(450, 200, 60, 500), Mirror(400, 400, 80, 200) }; // x, y, angle, length, Mirror(450, 200, 45, 500) , Mirror(800, 600, 90, 100)


    sf::Sprite laserRender(laser);
    sf::Sprite rayRender(ray);


    laserRender.setOrigin(laser.getSize().x / 2, laser.getSize().y / 2); // Making render's center picture's center
    laserRender.setPosition(200, 200);
    rayRender.setOrigin(0, ray.getSize().y / 2);
    rayRender.setPosition(200, 200);
    mirror.setPosition(500, 500);
    window.setKeyRepeatEnabled(false);

    sf::Vector2f laserDirection = { 1, 0 }; // Original laser direction
    sf::Vector2i mousePosFirst, mousePosSecond;
    int difference; // difference between first mouse position and position where mouseButton released
    double deltaX, deltaY; // difference in coordinates of the mirror when it's rotated
    double epsilon = 0.1; // mirror interaction precision

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            else if (event.type == sf::Event::KeyPressed && sf::Keyboard::Up == event.key.code) {
                laserRender.rotate(-1);
                rotate(laserDirection, -1, 1);
                rayRender.rotate(-1);
            }
            else if (event.type == sf::Event::KeyPressed && sf::Keyboard::Down == event.key.code) {
                laserRender.rotate(1);
                rotate(laserDirection, 1, 1);
                rayRender.rotate(1);
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z) && sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) {
                if (mirrors.size() > 0) { // how to set timer
                    mirrors.pop_back();
                    std::this_thread::sleep_for(std::chrono::milliseconds(500));
                }

            }
            else if (event.type == sf::Event::MouseButtonPressed) {
                if (sf::Mouse::Right == event.key.code) {
                    laserRender.setPosition(V2iToV2f(sf::Mouse::getPosition(window)));
                    rayRender.setPosition(laserRender.getPosition().x, laserRender.getPosition().y);
                }
                else if (sf::Mouse::Left == event.key.code) {
                    mousePosFirst = sf::Mouse::getPosition(window);
                }
            }
            else if (event.type = sf::Event::MouseButtonReleased) {
                if (sf::Mouse::Left == event.key.code) {
                    mousePosSecond = sf::Mouse::getPosition(window);
                    int newLength = sqrt(pow((mousePosSecond.x - mousePosFirst.x), 2) + pow((mousePosSecond.y - mousePosFirst.y), 2));
                    double newRotation = atan(double(mousePosSecond.y - mousePosFirst.y) / double(mousePosSecond.x - mousePosFirst.x));
                    std::cout << mousePosSecond.x - mousePosFirst.x << ' ' << mousePosSecond.y - mousePosFirst.y << ' ' << (double(mousePosSecond.x - mousePosFirst.x) / double(mousePosSecond.y - mousePosFirst.y)) << '\n';
                    mirrors.push_back(Mirror(mousePosFirst.x, mousePosFirst.y, newRotation * (180 / PI), newLength));

                    /*	difference = mousePos.y - sf::Mouse::getPosition(window).y;
                    int degrees = difference / 90;
                    if (degrees < 0){
                        laserRender.rotate(degrees);
                        rayRender.rotate(degrees);
                        rotate(laserDirection, 1, degrees);
                    }else if(degrees > 0){
                        laserRender.rotate(degrees);
                        rayRender.rotate(-degrees);
                        rotate(laserDirection, 1, degrees);
                    } */
                }
            }
        }


        window.clear(sf::Color::White);
        sf::Vector2f rayDirection = laserDirection;
        rayRender.setRotation(laserRender.getRotation());
        rayRender.setPosition(laserRender.getPosition().x, laserRender.getPosition().y);
        while (rayRender.getPosition().x <= window.getSize().x && rayRender.getPosition().x >= 0 && rayRender.getPosition().y <= window.getSize().y && rayRender.getPosition().y >= 0) {
            rayRender.move(rayDirection);
            window.draw(rayRender);
            for (size_t i = 0; i < mirrors.size(); i++) {
                //std:: cout << "current mirror's index is " << i << '\n';
                setMirror(mirrors[i], mirror);
                if (geometry(mirrors[i], rayRender, epsilon)) { //(geometry(mirrors[i], rayRender, deltaX, deltaY, epsilon) mirrorIntersect(mirrors[i], rayRender, deltaX, deltaY)
                    //double deltaRotation = 180 - (2*(mirror.getRotation() - rayRender.getRotation())); // 45 - 199 = 154 * 2 => 180 - - 308 = 488
                    int trueAngle = rayRender.getRotation();
                    while (trueAngle >= 180) {
                        trueAngle -= 180;
                    }
                    int deltaRotation = 2 * (mirrors[i].angle - trueAngle);
                    //std::cout << "rayRender rotation is " << rayRender.getRotation() << '\n';
                    //std::cout << "Delta rotation is " << deltaRotation << '\n';
                    rayRender.rotate(deltaRotation);
                    rotate(rayDirection, -1, -deltaRotation);

                }
            }

        }
        /*double rad = mirrors[1].angle * PI / 180.0; // 1.57 debug starts
        std::cout << rad << '\n';
        deltaY  = fabs(sin(rad) * (mirrors[1].length / 2));
        deltaX  = fabs(cos(rad) * (mirrors[1].length / 2));
        std::cout << deltaX << '\n';
        std::cout << deltaX << '\n'; // debug ends */
        //test.setPosition(mirrors[1].x, mirrors[1].y); // wrong Origin maybe?
      //  window.draw(test);
        window.draw(laserRender);
        for (size_t i = 0; i < mirrors.size(); i++) {
            setMirror(mirrors[i], mirror);
            window.draw(mirror);
        }


        window.display();
    }
}
