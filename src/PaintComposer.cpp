#include "../include/linde/PaintComposer.h"
/**
* @author Thomas Lindemeier
*
* University of Konstanz
* Department of Computer and Information Science
* Work Group Computer Graphics
*/
namespace linde
{

const std::string PaintComposer::SERIALIZE_TYPE_KS = "AbsorptionAndScattering";
const std::string PaintComposer::SERIALIZE_TYPE_RR = "ReflectionOnBlackAndWhite";

PaintComposer::PaintComposer()
{

}

PaintComposer::~PaintComposer()
{

}

Paint PaintComposer::getPaint(int index) const
{
    return m_palette[index];
}

void PaintComposer::addPaint(const Paint &paint)
{
    m_palette.push_back(paint);

    glm::vec3 R, T;
    paint.computeReflectanceAndTransmittance(R, T);
    m_reflectance.push_back(R);
    m_transmittance.push_back(T);
}

void PaintComposer::clear()
{
    m_reflectance.clear();
    m_transmittance.clear();
    m_palette.clear();
}

glm::vec3 PaintComposer::composeDry(std::vector<Paint> &usedPaints, float eps, int maxIterations) const
{
    glm::vec3 Lab0, Lab1;
    convert_rgb2lab(m_Rsource, Lab0);
    convert_rgb2lab(m_Rtarget, Lab1);

    float dist = glm::distance(Lab0, Lab1);

    usedPaints.clear();

    glm::vec3 R2;
    glm::vec3 T2;
    glm::vec3 R = m_Rsource;
    glm::vec3 tempR;
    glm::vec3 R1;
    float td;
    // as long as final reflectance is not reached
    for (int it = 0; (it < maxIterations) || (dist < eps); it++)
    {
        // find the paint the gets us closer to the final reflectance
        int best = -1;
        R1 = R;
        for (uint i = 0; i < m_palette.size(); ++i)
        {
            R2 = m_reflectance[i];
            T2 = m_transmittance[i];

            tempR = R2 + (T2*T2*R1) / (1.f - R2*R1);
            convert_rgb2lab(tempR, Lab0);

            td = glm::distance(Lab0, Lab1);

            if (td < dist)
            {
                dist = td;
                best = i;
                R = tempR;
            }
        }

        if (best >= 0)
        {
            usedPaints.push_back(m_palette[best]);
        } else
        {

            break;
        }
    }
    return R;
}


glm::vec3 PaintComposer::getSourceReflectance() const
{
    return m_Rsource;
}

void PaintComposer::setSourceReflectance(const glm::vec3 &R)
{
    m_Rsource = R;
}

glm::vec3 PaintComposer::getTargetReflectance() const
{
    return m_Rtarget;
}

void PaintComposer::setTargetReflectance(const glm::vec3 &R)
{
    m_Rtarget = R;
}

void PaintComposer::serialize(std::ostream &ostream) const
{
    ostream << SERIALIZE_TYPE_KS << std::endl;
    for (uint i = 0; i < m_palette.size(); i++)
    {
        const KMColor & c = m_palette[i].getColor();
        ostream << c.K.r << ",";
        ostream << c.K.g << ",";
        ostream << c.K.b << ",";
        ostream << c.S.r << ",";
        ostream << c.S.g << ",";
        ostream << c.S.b << ",";
        if (i == m_palette.size()-1)
        {
            ostream << m_palette[i].getOpacity();
        }
        else
        {
            ostream << m_palette[i].getOpacity() << std::endl;
        }
    }
}

// http://stackoverflow.com/questions/1120140/how-can-i-read-and-parse-csv-files-in-c
void ParseCSV(const std::string& csvSource, std::vector<std::vector<std::string> >& lines)
{
    bool inQuote(false);
    bool newLine(false);
    std::string field;
    lines.clear();
    std::vector<std::string> line;

    std::string::const_iterator aChar = csvSource.begin();
    while (aChar != csvSource.end())
    {
        switch (*aChar)
        {
        case '"':
            newLine = false;
            inQuote = !inQuote;
            break;

        case ',':
            newLine = false;
            if (inQuote == true)
            {
                field += *aChar;
            }
            else
            {
                line.push_back(field);
                field.clear();
            }
            break;

        case '\n':
        case '\r':
            if (inQuote == true)
            {
                field += *aChar;
            }
            else
            {
                if (newLine == false)
                {
                    line.push_back(field);
                    lines.push_back(line);
                    field.clear();
                    line.clear();
                    newLine = true;
                }
            }
            break;

        default:
            newLine = false;
            field.push_back(*aChar);
            break;
        }

        aChar++;
    }

    if (field.size())
        line.push_back(field);

    if (line.size())
        lines.push_back(line);
}

void PaintComposer::deserialize(std::istream &istream)
{
    std::string csv((std::istreambuf_iterator<char>(istream)),
                    std::istreambuf_iterator<char>());
    std::vector<std::vector<std::string> > data;
    ParseCSV(csv, data);

    const std::string type = data[0][0];

    clear();
    for (uint i = 1; i < data.size(); i++)
    {
        float thickness;
        glm::vec3 K, S;
        K.r = std::stof(data[i][0]);
        K.g = std::stof(data[i][1]);
        K.b = std::stof(data[i][2]);
        S.r = std::stof(data[i][3]);
        S.g = std::stof(data[i][4]);
        S.b = std::stof(data[i][5]);
        thickness = std::stof(data[i][6]);

        if (type == SERIALIZE_TYPE_KS)
        {
            addPaint(Paint(KMColor(K, S), thickness));
        }
        else if (type == SERIALIZE_TYPE_RR)
        {
            addPaint(Paint(KMColor::from_Rb_Rw(K, S), thickness));
        }

    }
}

size_t PaintComposer::size() const
{
    return m_palette.size();
}


} // namespace linde
