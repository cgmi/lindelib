#include "../include/linde/MultiGridDiffusion.h"
#include "../include/linde/GLWindow.h"
#include "../include/linde/Texture.h"
#include "../include/linde/Shader.h"

namespace linde
{


GPU_MultiGridDiffusion::GPU_MultiGridDiffusion():
    m_nSmooth(5),
    m_glwindow(nullptr),
    m_jacobiShader(nullptr),
    m_restrictShader(nullptr),
    m_prolongationShader(nullptr),
    m_steps(1)
{
}

GPU_MultiGridDiffusion::GPU_MultiGridDiffusion(GLWindow* window) :
    GPU_MultiGridDiffusion()
{
    m_glwindow = window;

    m_jacobiShader = m_glwindow->createComputeShader("shaders/lindeLibShaders/MultiGridDiffusion_jacobi.glsl");
    m_restrictShader = m_glwindow->createComputeShader("shaders/lindeLibShaders/MultiGridDiffusion_restriction.glsl");
    m_prolongationShader = m_glwindow->createComputeShader("shaders/lindeLibShaders/MultiGridDiffusion_prolongation.glsl");
}

GPU_MultiGridDiffusion::~GPU_MultiGridDiffusion()
{

}

uint nextPowerOf2(uint n)
{
    n--;
    n |= n >> 1;   // Divide by 2^k for consecutive doublings of k up to 32,
    n |= n >> 2;   // and then or the results.
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    n++;
    return n;
}

void GPU_MultiGridDiffusion::solve(cv::Mat_<glm::vec4> &psi_in)
{
    // ensure power of two size and squared
    const cv::Size oSize = psi_in.size();
    cv::Size sqSize;
    sqSize.width = sqSize.height = nextPowerOf2((uint)std::max(psi_in.rows, psi_in.cols));
    cv::Mat_<glm::vec4> psi(sqSize);
    for (int i = 0; i< sqSize.width*sqSize.height; i++)
    {
        psi(i) = glm::vec4(0.f, 0.f, 0.f, 0.f);
    }
    psi_in.copyTo(psi(cv::Rect(0, 0, oSize.width, oSize.height)));

    std::shared_ptr<Texture> m0 = m_glwindow->createTexture(psi.cols, psi.rows, GL_RGBA32F, GL_RGBA, GL_FLOAT, GL_NEAREST, GL_NEAREST);

    cv::Mat_<glm::vec4> tempFlip;
    cv::flip(psi, tempFlip, 0);
    m0->create(tempFlip.data);

    for (int steps = 0; steps < m_steps; steps++)
    {
        vCycle(m0);

//        m0->render(0, 0, m_glwindow->getWidth(), m_glwindow->getHeight());
//        m_glwindow->update(true);

        //std::cout << "vcycle iteration: " << steps << std::endl;
    }
    // get result from GPU
    m0->bind();
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, tempFlip.data);
    m0->unbind();
    cv::flip(tempFlip, psi, 0);

    // remove borders
    psi(cv::Rect(0, 0, oSize.width, oSize.height)).copyTo(psi_in);
}



void GPU_MultiGridDiffusion::relaxation(std::shared_ptr<Texture> &m0, const int iterations) const
{
    std::shared_ptr<Texture> m1 = m_glwindow->createTexture(m0->width(), m0->height(), m0->getInternalFormat(),  m0->getFormat(),  m0->getType(), m0->getMinFilter(), m0->getMagFilter());
    m1->create(nullptr);

    m_jacobiShader->bind(true);
    int step;
    for (step = 0; step < iterations; step++)
    {
        if ((step % 2) == 0)
        {
            m0->bindLocationUnit(0, GL_READ_ONLY);
            m1->bindLocationUnit(1, GL_WRITE_ONLY);
        } else
        {
            m1->bindLocationUnit(0, GL_READ_ONLY);
            m0->bindLocationUnit(1, GL_WRITE_ONLY);
        }

        glm::vec3 workSize = m_jacobiShader->getWorkGroupSize();
        m_jacobiShader->dispatchCompute(m0->width() / workSize.x + 1, m0->height() / workSize.y + 1, 1);
        m_jacobiShader->memoryBarrier();
        m0->unbind();
        m1->unbind();
    }
    m_jacobiShader->bind(false);

    if ((step % 2) == 1)
    {
        m0 = m1;
    }
}

void GPU_MultiGridDiffusion::restriction(const std::shared_ptr<Texture> &u, std::shared_ptr<Texture> &U) const
{
    m_restrictShader->bind(true);
    u->bindLocationUnit(0, GL_READ_ONLY);
    U->bindLocationUnit(1, GL_WRITE_ONLY);
    glm::vec3 workSize = m_restrictShader->getWorkGroupSize();
    m_restrictShader->dispatchCompute(U->width() / workSize.x + 1, U->height() / workSize.y + 1, 1);
    m_restrictShader->memoryBarrier();
    m_restrictShader->bind(false);

}

void GPU_MultiGridDiffusion::prolongation(const std::shared_ptr<Texture> &U, std::shared_ptr<Texture> &u) const
{    
    m_prolongationShader->bind(true);
    U->bindLocationUnit(0, GL_READ_ONLY);
    u->bindLocationUnit(1, GL_READ_WRITE);
    glm::vec3 workSize = m_prolongationShader->getWorkGroupSize();
    m_prolongationShader->dispatchCompute(U->width() / workSize.x + 1, U->height() / workSize.y + 1, 1);
    m_prolongationShader->memoryBarrier();
    m_prolongationShader->bind(false);

}


void GPU_MultiGridDiffusion::vCycle(std::shared_ptr<Texture> &u) const
{
    const int uw = u->width();
    const int uh = u->height();
    const int Uw = uw / 2;
    const int Uh = uh / 2;

    const int lvl = round(log(uw) / log(2.f));

    //std::cout << "w: " << u->width() << " h: " << u->height() << std::endl;

    if (Uw <= 0 || Uh <= 0)
    {
        return;
    }

    // pre smoothings
    relaxation(u, lvl*m_nSmooth);

    // restriction
    std::shared_ptr<Texture> U = m_glwindow->createTexture(Uw, Uh, u->getInternalFormat(),  u->getFormat(),  u->getType(), u->getMinFilter(), u->getMagFilter());
    U->create(nullptr);
    restriction(u, U);

    // call recursively
    vCycle(U);

    // inject solution
    prolongation(U, u);

    // post smoothings
    relaxation(u, lvl*m_nSmooth);
}

} // namespace linde
