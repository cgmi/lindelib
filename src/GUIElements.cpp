#include "linde/GUIElements.h"
#include "linde/GLWindow.h"

namespace linde
{

GLboolean GUIElement::inRange(GLfloat screenX, GLfloat screenY) const
{
    return screenX + TOL >= m_origin[0] && screenY + TOL >= m_origin[1] &&
            screenX - TOL <= (m_origin[0] + m_size.x) && screenY - TOL <= (m_origin[1] + m_size.y);
}

const GLfloat GUIElement::TOL = 3.f;
const GLfloat GUIElement::GUI_DISPLACEMENT_FACTOR = 1.7f;

GUIElement::GUIElement(GLWindow * window, GLfloat posX, GLfloat posY, GLfloat w, GLfloat h) :
    m_origin(posX, posY),
    m_window(window),
    m_size(w, h),
    m_text("GUI element"),
    m_lock(GL_FALSE),
    m_color(1.f, 1.f, 1.f, 1.f)
{

}

GLvoid GUIElement::setColor(const glm::vec4 & color)
{
    m_color = color;
}

GLvoid GUIElement::setText(const std::string & text)
{
    m_text = text;
}



GUIElement::~GUIElement()
{

}

GLboolean GUIElement::update(const glm::vec2 & screenPos)
{
    return update(screenPos[0], screenPos[1]);
}

GLboolean GUIElement::locked() const
{
    return m_lock;
}

void GUIElement::lock(GLboolean lock)
{
    m_lock = lock;
}



Label::Label(GLWindow * window, GLfloat posX, GLfloat posY, GLfloat w, GLfloat h) :
    GUIElement(window, posX, posY, w, h)
{

}

Label::~Label(){}

GLboolean Label::update(GLfloat screenX, GLfloat screenY)
{
    return false;
}

void Label::paint() const
{
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);

    glLineWidth(1);

    m_window->renderText(m_text, glm::vec2(m_origin[0], m_origin[1] + 0.75f*m_size.y), m_color);

    glPopClientAttrib();
    glPopAttrib();
}





bool CheckBox::isChecked() const
{
    return *m_checked;
}

void CheckBox::setChecked(bool *checked)
{
    m_checked = checked;
}

CheckBox::CheckBox(GLWindow * window, GLfloat posX, GLfloat posY, GLfloat w, GLfloat h, bool * value) :
    GUIElement(window, posX, posY, w, h),
    m_checked(value)
{

}

CheckBox::~CheckBox(){}

GLboolean CheckBox::update(GLfloat screenX, GLfloat screenY)
{
    // click is out of range
    if (!inRange(screenX, screenY)) return false;

    *m_checked = !(*m_checked);

    return true;
}

void CheckBox::paint() const
{
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);

    glLineWidth(1);

    m_window->renderText(m_text, glm::vec2(m_origin[0] + m_size.x + 4, m_origin[1] + 0.75f*m_size.y), m_color);

    enable2D();

    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4fv(&m_color[0]);
    glBegin(GL_LINE_LOOP);
    glVertex2f(m_origin[0], m_origin[1]);
    glVertex2f(m_origin[0] + m_size.x, m_origin[1]);
    glVertex2f(m_origin[0] + m_size.x, m_origin[1] + m_size.y);
    glVertex2f(m_origin[0], m_origin[1] + m_size.y);
    glEnd();
    if (*m_checked)
    {
        glPointSize(m_size.x - 4);
        glBegin(GL_POINTS);
        glVertex2f(m_origin[0] + m_size.x / 2, m_origin[1] + m_size.y / 2);
        glEnd();
    }
    glDisable(GL_BLEND);

    disable2D();

    glPopClientAttrib();
    glPopAttrib();
}

Button::Button(GLWindow * window, GLfloat posX, GLfloat posY, GLfloat w, GLfloat h, const std::function<void()> &  callback) :
    GUIElement(window, posX, posY, w, h),
    m_toCall(callback)
{

}


Button::~Button() {}

GLboolean Button::update(GLfloat screenX, GLfloat screenY)
{
    // click is out of range
    if (!inRange(screenX, screenY)) return false;

    m_toCall();

    return true;
}

void Button::paint() const
{
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);

    glLineWidth(1);


    m_window->renderText(m_text, glm::vec2(m_origin[0] + m_size.x + 4, m_origin[1] + 0.75f*m_size.y), m_color);

    enable2D();

    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glm::vec2 cursorPos = m_window->getCursorPos();
    if (m_window->getMouseButtonState(GLFW_MOUSE_BUTTON_1) == GLFW_PRESS && inRange(cursorPos.x, cursorPos.y))
    {
        glColor4f(1.f - m_color[0], 1.f - m_color[1], 1.f - m_color[2], m_color[3]);
    }
    else
    {
        glColor4fv(&m_color[0]);
    }
    glBegin(GL_QUADS);
    glVertex2f(m_origin[0], m_origin[1]);
    glVertex2f(m_origin[0] + m_size.x, m_origin[1]);
    glVertex2f(m_origin[0] + m_size.x, m_origin[1] + m_size.y);
    glVertex2f(m_origin[0], m_origin[1] + m_size.y);
    glEnd();

    glDisable(GL_BLEND);

    disable2D();

    glPopClientAttrib();
    glPopAttrib();
}

const std::vector<std::string> &DropDownBox::getOptions() const
{
    return m_options;
}

void DropDownBox::setOptions(const std::vector<std::string> &options)
{
    m_options = options;
}

int DropDownBox::getSelection() const
{
    return *m_selection;
}

void DropDownBox::setSelection(int *selection)
{
    m_selection = selection;
}

DropDownBox::DropDownBox(GLWindow * window,
                         GLfloat posX, GLfloat posY, GLfloat w, GLfloat h,
                         const std::vector<std::string>& options,
                         int * selection, const std::function<void(int)> &callback) :
    GUIElement(window, posX, posY, w, h),
    m_selection(selection),
    m_options(options),
    m_opened(false),
    m_callback(callback)
{

}

DropDownBox::~DropDownBox(){}

GLboolean DropDownBox::update(GLfloat screenX, GLfloat screenY)
{
    // click is out of range
    if (!m_opened && !inRange(screenX, screenY)) return false;

    if (!m_opened)
    {
        if (inRange(screenX, screenY))
        {
            m_opened = true;
            m_lock = true;
            return true;
        }
        else
            return false;
    }
    else
    {
        // just deactivating the box
        if (inRange(screenX, screenY))
        {
            m_opened = false;
            m_lock = false;
            return true;
        }

        // if it is in open range
        float height = m_options.size() * m_size.y;
        if (screenX + TOL >= m_origin[0] && screenY + TOL >= m_origin[1] + m_size.y &&
                screenX - TOL <= (m_origin[0] + m_size.x) && screenY - TOL <= (m_origin[1] + height + m_size.y))
        {
            m_lock = true;
            // check which option was clicked
            int index = 0;
            for (float y = m_origin.y + m_size.y; y <= m_origin.y + height /*+ m_size.y*/; y += m_size.y, index++)
            {
                if (screenY > y && screenY < y + m_size.y)
                {
                    if (m_selection)
                    {
                        *m_selection = index;
                    }
                    if (m_callback)
                    {
                        m_callback(index);
                    }
                    m_opened = false;
                    m_lock = GL_FALSE;
                    return true;
                }
            }

            m_opened = false;
            return true;
        }
        else
        {
            return false;
        }
    }


    return true;
}

void DropDownBox::paint() const
{
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);

    glLineWidth(1);

    // draw text
    m_window->renderText(m_text, glm::vec2(m_origin[0] + m_size.x + 4, m_origin[1] + 0.75f*m_size.y), m_color);

    glDisable(GL_TEXTURE_2D);

    // draw slider
    enable2D();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColor4fv(&m_color[0]);

    glBegin(GL_LINE_LOOP);
    glVertex2f(m_origin[0], m_origin[1]);
    glVertex2f(m_origin[0] + m_size.x, m_origin[1]);
    glVertex2f(m_origin[0] + m_size.x, m_origin[1] + m_size.y);
    glVertex2f(m_origin[0], m_origin[1] + m_size.y);
    glEnd();


    if (m_opened)
    {
        const float h = (m_options.size()+1) * m_size.y;

        glBegin(GL_QUADS);
        glVertex2f(m_origin[0], m_origin[1] + m_size.y);
        glVertex2f(m_origin[0] + m_size.x, m_origin[1] + m_size.y);
        glVertex2f(m_origin[0] + m_size.x, m_origin[1] + h);
        glVertex2f(m_origin[0], m_origin[1] + h);
        glEnd();

        glColor4f(1.f - m_color[0], 1.f - m_color[1], 1.f - m_color[2], m_color[3]);
        glBegin(GL_LINES);
        for (size_t i = 2; i < m_options.size() + 1; i++)
        {
            glVertex2f(m_origin[0], m_origin[1] + i*m_size.y);
            glVertex2f(m_origin[0] + m_size.x, m_origin[1] + i*m_size.y);
        }
        glEnd();
    }
    glDisable(GL_BLEND);

    m_window->renderText(m_options[*m_selection], glm::vec2(m_origin[0] + 4.f /*+ (m_size.x + 4.f) / 2.f*/, m_origin[1] + 0.75f*m_size.y), glm::vec4(m_color.r, m_color.g, m_color.b, m_color.a));

    glColor4fv(&m_color[0]);
    if (m_opened)
    {
        for (size_t i = 0; i < m_options.size(); i++)
        {
            m_window->renderText(m_options[i], glm::vec2(m_origin[0] + 4.f /*(m_size.x + 4.f) / 2.f*/, ((i+1)*m_size.y) + (m_origin[1] + 0.75f*m_size.y)), glm::vec4(1.f - m_color.r, 1.f - m_color.g, 1.f - m_color.b, m_color.a));
        }
    }


    disable2D();

    glPopAttrib();
    glPopClientAttrib();
}



GUI_stuff::GUI_stuff() :
    m_elements(),
    m_overlayColor(1.f, 1.f, 1.f, 1.f),
    m_mouseLeftPressed(GL_FALSE),
    m_show(GL_FALSE),
    m_nextAvailablePosition(10.f, 10.f),
    m_elementHeight(14.f),
    m_sliderWidth(200.f)
{}



template <typename T>
T Slider<T>::getValue() const
{
    return *m_value;
}

template <typename T>
void Slider<T>::setValue(T *value)
{
    m_value = value;
}

template <typename T>
T Slider<T>::getMinValue() const
{
    return m_minValue;
}

template <typename T>
void Slider<T>::setMinValue(const T &minValue)
{
    m_minValue = minValue;
}

template <typename T>
T Slider<T>::getMaxValue() const
{
    return m_maxValue;
}

template <typename T>
void Slider<T>::setMaxValue(const T &maxValue)
{
    m_maxValue = maxValue;
}

template <typename T>
Slider<T>::Slider(GLWindow * window, GLfloat posX, GLfloat posY, GLfloat w, GLfloat h, const T & minValue, const T & maxValue, T * value) :
    GUIElement(window, posX, posY, w, h),
    m_value(value),
    m_minValue(minValue),
    m_maxValue(maxValue)
{
    *m_value = glm::clamp(*m_value, m_minValue, m_maxValue);

    GLfloat al = (*m_value - m_minValue) / (GLfloat)m_maxValue;

    m_relPosition[0] = al * m_size.x;
    m_relPosition[1] = m_origin[1];
}

template <typename T>
Slider<T>::~Slider(){}

template <typename T>
GLboolean Slider<T>::update(GLfloat screenX, GLfloat screenY)
{
    // click is out of range
    if (!inRange(screenX, screenY)) return false;

    // relative position
    m_relPosition[0] = glm::clamp(screenX - m_origin[0], 0.f, m_size.x);

    *m_value = (T)linde::mapRange<double>(m_relPosition[0], 0.f, m_size.x, m_minValue, m_maxValue);

    return true;
}

template <typename T>
void Slider<T>::paint() const
{
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);

    glLineWidth(1);


    GLfloat offset = 0.f;
    GLfloat tx = m_origin[0] + offset;
    GLfloat ty = m_origin[1] + offset;
    GLfloat lx = m_origin[0] + m_relPosition[0] - offset;
    GLfloat ly = m_origin[1] + m_size.y - offset;

    // draw text
    m_window->renderText(m_text, glm::vec2(m_origin[0] + m_size.x + 4, m_origin[1] + 0.75f*m_size.y), m_color);

    glDisable(GL_TEXTURE_2D);

    // draw slider
    enable2D();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4fv(&m_color[0]);
    glRectf(tx, ty, lx, ly);
    glBegin(GL_LINE_LOOP);
    glVertex2f(m_origin[0], m_origin[1]);
    glVertex2f(m_origin[0] + m_size.x, m_origin[1]);
    glVertex2f(m_origin[0] + m_size.x, m_origin[1] + m_size.y);
    glVertex2f(m_origin[0], m_origin[1] + m_size.y);
    glEnd();

    std::stringstream stream;
    stream << *m_value;
    m_window->renderText(stream.str(), glm::vec2(m_origin[0] + (m_size.x + 4.f) / 2.f, m_origin[1] + 0.75f*m_size.y), glm::vec4(1.f - m_color.r, 1.f - m_color.g, 1.f - m_color.b, m_color.a));

    disable2D();

    glPopAttrib();
    glPopClientAttrib();
}

void ProgressBar::render() const
{
    if (m_progress >= 1.f || m_progress < 0.f ) return;

    const float w = m_window->getWidth();
    const float h = m_window->getHeight();

    const glm::vec2 posText(0.0f * w, 0.975f * h);
    const float p = /*h -*/ (0.95f * h);

    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);

    enable2D();
    glColor4f(0.7f, 0.8f, 0.7f, 1.f);
    // progress
    glBegin(GL_QUADS);
    glVertex2f(0, h);
    glVertex2f(m_progress * w, h);
    glVertex2f(m_progress * w, p);
    glVertex2f(0, p);
    glEnd();
    disable2D();

    m_window->renderText(m_text, posText, glm::vec4(0.3f, 0.2f, 0.3f, 1.f));

    glPopClientAttrib();
    glPopAttrib();
}

ProgressBar::ProgressBar(GLWindow *window) :
    m_window(window),
    m_progress(-1),
    m_text("Progress Bar")
{

}

ProgressBar::~ProgressBar()
{

}

void ProgressBar::set(float p)
{
    m_progress = p;
    m_window->renderOnce(false);
}

void ProgressBar::setText(const std::string &text)
{
    m_text = text;
}

float ProgressBar::get()
{
    return m_progress;
}


} // namespace linde
