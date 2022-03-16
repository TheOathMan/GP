#include "Input.h"

 void InputData::print(){
    const char* str;
    switch (action)
    {
        case (InputState::Pressed): str = "Preseed"; break;
        case (InputState::Released): str = "Released"; break;
        case (InputState::Hold):  str = "Hold"; break;
        case (InputState::NONE):  str = "None"; break;
    }
    GP_Print("Key: " << key << " action: " << str);
}
bool InputData::operator == (InputData other){ if(key == other.key)return true; return false;}
void InputData::Swap(InputData& other) {std::swap(action,other.action);std::swap(key,other.key); }

Input::Input(InputData ip){ 
    auto input = std::find(input_pool.begin(),input_pool.end(),ip);
    if(input == input_pool.end() ){
        input_pool.push_back(ip);
    }
    else{
        (*input).action = ip.action;
        // reset all released input on a new pressed entry
        if((*input).action == InputState::Pressed) for(auto&& i: input_pool){ if(i.action == InputState::Released) {i.action = InputState::NONE;} }
        //updated input pushed on top.
        for(auto itr = input_pool.begin();(*input).key != (*itr).key;itr++) (*input).Swap(*itr);
    }
} 

std::vector<InputData> Input::input_pool;
bool Input::is_released(int key){
    auto input = std::find_if(input_pool.begin(),input_pool.end(),[=](InputData a){return a.key == key;});
    if(input != input_pool.end()  && (*input).action == InputState::Released){
        (*input).action = InputState::NONE;
        return true;
    }
    return false;
}
bool Input::is_pressed(int key){
    auto input = std::find_if(input_pool.begin(),input_pool.end(),[=](InputData a){return a.key == key;});
    if(input != input_pool.end()  && (*input).action == InputState::Pressed){
        (*input).action = InputState::NONE;
        return true;
    }
    return false;
}
bool Input::is_pressing(int key){
    auto input = std::find_if(input_pool.begin(),input_pool.end(),[=](InputData a){return a.key == key;});
    if(input != input_pool.end()  && ((*input).action == InputState::Pressed || (*input).action == InputState::Hold)){
        return true;
    }
    return false;
}
