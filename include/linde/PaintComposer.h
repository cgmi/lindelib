#ifndef PAINTCOMPOSER_H
#define PAINTCOMPOSER_H

#include "linde.h"
#include "Color.h"


/**
* @author Thomas Lindemeier
*
* University of Konstanz
* Department of Computer and Information Science
* Work Group Computer Graphics
*/
namespace linde
{

class PaintComposer
{
    std::vector<Paint>      m_palette;
    std::vector<glm::vec3>  m_reflectance;
    std::vector<glm::vec3>  m_transmittance;
    glm::vec3               m_Rsource;
    glm::vec3               m_Rtarget;

    static const std::string SERIALIZE_TYPE_KS;
    static const std::string SERIALIZE_TYPE_RR;

public:
    PaintComposer();
    ~PaintComposer();

    Paint getPaint(int index) const;

    void addPaint(const Paint & paint);

    void clear();

    glm::vec3   composeDry(std::vector<Paint> & usedPaints, float eps = 0.1f, int maxIterations = 300) const;
    void        composeWet(std::vector<Paint> & usedPaints, std::vector<float> & paintConcentrations) const;

    glm::vec3 getSourceReflectance() const;
    void setSourceReflectance(const glm::vec3 &R);

    glm::vec3 getTargetReflectance() const;
    void setTargetReflectance(const glm::vec3 &R);

    void serialize(std::ostream & ostream) const;
    void deserialize(std::istream & istream);

    size_t size() const;
};

} // namespace linde

#endif // PAINTCOMPOSER_H
