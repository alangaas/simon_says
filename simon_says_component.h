#include "esphome.h"
#include <vector>

class SimonSays : public esphome::Component {
 public:
  void setup() override {
    sequence_.resize(20);
    for (int i = 0; i < 20; i++) {
      sequence_[i] = esphome::random(0, 5);  // Use ESPHome's random function
    }
    level_ = 1;
    input_index_ = 0;
    play_sequence_ = true;
  }

  void loop() override {
    if (play_sequence_) {
      this->play_sequence();  // Non-blocking play_sequence method
    }
  }

  void set_leds(const std::vector<esphome::output::BinaryOutput *> &leds) {
    this->leds_ = leds;
  }

  void button_pressed(int index) {
    if (index != sequence_[input_index_]) {
      game_over();
      return;
    }

    leds_[index]->turn_on();
    esphome::delay(200);  // Non-blocking delay
    leds_[index]->turn_off();

    input_index_++;
    if (input_index_ == level_) {
      level_++;
      if (level_ > 20) {
        win_game();
      } else {
        play_sequence_ = true;
      }
    }
  }

 protected:
  std::vector<esphome::output::BinaryOutput *> leds_;
  std::vector<int> sequence_;
  int level_{1};
  int input_index_{0};
  bool play_sequence_{true};

  void play_sequence() {
    static int last_play_time = 0;
    int current_time = millis();
    if (current_time - last_play_time >= 500) {
      last_play_time = current_time;

      for (int i = 0; i < level_; i++) {
        leds_[sequence_[i]]->turn_on();
        esphome::delay(300);  // Non-blocking delay
        leds_[sequence_[i]]->turn_off();
        esphome::delay(200);  // Non-blocking delay
      }

      input_index_ = 0;
      play_sequence_ = false;
    }
  }

  void game_over() {
    ESP_LOGI("simon", "Game over");
    for (int i = 0; i < 3; i++) {
      for (auto *led : leds_)
        led->turn_on();
      esphome::delay(300);  // Non-blocking delay
      for (auto *led : leds_)
        led->turn_off();
      esphome::delay(300);  // Non-blocking delay
    }
    setup();  // Restart game
  }

  void win_game() {
    ESP_LOGI("simon", "You win!");
    for (int i = 0; i < 5; i++) {
      for (auto *led : leds_)
        led->turn_on();
      esphome::delay(200);  // Non-blocking delay
      for (auto *led : leds_)
        led->turn_off();
      esphome::delay(200);  // Non-blocking delay
    }
    setup();
  }
};
