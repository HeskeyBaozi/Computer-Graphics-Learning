
#include "Shader.h"

using namespace std;

Shader::Shader(const char *vertexShaderUrl, const char *fragmentShaderUrl) {
    ifstream vertexFile, fragmentFile;

    vertexFile.exceptions(ifstream::failbit | ifstream::badbit);
    fragmentFile.exceptions(ifstream::failbit | ifstream::badbit);


    try {
        vertexFile.open(vertexShaderUrl);
        fragmentFile.open(fragmentShaderUrl);

        stringstream vertexStream, fragmentStream;

        vertexStream << vertexFile.rdbuf();
        fragmentStream << fragmentFile.rdbuf();

        string vertexShaderCode, fragmentShaderCode;

        const char *vertexShaderSource;
        const char *fragmentShaderSource;

        vertexShaderCode = vertexStream.str();
        fragmentShaderCode = fragmentStream.str();

        vertexShaderSource = vertexShaderCode.c_str();
        fragmentShaderSource = fragmentShaderCode.c_str();

        /************************************************************
        *                  Shader Setup
        ************************************************************/

        // Compile Shader
        unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
        glCompileShader(vertexShader);

        // test
        int isSuccessful = 0;
        char infoLog[512] = "";
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isSuccessful);
        if (!isSuccessful) {
            glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                      << infoLog << std::endl;
        }

        // Fragment Shader
        unsigned int fragmentShader;
        fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
        glCompileShader(fragmentShader);

        // test
        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isSuccessful);
        if (!isSuccessful) {
            glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
            std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                      << infoLog << std::endl;
        }

        // Link
        this->PID = glCreateProgram();
        glAttachShader(this->PID, vertexShader);
        glAttachShader(this->PID, fragmentShader);
        glLinkProgram(this->PID);

        // test
        glGetProgramiv(this->PID, GL_LINK_STATUS, &isSuccessful);
        if (!isSuccessful) {
            glGetProgramInfoLog(this->PID, 512, nullptr, infoLog);
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                      << infoLog << std::endl;
        }

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    } catch (ifstream::failure) {
        cout << "Read File Failed" << endl;
    }
}

void Shader::use() {
    glUseProgram(this->PID);
}
