#include <ge211.h>

#include <vector>

using namespace ge211;
using namespace std;

// MODEL CONSTANTS

Dimensions const scene_dimensions{1024, 768};
int const min_x_coord{0};
int const max_x_coord{1024};
int const min_y_coord{0};
int const max_y_coord{768};
int const sprite_size{5};
int const collision_size{3};
int const pixel_update{10};
double last_update{0};
double const reset_update{0.05};
bool add_food{false};
bool game_over{false};

//maximum amount of food at once on screen
int const max_food{3};


// MODEL DATA DEFINITIONS

struct Food {
    vector<Position> locs;

    static Position random(
            Random&,
            int min_x, int max_x,
            int min_y, int max_y);
};

struct Snake {
    enum class Direction { up, left, down, right };

    Direction dir = Direction::left;
    vector<Position> segments;

    static Position random(
            Random&,
            int min_x, int max_x,
            int min_y, int max_y);
    void grow();
    void update();
};

struct Model {
    Food food;
    Snake snake;

    void add_random_food(Random&);
    void add_snake_start(Random&);
    bool food_collision();
    bool self_collision();
    bool out_of_bounds();
    void update();
};

// VIEW DATA DEFINITIONS

struct View {
    Circle_sprite food_sprite{sprite_size, Color::medium_magenta()};
    Circle_sprite snake_sprite{sprite_size, Color::medium_green()};
    Circle_sprite lose_sprite{sprite_size, Color::medium_red()};
};

struct Simple_snake : Abstract_game {

    // Model
    Model model;

    // View
    View view;
    Dimensions initial_window_dimensions() const override;
    void draw(Sprite_set& sprites) override;

    // Controller
    bool game_start{true};
    bool is_paused{false};
    void on_key(Key key) override;
    void on_frame(double dt) override;
};

int main() {
    Simple_snake{}.run();
}

// FUNCTION DEFINITIONS FOR MODEL

// ~~~MODEL~~~
void Model::add_random_food(Random& rng) {
    while (food.locs.size() < max_food) {
        food.locs.push_back(Food::random(rng, min_x_coord, max_x_coord,
                                         min_y_coord, max_y_coord));
    }
}

void Model::add_snake_start(Random& rng) {
    snake.segments.push_back(Snake::random(rng, min_x_coord, max_x_coord,
                                           min_y_coord, max_y_coord));
}

bool Model::food_collision() {
    Position snake_head = snake.segments.front();
    for (int i = 0; i < food.locs.size(); ++i) {
        double x_dist = food.locs[i].x - snake_head.x;
        double y_dist = food.locs[i].y - snake_head.y;

        // circle intersection formula, based on `collision_size` radius
        if ( sqrt((x_dist * x_dist) + (y_dist * y_dist)) < (collision_size * collision_size) ) {
            food.locs.erase(food.locs.begin() + i);
            add_food = true;
            return true;
        }
    }
    return false;
}

bool Model::self_collision() {
    Position snake_head = snake.segments.front();
    for (int i = 1; i < snake.segments.size(); ++i) {
        if ((snake.segments[i].x == snake_head.x) && (snake.segments[i].y == snake_head.y)) {
            return true;
        }
    }
    return false;
}

bool Model::out_of_bounds() {
    Position snake_head = snake.segments.front();
    return (snake_head.x < min_x_coord) ||
           (snake_head.x > max_x_coord) ||
           (snake_head.y < min_y_coord) ||
           (snake_head.y > max_y_coord);
}

void Model::update() {
    snake.update();
}

// ~~~SNAKE~~~
void Snake::update() {

    Position new_head = segments.front();

    switch (dir) {
        case Direction::down:
            segments.pop_back();
            new_head.y += pixel_update;
            segments.insert(segments.begin(), new_head);
            break;
        case Direction::left:
            segments.pop_back();
            new_head.x -= pixel_update;
            segments.insert(segments.begin(), new_head);
            break;
        case Direction::up:
            segments.pop_back();
            new_head.y -= pixel_update;
            segments.insert(segments.begin(), new_head);
            break;
        case Direction::right:
            segments.pop_back();
            new_head.x += pixel_update;
            segments.insert(segments.begin(), new_head);
            break;
    }
}

void Snake::grow() {
    Position new_head = segments.back();
    segments.push_back(new_head);
}

Position Snake::random(
        Random& rng,
        int min_x, int max_x,
        int min_y, int max_y)
{
    int x_pos = rng.between(300, 600);
    int y_pos = rng.between(200, 400);
    Position pos{x_pos, y_pos};
    return pos;
}

// ~~~FOOD~~~
Position Food::random(
        Random& rng,
        int min_x, int max_x,
        int min_y, int max_y)
{
    int x_pos = rng.between(min_x, max_x);
    int y_pos = rng.between(min_y, max_y);
    Position pos{x_pos, y_pos};
    return pos;
}

// FUNCTION DEFINITIONS FOR VIEW

Dimensions Simple_snake::initial_window_dimensions() const {
    return scene_dimensions;
}

void Simple_snake::draw(Sprite_set &sprites) {
    for (Position const& loc : model.food.locs) {
        sprites.add_sprite(view.food_sprite, loc);
    }
    if (!game_over) {
        for (Position const &pos : model.snake.segments) {
            sprites.add_sprite(view.snake_sprite, pos);
        }
    }
    else {
        for (Position const &pos : model.snake.segments) {
            sprites.add_sprite(view.lose_sprite, pos);
        }
    }
}

// FUNCTION DEFINITIONS FOR CONTROLLER

void Simple_snake::on_key(Key key) {

    Snake::Direction cur_dir = model.snake.dir;

    if (key == Key::code('q')) {
        quit();
    } else if (key == Key::code('f')) {
        get_window().set_fullscreen(!get_window().get_fullscreen());
    } else if (key == Key::code('p')) {
        is_paused = !is_paused;
    } else if (key == Key::up() && cur_dir != Snake::Direction::down) {
        model.snake.dir = Snake::Direction::up;
    } else if (key == Key::down() && cur_dir != Snake::Direction::up) {
        model.snake.dir = Snake::Direction::down;
    } else if (key == Key::left() && cur_dir != Snake::Direction::right) {
        model.snake.dir = Snake::Direction::left;
    } else if (key == Key::right() && cur_dir != Snake::Direction::left) {
        model.snake.dir = Snake::Direction::right;
    }
}

void Simple_snake::on_frame(double dt)
{
    double time_remaining = last_update - dt;
    if (time_remaining > 0) {
        last_update = time_remaining;
    }
    else {
        last_update = reset_update + time_remaining;

        //initialize the game state
        if (game_start) {
            model.add_random_food(get_random());
            model.add_snake_start(get_random());
            game_start = !game_start;
        }

        if ((!is_paused) && (!game_over))
            model.update();

        if (model.out_of_bounds() || model.self_collision()) {
            game_over = true;
        }

        if (model.food_collision())
            model.snake.grow();

        if (add_food) {
            add_food = false;
            model.add_random_food(get_random());
        }
    }
}
