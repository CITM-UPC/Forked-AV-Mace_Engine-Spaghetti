#include "Shader.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <GL/glew.h>

Shader::Shader(const string& filepath) : m_FilePath(filepath), m_RendererID(0)
{
    ShaderProgramSource source = ParseShader(filepath);
    //check strings
    /*
    cout << "VERTEX" << endl;
    cout << source.VertexSource << endl;
    cout << "FRAGMENT" << endl;
    cout << source.FragmentSource << endl;
    */
    m_RendererID = CreateShader(source.VertexSource, source.FragmentSource);
}

Shader::~Shader()
{
    glDeleteProgram(m_RendererID);
}

ShaderProgramSource Shader::ParseShader(const std::string& filepath)
{
    ifstream stream(filepath);

    enum class ShaderType {
        NONE = -1,
        VERTEX = 0,
        FRAGMENT = 1
    };

    string line;
    stringstream ss[2];
    ShaderType type = ShaderType::NONE;
    while (getline(stream, line))
    {
        if (line.find("#shader") != string::npos) {
            if (line.find("vertex") != string::npos) {
                type = ShaderType::VERTEX;
            }
            else if (line.find("fragment") != string::npos) {
                type = ShaderType::FRAGMENT;
            }
        }
        else {
            ss[(int)type] << line << '\n';
        }
    }

    return { ss[0].str(), ss[1].str() };
}

unsigned int Shader::CompileShader(unsigned int type, const std::string& source)
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();

    //especifica sel source del shader
    //id, how many source code, el source, (lenght) nullptr para llega al final
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    //  Todo: Error Handling
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (!result)
    {
        int lenght;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &lenght);
        char* message = (char*)alloca(lenght * sizeof(char));
        glGetShaderInfoLog(id, lenght, &lenght, message);

        cout << "Failed to compile " <<
            (type == GL_VERTEX_SHADER ? "vertex" : "fragment")
            << " shader!" << endl;
        cout << message << endl;

        glDeleteShader(id);

        return 0;
    }

    return id;
}

unsigned int Shader::CreateShader(const std::string& VertexShader, const std::string& FragmentShader)
{
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, VertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, FragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

void Shader::Bind() const
{
    glUseProgram(m_RendererID);
}

void Shader::UnBind() const
{
    glUseProgram(0);
}

void Shader::SetUniform4f(const string& name, float v0, float v1, float v2, float v3)
{
    glUniform4f(GetUniformLocation(name), v0, v1, v2, v3);
}

void Shader::SetUniform1i(const string& name, int value)
{
    glUniform1i(GetUniformLocation(name), value);
}

void Shader::SetUniformBool(const string& name, bool value)
{
    glUniform1i(GetUniformLocation(name), value);
}

void Shader::SetUniform1iv(const string& name, int size, int value[])
{
    glUniform1iv(GetUniformLocation(name), size ,value);
}

void Shader::SetUniformMat4f(const string& name, const glm::mat4& matrix)
{
    glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]);
}

int Shader::GetUniformLocation(const string& name)
{
    if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end()) {
        return (unsigned int)m_UniformLocationCache[name];
    }

    int location = glGetUniformLocation(m_RendererID, name.c_str());
    if (location == -1) {
        cout << "Warning: uniform '" << name << "' doesn't exist!" << endl;
    }

    m_UniformLocationCache[name] = location;
    return (unsigned int)location;
}
