#include "../include/linde/Shader.h"
#include "../include/linde/File.h"

/*
    #################################################################################
    #################################################################################
    #################################################################################
    ################### Shader ######################################################
    #################################################################################
    #################################################################################
    #################################################################################
    #################################################################################
    #################################################################################
    */
namespace linde
{


AbstractShader::AbstractShader() :
    m_progHandle(0)
{
    if (!m_progHandle)
        m_progHandle = glCreateProgram();
}

AbstractShader::~AbstractShader()
{
    if (m_progHandle)
    {
        glDeleteProgram(m_progHandle);
        m_progHandle = 0;
    }
}

void AbstractShader::bind(GLboolean bind)
{
    if (bind)
    {
//#ifdef DEBUG_BUILD
//        checkShaderChanged();
//#endif
        glUseProgram(m_progHandle);
    }
    else
    {
        glUseProgram(0);
    }
}

const GLchar * AbstractShader::readFile(const std::string & filename) const
{
    FILE *fp = NULL;
    GLchar *content = NULL;
    int count = 0;

    if (!filename.empty())
    {
        fp = fopen(filename.c_str(), "rt");

        if (fp)
        {
            fseek(fp, 0, SEEK_END);
            count = ftell(fp);
            rewind(fp);

            if (count > 0)
            {
                content = new GLchar[count + 1];
                count = (GLint)fread(content, sizeof(GLchar), count, fp);
                content[count] = '\0';
            }

            fclose(fp);
        }
    }

    return content;
}

GLuint AbstractShader::getProgHandle() const
{
    return m_progHandle;
}

GLuint AbstractShader::getCurrentlyBoundProgram() const
{
    GLint id;
    glGetIntegerv(GL_CURRENT_PROGRAM, &id);

    return (GLuint) id;
}

void AbstractShader::seti(const GLchar* label, int arg)
{
    glUniform1i(glGetUniformLocation(m_progHandle, label), arg);
}

void AbstractShader::setf(const GLchar* label, float arg)
{
    glUniform1f(glGetUniformLocation(m_progHandle, label), arg);
}

void AbstractShader::set2i(const GLchar* label, int arg1, int arg2)
{
    glUniform2i(glGetUniformLocation(m_progHandle, label), arg1, arg2);
}

void AbstractShader::set2f(const GLchar* label, float arg1, float arg2)
{
    glUniform2f(glGetUniformLocation(m_progHandle, label), arg1, arg2);
}

void AbstractShader::set3i(const GLchar* label, int arg1, int arg2, int arg3)
{
    glUniform3i(glGetUniformLocation(m_progHandle, label), arg1, arg2, arg3);
}

void AbstractShader::set3f(const GLchar* label, float arg1, float arg2, float arg3)
{
    glUniform3f(glGetUniformLocation(m_progHandle, label), arg1, arg2, arg3);
}

void AbstractShader::set3f(const GLchar* label, const glm::vec3 &v)
{
    glUniform3f(glGetUniformLocation(m_progHandle, label), v[0], v[1], v[2]);
}

void AbstractShader::set4f(const GLchar* label, const glm::vec4 &v)
{
    glUniform4f(glGetUniformLocation(m_progHandle, label), v[0], v[1], v[2], v[3]);
}

void AbstractShader::set4i(const GLchar* label, int arg1, int arg2, int arg3, int arg4)
{
    glUniform4i(glGetUniformLocation(m_progHandle, label), arg1, arg2, arg3, arg4);
}

void AbstractShader::set4f(const GLchar* label, float arg1, float arg2, float arg3, float arg4)
{
    glUniform4f(glGetUniformLocation(m_progHandle, label), arg1, arg2, arg3, arg4);
}

void AbstractShader::set3iv(const GLchar* label, const int* args)
{
    glUniform3iv(glGetUniformLocation(m_progHandle, label), 1, args);
}

void AbstractShader::set3fv(const GLchar* label, const float* args)
{
    glUniform3fv(glGetUniformLocation(m_progHandle, label), 1, args);
}

void AbstractShader::set4fv(const GLchar* label, const float* args)
{
    glUniform4fv(glGetUniformLocation(m_progHandle, label), 1, args);
}

void AbstractShader::setMatrix(const GLchar* label, const float* m, bool transpose)
{
    glUniformMatrix4fv(glGetUniformLocation(m_progHandle, label), 1, transpose, m);
}

void AbstractShader::setMatrix(const GLchar* label, const double* m, bool transpose)
{
    glUniformMatrix4dv(glGetUniformLocation(m_progHandle, label), 1, transpose, m);
}

void AbstractShader::setMatrix(const GLchar* label, const glm::mat4 &mat)
{
    glUniformMatrix4fv(glGetUniformLocation(m_progHandle, label), 1, GL_FALSE, &(mat[0][0]));
}

void AbstractShader::setMatrix(const GLchar* label, const glm::mat3 &mat)
{
    glUniformMatrix3fv(glGetUniformLocation(m_progHandle, label), 1, GL_FALSE, &(mat[0][0]));
}



Shader::Shader() :
    AbstractShader(),
    m_vertProg(0),
    m_contProg(0),
    m_evalProg(0),
    m_geomProg(0),
    m_fragProg(0)
{

}

Shader::Shader(const std::string &vFileName, const std::string &fFileName) :
    AbstractShader(),
    m_vertProg(0),
    m_contProg(0),
    m_evalProg(0),
    m_geomProg(0),
    m_fragProg(0)
{

    attachVertexShader(vFileName);
    attachFragmentShader(fFileName);
}

Shader::Shader(const std::string &vFileName, const std::string &gFileName, const std::string &fFileName) :
    AbstractShader(),
    m_vertProg(0),
    m_contProg(0),
    m_evalProg(0),
    m_geomProg(0),
    m_fragProg(0)
{
    attachVertexShader(vFileName);
    attachGeometryShader(gFileName);
    attachFragmentShader(fFileName);
}

Shader::~Shader()
{
    cleanUp();
}

void Shader::attachVertexShader(const std::string &fileName)
{
    if (!fileName.empty())
    {
        m_vFileName = fileName;

        const GLchar *source = readFile(fileName);
        m_vfileWriteTime = linde::getFileChangedTimeStamp(m_vFileName);

        if (source)
        {
            m_vertProg = compile(source, GL_VERTEX_SHADER);
            glAttachShader(m_progHandle, m_vertProg);

            glLinkProgram(m_progHandle);

            myDebug << "SHADER::attachVertexShader():attached: " << fileName << std::endl;
        }
        else
        {
            std::cerr << "SHADER::attachVertexShader():not found: " << fileName << std::endl;
        }
    }
}

void Shader::attachControlShader(const std::string &fileName)
{
    if (!fileName.empty())
    {
        m_cFileName = fileName;
        const GLchar *source = readFile(fileName);

        m_cfileWriteTime = linde::getFileChangedTimeStamp(m_cFileName);

        if (source)
        {
            m_contProg = compile(source, GL_TESS_CONTROL_SHADER);
            glAttachShader(m_progHandle, m_contProg);

            glLinkProgram(m_progHandle);

            myDebug << "SHADER::attachControlShader():attached: " << fileName << std::endl;
        }
        else
        {
            std::cerr << "SHADER::attachControlShader()::not found: " << fileName << std::endl;
        }

    }
}

void Shader::attachEvaluationShader(const std::string &fileName)
{
    if (!fileName.empty())
    {
        m_eFileName = fileName;
        const GLchar *source = readFile(fileName);

        m_efileWriteTime = linde::getFileChangedTimeStamp(m_eFileName);

        if (source)
        {
            m_evalProg = compile(source, GL_TESS_EVALUATION_SHADER);
            glAttachShader(m_progHandle, m_evalProg);

            glLinkProgram(m_progHandle);

            myDebug << "SHADER::attachEvaluationShader():attached: " << fileName << std::endl;
        }
        else
        {
            std::cerr << "SHADER::attachEvaluationShader()::not found: " << fileName << std::endl;
        }

    }
}

void Shader::attachGeometryShader(const std::string &fileName)
{
    if (!fileName.empty())
    {
        m_gFileName = fileName;
        const GLchar *source = readFile(fileName);

        m_gfileWriteTime = linde::getFileChangedTimeStamp(m_gFileName);
        if (source)
        {
            m_geomProg = compile(source, GL_GEOMETRY_SHADER);
            glAttachShader(m_progHandle, m_geomProg);

            glLinkProgram(m_progHandle);

            myDebug << "SHADER::attachGeometryShader():attached: " << fileName << std::endl;
        }
        else
        {
            std::cerr << "SHADER::attachGeometryShader()::not found: " << fileName << std::endl;
        }
    }
}

void Shader::attachFragmentShader(const std::string &fileName)
{
    if (!fileName.empty())
    {
        m_fFileName = fileName;
        const GLchar *source = readFile(fileName);

        m_ffileWriteTime = linde::getFileChangedTimeStamp(m_fFileName);

        if (source)
        {
            m_fragProg = compile(source, GL_FRAGMENT_SHADER);

            if (m_fragProg)
            {
                glAttachShader(m_progHandle, m_fragProg);
                glLinkProgram(m_progHandle);
                myDebug << "SHADER::attachFragmentShader():attached: " << fileName << std::endl;
            }

        }
        else
        {
            std::cerr << "SHADER::attachFragmentShader()::not found: " << fileName << std::endl;
        }
    }
}



GLuint Shader::compile(const GLchar *source, GLuint type)
{
    GLuint id = glCreateShader(type);
    glShaderSource(id, 1, &source, NULL);
    glCompileShader(id);

    int c = 0;

    glGetShaderiv(id, GL_COMPILE_STATUS, &c);

    if (!c)
    {
        GLchar* logstr = new GLchar[2048];
        glGetShaderInfoLog(id, 2048, NULL, logstr);
        std::cerr << "SHADER::Error compiling shader" << "\n" << source << "\n" << logstr << std::endl;
        delete[] logstr;
        return 0;
    }

    return id;
}

void Shader::cleanUp()
{
    if (m_progHandle)
    {
        if (m_vertProg)
        {
            glDetachShader(m_progHandle, m_vertProg);
            glDeleteShader(m_vertProg);
            m_vertProg = 0;
        }

        if (m_contProg)
        {
            glDetachShader(m_progHandle, m_contProg);
            glDeleteShader(m_contProg);
            m_contProg = 0;
        }

        if (m_evalProg)
        {
            glDetachShader(m_progHandle, m_evalProg);
            glDeleteShader(m_evalProg);
            m_evalProg = 0;
        }

        if (m_geomProg)
        {
            glDetachShader(m_progHandle, m_geomProg);
            glDeleteShader(m_geomProg);
            m_geomProg = 0;
        }

        if (m_fragProg)
        {
            glDetachShader(m_progHandle, m_fragProg);
            glDeleteShader(m_fragProg);
            m_fragProg = 0;
        }
    }
}

void Shader::checkShaderChanged()
{
    if (!m_vFileName.empty() && linde::getFileChangedTimeStamp(m_vFileName) != m_vfileWriteTime)
    {
        glDetachShader(m_progHandle, m_vertProg);
        glDeleteShader(m_vertProg);
        m_vertProg = 0;

        attachVertexShader(m_vFileName);
    }
    if (!m_cFileName.empty() && linde::getFileChangedTimeStamp(m_cFileName) != m_cfileWriteTime)
    {
        glDetachShader(m_progHandle, m_contProg);
        glDeleteShader(m_contProg);
        m_contProg = 0;

        attachControlShader(m_cFileName);
    }
    if (!m_eFileName.empty() && linde::getFileChangedTimeStamp(m_eFileName) != m_efileWriteTime)
    {
        glDetachShader(m_progHandle, m_evalProg);
        glDeleteShader(m_evalProg);
        m_evalProg = 0;

        attachEvaluationShader(m_eFileName);
    }
    if (!m_fFileName.empty() && linde::getFileChangedTimeStamp(m_fFileName) != m_ffileWriteTime)
    {
        glDetachShader(m_progHandle, m_fragProg);
        glDeleteShader(m_fragProg);
        m_fragProg = 0;

        attachFragmentShader(m_fFileName);
    }
    if (!m_gFileName.empty() && linde::getFileChangedTimeStamp(m_gFileName) != m_gfileWriteTime)
    {

        glDetachShader(m_progHandle, m_geomProg);
        glDeleteShader(m_geomProg);
        m_geomProg = 0;

        attachGeometryShader(m_gFileName);
    }
}



void Shader::link() const
{
    glLinkProgram(m_progHandle);
}

void Shader::bindAttribLocation(const GLchar *label, GLuint attribID)
{
    glBindAttribLocation(m_progHandle, attribID, label);
}


void Shader::bindAttribLocations()
{
    this->bindAttribLocation("Position", VERTEX_POSITION);
    this->bindAttribLocation("Normal", VERTEX_NORMAL);
    this->bindAttribLocation("Color", VERTEX_COLOR);
    this->bindAttribLocation("Texture", VERTEX_TEXTURE);
}

ComputeShader::ComputeShader() :
    AbstractShader(),
    m_filename(),
    m_fileWriteTime(-1)
{

}

ComputeShader::~ComputeShader()
{
    cleanUp();
}

ComputeShader::ComputeShader(const std::string & filename) :
    AbstractShader(),
    m_filename(),
    m_fileWriteTime(-1)
{
    attach(filename);
}


void ComputeShader::attach(const std::string & fileName)
{
    if (!fileName.empty())
    {
        m_filename = fileName;

        const GLchar * source = readFile(fileName);
        m_fileWriteTime = linde::getFileChangedTimeStamp(fileName);

        if (source)
        {
            m_shaderHandle = compile(source);
            glAttachShader(m_progHandle, m_shaderHandle);

            glLinkProgram(m_progHandle);
            int rvalue;
            glGetProgramiv(m_progHandle, GL_LINK_STATUS, &rvalue);
            if (!rvalue)
            {
                std::cerr << "Error in linking compute shader program" << std::endl;
                GLchar log[10240];
                GLsizei length;
                glGetProgramInfoLog(m_progHandle, 10239, &length, log);
                std::cerr << source << "\n" << log << std::endl;
            }

            myDebug << "SHADER::attachComputeShader():attached: " << fileName << std::endl;
        }
        else
        {
            std::cerr << "SHADER::attachComputeShader():failed: " << fileName << std::endl;
        }

    }
}




GLuint ComputeShader::compile(const GLchar * source)
{
    GLuint id = glCreateShader(GL_COMPUTE_SHADER);

    glShaderSource(id, 1, &source, NULL);
    glCompileShader(id);

    int c = 0;

    glGetShaderiv(id, GL_COMPILE_STATUS, &c);

    if (!c)
    {
        GLchar* logstr = new GLchar[2048];
        glGetShaderInfoLog(id, 2048, NULL, logstr);
        std::cerr << "SHADER::Error compiling shader" << "\n" << source << "\n" << logstr << std::endl;
        delete[] logstr;
        return 0;
    }

    return id;
}

void ComputeShader::cleanUp()
{
    glDetachShader(m_progHandle, m_shaderHandle);
    glDeleteShader(m_shaderHandle);
}

void ComputeShader::checkShaderChanged()
{
    if (linde::getFileChangedTimeStamp(m_filename) != m_fileWriteTime)
    {
        cleanUp();
        m_progHandle = glCreateProgram();

        attach(m_filename);
    }
}

glm::ivec3	ComputeShader::getWorkGroupSize()
{
//    GLuint currentProgram = getCurrentlyBoundProgram();
//    if (currentProgram != m_progHandle)
//    {
//        bind(true);
//    }
    glm::ivec3 size;
    glGetProgramiv(m_progHandle, GL_COMPUTE_WORK_GROUP_SIZE, &(size[0]));

//    if (currentProgram != m_progHandle)
//    {
//        bind(false);
//        glUseProgram(currentProgram);
//    }

    return size;
}

glm::ivec3	ComputeShader::getMaxWorkGroupSize() const
{
    glm::ivec3 size;
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &(size[0]));
    return size;
}

void ComputeShader::dispatchCompute(GLuint num_groups_x, GLuint num_groups_y, GLuint num_groups_z) const
{
    glDispatchCompute(num_groups_x, num_groups_y, num_groups_z);
}

void ComputeShader::dispatchCompute(
        GLuint num_groups_x, GLuint num_groups_y, GLuint num_groups_z,
        GLuint group_size_x, GLuint group_size_y, GLuint group_size_z) const
{
    glDispatchComputeGroupSizeARB(num_groups_x, num_groups_y, num_groups_z, group_size_x, group_size_y, group_size_z);
}

void ComputeShader::memoryBarrier(GLbitfield barrierType) const
{
    glMemoryBarrier(barrierType);
}



} // namespace linde
