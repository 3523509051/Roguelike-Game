#ifndef INPUT_H
#define INPUT_H

class Input {
public:
    Input();
    char getKey();  // 阻塞等待按键，返回字符
};

#endif // INPUT_H
