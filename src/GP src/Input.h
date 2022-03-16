#include "AppCore.h"
#include <vector>

enum class InputState: short {Pressed=1,Released=0,Hold=2,NONE};
struct InputData{
    int key; InputState action; 
    bool operator == (InputData other);
    void print();
    void Swap(InputData& other);
};
struct Input{
    static std::vector<InputData> input_pool; //recent input sorted on top
    Input(InputData ip);
    static bool is_released(int key);
    static bool is_pressed(int key);
    static bool is_pressing(int key);
};
