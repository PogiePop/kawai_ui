#include <shader>
#include <ka_basic>
#include <sstream>
#include <fstream>
#include <glm/gtc/type_ptr.hpp>

namespace Kawai
{
    Shader::Shader(const char* vertexShader, const char* geometryShader, const char* fragmentShader)
    {
        if(!vertexShader)
        {
            KA_ASSERT(vertexShader, "一个着色器至少包含一个vertexShader!!!");
        }
        std::ifstream vFs, gFs, fFs;
        std::string vStr, gStr, fStr;
        vFs.exceptions(std::ios::badbit | std::ios::failbit);
        gFs.exceptions(std::ios::badbit | std::ios::failbit);
        fFs.exceptions(std::ios::badbit | std::ios::failbit);
        try
        {
            vFs.open(vertexShader);
            std::stringstream vss;
            vss << vFs.rdbuf();
            vStr = vss.str();
            if(geometryShader)
            {
                gFs.open(geometryShader);
                std::stringstream gss;
                gss << gFs.rdbuf();
                gStr = gss.str();
            }
            if(fragmentShader)
            {
                fFs.open(fragmentShader);
                std::stringstream fss;
                fss << fFs.rdbuf();
                fStr = fss.str();
            }
        }catch(std::ios::failure& e)
        {
            std::println("着色器文件读取失败: {}", e.what());
            return;
        }
        GLuint vsd = 0, gsd = 0, fsd = 0;
        vsd = glCreateShader(GL_VERTEX_SHADER);
        const char* vCStr = vStr.c_str();
        glShaderSource(vsd, 1, &vCStr, nullptr);
        glCompileShader(vsd);
        CheckCompileErrors(vsd, VERTEX_SHADER);
        if(geometryShader)
        {
            gsd = glCreateShader(GL_GEOMETRY_SHADER);
            const char* gCstr = gStr.c_str();
            glShaderSource(gsd, 1, &gCstr, nullptr);
            glCompileShader(gsd);
            CheckCompileErrors(gsd, GEOMETRY_SHADER);
        }
        if(fragmentShader)
        {
            fsd = glCreateShader(GL_FRAGMENT_SHADER);
            const char* fCstr = fStr.c_str();
            glShaderSource(fsd, 1, &fCstr, nullptr);
            glCompileShader(fsd);
            CheckCompileErrors(fsd, FRAGMENT_SHADER);
        }

        ID = glCreateProgram();
        glAttachShader(ID, vsd);
        if(geometryShader)
            glAttachShader(ID, gsd);
        if(fragmentShader)
            glAttachShader(ID, fsd);
        glLinkProgram(ID);
        CheckCompileErrors(ID, PROGRAM);

        glDeleteShader(vsd);
        glDeleteShader(fsd);
        glDeleteShader(gsd);
    }

    Shader::~Shader()
    {
        glDeleteProgram(ID);
    }

    void Shader::SetMat4(const std::string& name, const glm::mat4& value)const
    {
        //get location in shader program
        int location = GetLocationFromProgram(name);
        if(location != -1)
            glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
    }

    void Shader::SetVec3(const std::string& name, const glm::vec3& value)const
    {
        int location = GetLocationFromProgram(name);
        if(location != -1)
            glUniform3fv(location, 1, glm::value_ptr(value));
    }

    void Shader::SetFloat1(const std::string& name, const float& value)const
    {
        int location = GetLocationFromProgram(name);
        if(location != -1)
            glUniform1f(location, value);
    }

    void Shader::SetInt1(const std::string& name, const int& value)const
    {
        int location = GetLocationFromProgram(name);
        if(location != -1)
            glUniform1i(location, value);
    }

    void Shader::SetVec4(const std::string& name, const glm::vec4& value)const
    {
        int location = GetLocationFromProgram(name);
        if(location != -1)
            glUniform4fv(location, 1, glm::value_ptr(value));
    }

    void Shader::SetVec2(const std::string& name, const glm::vec2& value)const
    {
        int location = GetLocationFromProgram(name);
        if(location != -1)
            glUniform2fv(location, 1, glm::value_ptr(value));
    }

    void Shader::CheckCompileErrors(const GLuint& id, const GLuint& _type)const
    {
        //不合法id
        if(id <= 0)return;
        int success;
        char infoLog[512];
        if((_type & 0x08) == 0x00)
        {
            glGetShaderiv(id, GL_COMPILE_STATUS, &success);
            if(!success)
            {
                glGetShaderInfoLog(id, sizeof(infoLog), nullptr, infoLog);
                switch(_type)
                {
                    case(VERTEX_SHADER): std::println("vertex shader compile fail: {}", infoLog);
                    break;
                    case(GEOMETRY_SHADER): std::println("geometry shader compile fail: {}", infoLog);
                    break;
                    case(FRAGMENT_SHADER): std::println("fragment shader compile fail: {}", infoLog);
                    break;
                    default: break;
                }
            }
        }
        else
        {
            glGetProgramiv(id, GL_LINK_STATUS, &success);
            if(!success)
            {
                glGetProgramInfoLog(id, sizeof(infoLog), nullptr, infoLog);
                std::println("program link fail: {}", infoLog);
            }
        }
    }

    //Shader defaultUIShader = Shader("shaders/ui.vert", nullptr, "shaders/ui.frag");
}