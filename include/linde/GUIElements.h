#ifndef GUIELEMENTS_H
#define GUIELEMENTS_H

#include "lindeOpenGL.h"

namespace linde
{
class TextRenderer;
class GLWindow;

/**
    * @author Thomas Lindemeier
    * @date 25.06.2013
    *
    * University of Konstanz-
    * Department for Computergraphics
    */
class GUIElement
{
protected:
    glm::vec2	m_origin;
    GLWindow	*m_window;
    glm::vec2	m_size;
    std::string m_text;
    GLboolean	m_lock;
    glm::vec4	m_color;

    GLboolean inRange(GLfloat screenX, GLfloat screenY) const;

public:
    static const GLfloat TOL;
    static const GLfloat GUI_DISPLACEMENT_FACTOR;

    GUIElement(GLWindow * window, GLfloat posX, GLfloat posY, GLfloat w, GLfloat h);

    GLvoid setColor(const glm::vec4 & color);
    GLvoid setText(const std::string & text);

    virtual ~GUIElement();

    virtual GLboolean update(GLfloat screenX, GLfloat screenY) = 0;
    virtual GLboolean update(const glm::vec2 & screenPos);
    virtual GLboolean locked() const;
    virtual void lock(GLboolean lock);

    virtual void paint() const = 0;
};

class NeedButtonRelease{};

class Label : public GUIElement
{

public:
    Label(GLWindow * window, GLfloat posX, GLfloat posY, GLfloat w, GLfloat h);
    virtual ~Label();

    GLboolean update(GLfloat screenX, GLfloat screenY);

    void paint() const;

};

class CheckBox : public GUIElement, public NeedButtonRelease
{
    bool * m_checked;

public:
    CheckBox(GLWindow * window, GLfloat posX, GLfloat posY, GLfloat w, GLfloat h, bool * value);
    virtual ~CheckBox();

    GLboolean update(GLfloat screenX, GLfloat screenY);

    void paint() const;

    bool isChecked() const;
    void setChecked(bool *checked);
};


class Button : public GUIElement
{
    std::function<void()>       m_toCall;
public:
    Button(GLWindow * window, GLfloat posX, GLfloat posY, GLfloat w, GLfloat h, const std::function<void()> &  toCall);
    virtual ~Button();

    GLboolean update(GLfloat screenX, GLfloat screenY);

    void paint() const;

};

class DropDownBox : public GUIElement, public NeedButtonRelease
{
    int *                           m_selection;
    std::vector<std::string>        m_options;
    bool                            m_opened;
    std::function<void(int)>        m_callback;

public:
    DropDownBox(GLWindow * window, GLfloat posX, GLfloat posY, GLfloat w, GLfloat h, const std::vector<std::string>& options, int * selection, const std::function<void(int)> &callback);
    virtual ~DropDownBox();

    GLboolean update(GLfloat screenX, GLfloat screenY);

    void paint() const;

    const std::vector<std::string> & getOptions() const;
    void setOptions(const std::vector<std::string> &options);
    int getSelection() const;
    void setSelection(int *selection);
};

template <class T>
class Slider : public GUIElement
{
private:
    T			*m_value;
    T			m_minValue;
    T			m_maxValue;

    glm::vec2 m_relPosition;

public:
    Slider(GLWindow * window, GLfloat posX, GLfloat posY, GLfloat w, GLfloat h, const T & minValue, const T & maxValue, T * value);

    virtual ~Slider();
    GLboolean update(GLfloat screenX, GLfloat screenY);
    void paint() const;

    T getValue() const;
    void setValue(T *value);

    T getMinValue() const;
    void setMinValue(const T &minValue);

    T getMaxValue() const;
    void setMaxValue(const T &maxValue);
};

class ProgressBar
{
    friend class GLWindow;

    GLWindow *      m_window;
    float           m_progress;
    std::string     m_text;

    void            render() const;
public:
    ProgressBar(GLWindow * window);
    ~ProgressBar();

    void set(float p);
    void setText(const std::string &text);
    float get();
};

struct GUI_stuff
{
    std::vector<
    std::shared_ptr<
    GUIElement> >       m_elements;
    glm::vec4			m_overlayColor;
    GLboolean			m_mouseLeftPressed;
    GLboolean			m_show;
    glm::vec2			m_nextAvailablePosition;
    GLfloat				m_elementHeight;
    GLfloat				m_sliderWidth;

    GUI_stuff();
};



} // namespace linde

#endif // GUIELEMENTS_H

