#include <SFML/Graphics.hpp>
int main(){
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "Hello, world! window");
    sf::Font proggy;
    proggy.loadFromFile("resources/ProggyTiny.ttf");
    sf::Text hello_text("Hello, world", proggy, 50);
    while(window.isOpen()){
        sf::Event event;
        while(window.pollEvent(event)){
            if(event.type==sf::Event::Closed){
                window.close();
            }
        }
        window.clear();
        window.draw(hello_text);
        window.display();
    }
    return 1;
}
