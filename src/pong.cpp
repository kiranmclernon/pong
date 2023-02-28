#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <concepts>
#include <iostream>
#include <cmath>
#include <numbers>
#include <utility>

// If you reading this only runs on debug build not release build
// To run create a build directory and run cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
// Then go to build directory and run ./personal_learning_pong_bin
// As long as have an internet question this should run fine probably????
// Only tested on mac so hope for the best if your on windows or linux might have to 
// fiddle with cmakelists a bit
// No dependencies required uses fetch content to grab sfml

static constexpr float DEG_TO_RAD = std::numbers::pi / 180.0;
static constexpr float RAD_TO_DEG = 180.0 / std::numbers::pi; 

#define PADDLE_VELOCITY 0.5

enum PADDLE_POSITION{
    LEFT,
    RIGHT
};


std::string find_file_directory(const std::string& file){
    std::string current_file = __FILE__;
    std::size_t source_index = current_file.find("src");
    std::string dir_path = current_file.substr(0, source_index);
    std::cout << dir_path << std::endl;
    return dir_path + file;
}

class Paddle : public sf::Drawable {
public:
    Paddle(const float& height, const float& width, PADDLE_POSITION pos,
           const sf::RenderTarget& targ);
    sf::RectangleShape rect;
    const static sf::Color paddle_color;
    float x;
    float y;
    float width;
    float height;
    float max_y;
    float min_y;
    sf::Keyboard::Key up_key;
    sf::Keyboard::Key down_key;
    PADDLE_POSITION side;
    void keyHandler();
    void draw(sf::RenderTarget& target, sf::RenderStates) const;
    sf::Vector2f getPosition() const;
};

class LineDivider : public sf::Drawable {
public:
    LineDivider(const sf::RenderWindow& window_, const size_t& width_);
    const sf::RenderWindow& window;
    std::vector<sf::RectangleShape> blockouts;
    size_t width; 
    void draw(sf::RenderTarget& target, sf::RenderStates) const;
};

class ScoreBoard : public sf::Drawable {
public:
    ScoreBoard(const sf::RenderTarget& target_, const size_t& character_size);
    const sf::RenderTarget& target;
    sf::Font score_board_font;
    sf::Text player_1_text;
    sf::Text player_2_text;
    void draw(sf::RenderTarget& target, sf::RenderStates) const;
};

class Ball : public sf::Drawable{
public:
    Ball(const float& radius, const Paddle& left_paddle_, const Paddle& right_paddle_, const float& vel_mag_, const float& max_deflection_angle_, 
         const sf::RenderWindow& target_, ScoreBoard& score_board_);
    void ballEventHandle();
// private:
    sf::CircleShape circ;
    const Paddle& left_paddle;
    const Paddle& right_paddle;
    sf::Vector2f velocity;
    const float vel_mag;
    float max_deflection_angle;
    ScoreBoard score_board;
    const sf::RenderWindow& target;
    bool intersection_checker(const Paddle& paddle);
    float normalized_relative_intersection_y(const Paddle& paddle);
    void top_bottom_intersecion_handler();
    void draw(sf::RenderTarget& target, sf::RenderStates) const;
    void ball_exit_handler();
    void spawn_ball();
    std::pair<float, float> get_intersection_y(const Paddle& paddle);
    int player_1_score = 0;
    int player_2_score = 0;
};


int main(){
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "PONG IN UR FACD>>??");
    LineDivider div(window, 30);
    ScoreBoard sb(window, 40);
    Paddle left(100, 10, LEFT, window);
    Paddle right(100, 10, RIGHT, window);
    Ball ball(10, left, right, 0.5, 70, window, sb);
    while(window.isOpen()){
        sf::Event event;
        while(window.pollEvent(event)){
            if(event.type==sf::Event::Closed){
                window.close();
            }
        }
        left.keyHandler();
        right.keyHandler();
        ball.ballEventHandle();
        window.clear();
        sb.player_1_text.setString(std::to_string(ball.player_1_score));
        sb.player_2_text.setString(std::to_string(ball.player_2_score));
        window.draw(sb.player_1_text);
        window.draw(sb.player_2_text);
        window.draw(div);
        window.draw(left);
        window.draw(right);
        window.draw(ball);
        window.display();
    }
    return 0;
}

template<typename T>
concept GivesSize = requires(T a){
    {a.getSize()} ->  std::convertible_to<sf::Vector2f>;
};

template<GivesSize T>
void centre(T& shape){
    sf::Vector2f size = shape.getSize();
    shape.setOrigin(size.x / 2.0, size.y / 2.0);
}

void centreText(sf::Text& text){
    auto rect = text.getGlobalBounds();
    auto origin = text.getOrigin();
    text.setOrigin(origin.x + rect.width / 2, origin.y + rect.height / 2);
}

const sf::Color Paddle::paddle_color = sf::Color();

LineDivider::LineDivider(const sf::RenderWindow& window_, const size_t& width_) 
    : window(window_), width(width_){
    uint height = window_.getSize().y;
    float middle = window_.getSize().x / 2.0;
    size_t n = height / width;
    blockouts.reserve(n);
    for(size_t i = 0; i < n; i++){
        blockouts.push_back(sf::RectangleShape(sf::Vector2f(width, width)));
        centre(blockouts[i]);
        blockouts[i].setPosition(middle, 2 * width_ * i);
        blockouts[i].setFillColor(sf::Color::White);
    }
}

ScoreBoard::ScoreBoard(const sf::RenderTarget& target_, const size_t& character_size) 
    : target(target_), score_board_font(){
    float middle = target.getSize().x / 2.0;
    std::string proggy_path = find_file_directory("resources/ProggyTiny.ttf");
    score_board_font.loadFromFile(proggy_path);
    player_1_text.setFont(score_board_font);
    player_2_text.setFont(score_board_font);
    player_1_text.setString("0");
    player_2_text.setString("0");
    centreText(player_1_text);
    centreText(player_2_text);
    player_2_text.setCharacterSize(character_size);
    player_1_text.setCharacterSize(character_size);
    float size = player_1_text.getGlobalBounds().width;
    player_1_text.setPosition(middle + size * 2, character_size);
    player_2_text.setPosition(middle - size * 2, character_size);

}


void LineDivider::draw(sf::RenderTarget& target, sf::RenderStates) const{
    for(const auto& a : blockouts){
        target.draw(a);
    }

}

void ScoreBoard::draw(sf::RenderTarget& target, sf::RenderStates) const{
    target.draw(player_1_text);
    target.draw(player_2_text);
}


Paddle::Paddle(const float& height_, const float& width_, PADDLE_POSITION pos,
           const sf::RenderTarget& targ)
    : height(height_), width(width_), rect(sf::Vector2f(width_, height_)), side(pos){
    sf::Vector2f size = rect.getSize();
    sf::Vector2u window_size = targ.getSize();
    switch(pos){
        case LEFT :
            rect.setOrigin(0, size.y /  2.0);
            rect.setPosition(0, window_size.y / 2.0);
            x = 0.0;
            down_key = sf::Keyboard::X;
            up_key = sf::Keyboard::S;
            break;
        case RIGHT:
            rect.setOrigin(size.x, size.y / 2.0);
            rect.setPosition(window_size.x, window_size.y / 2.0);
            x = window_size.x;
            down_key = sf::Keyboard::Down;
            up_key = sf::Keyboard::Up;
    }
    y = window_size.y / 2.0;
    rect.setFillColor(sf::Color::White);
    min_y = height / 2.0;
    max_y = window_size.y - (height / 2.0);
}

void Paddle::draw(sf::RenderTarget& target, sf::RenderStates) const{
    target.draw(rect);
}

void Paddle::keyHandler(){
    if(sf::Keyboard::isKeyPressed(up_key)){
       if(y > min_y){
            y -= PADDLE_VELOCITY;
        }
    }
    if(sf::Keyboard::isKeyPressed(down_key)){
        if(y < max_y){
            y += PADDLE_VELOCITY;
        }
    }
    rect.setPosition(x, y);
}

sf::Vector2f Paddle::getPosition() const{
    return sf::Vector2f(x, y);
}

Ball::Ball(const float& radius, const Paddle& left_paddle_, const Paddle& right_paddle_, 
           const float& vel_mag_, const float& max_deflection_angle_, 
           const sf::RenderWindow& target_, ScoreBoard& score_board_) 
    : circ(radius), left_paddle(left_paddle_), right_paddle(right_paddle_),
    max_deflection_angle(max_deflection_angle_), target(target_), vel_mag(vel_mag_),
    score_board(score_board_){
    circ.setFillColor(sf::Color::White);
    circ.setOrigin(radius, radius);
    sf::Vector2f left_paddle_pos = left_paddle.getPosition();
    circ.setPosition(left_paddle_pos.x + left_paddle.width + radius, left_paddle_pos.y);
    velocity = sf::Vector2f(vel_mag, 0);
}

std::pair<float, float> Ball::get_intersection_y(const Paddle& paddle){
    float x;
    sf::Vector2f pos = circ.getPosition();
    switch(paddle.side){
        case LEFT:
            x = paddle.x + paddle.width;
            break;
        case RIGHT:
            x = paddle.x - paddle.width;
            break;
    }
    float a = std::pow(circ.getRadius(), 2) - (x * x) + (2 * pos.x * x) - (pos.x * pos.x) - (pos.y * pos.y);
    float k = std::sqrt(std::pow(circ.getPosition().y, 2) + a); // Quadratic formula and that
    return std::make_pair(pos.y + k, pos.y - k);
}

bool on_line(float max, float min, float point){
    return((point < max) && (point > min));
}

bool Ball::intersection_checker(const Paddle& paddle){
    sf::Vector2f pos = circ.getPosition();
    float rad = circ.getRadius();
    float half_paddle_height = left_paddle.height / 2.0;
    switch(paddle.side){
        case LEFT: 
            if(pos.x <= (left_paddle.x + left_paddle.width)){
                std::pair<float, float> intersecions = get_intersection_y(left_paddle);
                auto on_line_l = [&](const float& point)
                    {return on_line(left_paddle.y+(half_paddle_height), left_paddle.y - (half_paddle_height)  , point);};
                if(on_line_l(intersecions.first) or on_line_l(intersecions.second)){
                    return true;
                }
            }
            return false;
        case RIGHT:
            if(pos.x >= (right_paddle.x - right_paddle.width)){
                std::pair<float, float> intersecions = get_intersection_y(right_paddle);

                auto on_line_l = [&](const float& point)
                    {return on_line(right_paddle.y+(half_paddle_height), right_paddle.y - (half_paddle_height), point);};
                if(on_line_l(intersecions.first) or on_line_l(intersecions.second)){
                    return true;
                }
            }
            return false;
    }
}

float Ball::normalized_relative_intersection_y(const Paddle& paddle){
    float y_diff = circ.getPosition().y - paddle.y;
    return y_diff / paddle.height;
}

void Ball::top_bottom_intersecion_handler(){
    float bottom = target.getSize().y;
    if((circ.getPosition().y - circ.getRadius()) < 0){
        float heading = abs(std::atan(velocity.y / velocity.x)) * RAD_TO_DEG;
        sf::Transform transform;
        if(velocity.x > 0){
            transform.rotate(heading * 2);
            velocity = transform.transformPoint(velocity);
        } else {
            transform.rotate(-heading * 2);
            velocity = transform.transformPoint(velocity);
        }
    } else if((circ.getPosition().y + circ.getRadius()) > bottom){ // Case at the bottom
        float heading = abs(std::atan(velocity.y / velocity.x)) * RAD_TO_DEG;
        sf::Transform transform;
        if(velocity.x > 0){
            transform.rotate(-heading * 2);
            velocity = transform.transformPoint(velocity);
        } else {
            transform.rotate(heading * 2);
            velocity = transform.transformPoint(velocity);
        }
    }
}

void Ball::spawn_ball(){
    //https://stackoverflow.com/questions/686353/random-float-number-generation
    // float angle = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * std::numbers::pi; // this straight from stackoverflow
    float angle = 0;
    sf::Vector2u size = target.getSize();
    circ.setPosition(size.x / 2.0, size.y / 2.0);
    velocity = sf::Vector2f(vel_mag * std::cos(angle), vel_mag * std::sin(angle));
}

void Ball::ball_exit_handler(){
    if(circ.getPosition().x > right_paddle.x){ // Case it goes out the right hand side
        player_1_score += 1;
        score_board.player_1_text.setString(std::to_string(player_1_score));
        spawn_ball();
    }
    if(circ.getPosition().x < 0){ //In the case it goes out the left hand side
        player_2_score += 1;
        score_board.player_2_text.setString(std::to_string(player_2_score));
        spawn_ball();
    }
}

// Cheers to here https://gamedev.stackexchange.com/questions/4253/in-pong-how-do-you-calculate-the-balls-direction-when-it-bounces-off-the-paddl top answer
void Ball::ballEventHandle(){
    sf::Vector2f ballPos = circ.getPosition();
    top_bottom_intersecion_handler();
    if(intersection_checker(left_paddle)){
        float angle =  (normalized_relative_intersection_y(left_paddle) * max_deflection_angle) * DEG_TO_RAD; 
        velocity = sf::Vector2f(vel_mag * std::cos(angle), vel_mag * std::sin(angle));
    }
    if(intersection_checker(right_paddle)){
        float angle =  (normalized_relative_intersection_y(right_paddle) * max_deflection_angle) * DEG_TO_RAD;
        velocity = sf::Vector2f(-vel_mag * std::cos(angle),  vel_mag * std::sin(angle));
    }
    ball_exit_handler();
    float x = circ.getPosition().x + velocity.x;
    float y = circ.getPosition().y + velocity.y;
    circ.setPosition(x, y);
}

void Ball::draw(sf::RenderTarget& target, sf::RenderStates) const{
    target.draw(circ);
}

