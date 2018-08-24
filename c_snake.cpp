#include <ge211.h>

using namespace ge211;

struct snake_game : Abstract_game {
    Rectangle_sprite rect{Dimensions{300, 200}, Color::medium_red()};

    void draw(Sprite_set &sprites) override {
        sprites.add_sprite(rect, Position{100, 100});
    }
};

int main() {
    snake_game{}.run();
}