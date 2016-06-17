#ifndef MULTIGRIDDIFFUSION_H
#define MULTIGRIDDIFFUSION_H

#include "linde.h"

namespace linde
{
// author Thomas Lindemeier

class GLWindow;
class Texture;
class ComputeShader;


class GPU_MultiGridDiffusion
{
    int                             m_nSmooth;
    GLWindow*                       m_glwindow;
    std::shared_ptr<ComputeShader>  m_jacobiShader;
    std::shared_ptr<ComputeShader>  m_restrictShader;
    std::shared_ptr<ComputeShader>  m_prolongationShader;
    int                             m_steps;


    GPU_MultiGridDiffusion();

public:
    GPU_MultiGridDiffusion(GLWindow* window);
    ~GPU_MultiGridDiffusion();

    // alpha channel is constraint mask
    void solve(cv::Mat_<glm::vec4> &psi);

private:
    void relaxation(std::shared_ptr<Texture> &m0, const int iterations) const;
    void restriction(const std::shared_ptr<Texture> &u, std::shared_ptr<Texture> &U) const;
    void prolongation(const std::shared_ptr<Texture> &U, std::shared_ptr<Texture> &u) const;
    void vCycle(std::shared_ptr<Texture> &u) const;
 };




} // namespace linde


#endif // MULTIGRIDDIFFUSION_H

