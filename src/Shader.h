#ifndef HW3_SHADER_H
#define HW3_SHADER_H

#include <GL/gl3w.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

class Shader {
public:
    Shader(const char* vertexShaderUrl, const char*  fragmentShaderUrl);

    void use();

private:
    unsigned int PID;

};


#endif //HW3_SHADER_H
