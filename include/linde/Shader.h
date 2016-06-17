#ifndef LINDE_SHADER_H
#define LINDE_SHADER_H

// glsl graphics pipeline shader, and compute shader

#include "Stopwatch.h"
#include "lindeOpenGL.h"

#define SHADER_CHECK

/*
    #################################################################################
    #################################################################################
    #################################################################################
    ################### Shader ######################################################
    #################################################################################
    #################################################################################
    #################################################################################
    #################################################################################
    ####################*/
/**
    * @author Thomas Lindemeier
    * @date 03.06.2013
    *
    * University of Konstanz-
    * Department for Computergraphics
    */
namespace linde
{

class AbstractShader
{
protected:

    GLuint                                      m_progHandle;

    const GLchar *  readFile(const std::string & filename) const;

public:
    AbstractShader();
    virtual ~AbstractShader();

    void bind(GLboolean bind);

    GLuint getProgHandle() const;

    void seti(const GLchar* label, int arg);
    void setf(const GLchar* label, float arg);
    void set2i(const GLchar* label, int arg1, int arg2);
    void set2f(const GLchar* label, float arg1, float arg2);
    void set3i(const GLchar* label, int arg1, int arg2, int arg3);
    void set3f(const GLchar* label, float arg1, float arg2, float arg3);
    void set3f(const GLchar* label, const glm::vec3 &v);
    void set4i(const GLchar* label, int arg1, int arg2, int arg3, int arg4);
    void set4f(const GLchar* label, float arg1, float arg2, float arg3, float arg4);
    void set3iv(const GLchar* label, const int* args);
    void set3fv(const GLchar* label, const float* args);
    void set4fv(const GLchar* label, const float* args);
    void set4f(const GLchar* label, const glm::vec4 &v);
    void setMatrix(const GLchar* label, const float* m, bool transpose = false);
    void setMatrix(const GLchar* label, const double* m, bool transpose = false);
    void setMatrix(const GLchar* label, const glm::mat4 &mat);
    void setMatrix(const GLchar* label, const glm::mat3 &mat);


    GLuint getCurrentlyBoundProgram() const;

    virtual void checkShaderChanged() = 0;

private:

};


class Shader : public AbstractShader
{
protected:

    long int	m_vfileWriteTime;
    long int	m_cfileWriteTime;
    long int	m_efileWriteTime;
    long int	m_gfileWriteTime;
    long int	m_ffileWriteTime;

    std::string m_vFileName;
    std::string m_cFileName;
    std::string m_eFileName;
    std::string m_gFileName;
    std::string m_fFileName;

    GLuint m_vertProg;
    GLuint m_contProg;
    GLuint m_evalProg;
    GLuint m_geomProg;
    GLuint m_fragProg;

    GLuint compile(const GLchar *source, GLuint type);
    void cleanUp();


    void link() const;


public:
    Shader();
    Shader(const std::string &vFileName, const std::string &gFileName, const std::string &fFileName);
    Shader(const std::string &vFileName, const std::string &fFileName);
    virtual ~Shader();

    void checkShaderChanged();

    void attachVertexShader(const std::string &fileName);
    void attachControlShader(const std::string &fileName);
    void attachEvaluationShader(const std::string &fileName);
    void attachGeometryShader(const std::string &fileName);
    void attachFragmentShader(const std::string &fileName);

    void bindAttribLocation(const GLchar *label, GLuint attribID);

    void bindAttribLocations();

};



/**
    * @author Thomas Lindemeier
    * @date 03.11.2014
    *
    * University of Konstanz-
    * Department for Computergraphics
    */
class ComputeShader : public AbstractShader
{

protected:

    std::string		m_filename;
    long int		m_fileWriteTime;

    GLuint			m_shaderHandle;

    void  attach(const std::string & filename);

    GLuint compile(const GLchar * source);

    void cleanUp();



public:
    ComputeShader();
    ComputeShader(const std::string & filename);

    virtual ~ComputeShader();

    void checkShaderChanged();

    // start the job with specified number of groups
    void dispatchCompute(
            GLuint num_groups_x, GLuint num_groups_y, GLuint num_groups_z) const;
    // start the job with specified number of groups and sizes
    void dispatchCompute(
            GLuint num_groups_x, GLuint num_groups_y, GLuint num_groups_z,
            GLuint group_size_x, GLuint group_size_y, GLuint group_size_z) const;
    // set a memory barrier
    void  memoryBarrier(GLbitfield barrierType = GL_ALL_BARRIER_BITS) const;

    glm::ivec3
    getWorkGroupSize();
    glm::ivec3
    getMaxWorkGroupSize() const;

};

} // namespace linde

#endif // LINDE_SHADER_H
