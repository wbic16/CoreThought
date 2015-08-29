//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// (c) 2015 Will Bickford
///
/// Knowledge Text File Format (.kxt)
///
/// A Knowledge Text File is able to produce the intermediate state of a text file at any given point in time during the
/// recorded session history. The extension was formed by replacing the first 't' with a 'k'. Apparently it wasn't common either.
///
/// This format draws upon ideas collected from the material listed below and pushes the boundary on what fundamental text should be.
///
/// * https://engineering.linkedin.com/distributed-systems/log-what-every-software-engineer-should-know-about-real-time-datas-unifying
/// * Twitter
/// * Bitcoin and Blockchains in General
/// * Node.js Streams
/// * Git and GitHub
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <QCoreApplication>
#include <QDebug>
#include <QString>

#include <cstdint>
#include <memory>
#include <utility>

enum class EditMode
{
    Insert,
    Overwrite
};

////////////////////////////////////////////////////////////////////////
/// @class Editor
////////////////////////////////////////////////////////////////////////
class Editor
{
public:
    Editor(uint64_t initial, uint32_t rangeLength, EditMode initialMode)
        : position(initial), length(rangeLength), mode(initialMode)
    {
    }

    uint64_t position; // within the document
    uint32_t length;   // length of the edit range in the document
    EditMode mode;
};

////////////////////////////////////////////////////////////////////////
/// @class BaseFrame
////////////////////////////////////////////////////////////////////////
class BaseFrame
{
public:
    BaseFrame(const QString& initial)
        : content(initial)
    {
    }

    QString content;
};

////////////////////////////////////////////////////////////////////////
/// @class Frame
////////////////////////////////////////////////////////////////////////
class Frame : public BaseFrame
{
public:
    Frame(const QString& content, uint16_t interval)
        : BaseFrame(content), deltaMs(interval)
    {
    }

    uint16_t deltaMs;
};

////////////////////////////////////////////////////////////////////////
/// @class TemporalFrame
////////////////////////////////////////////////////////////////////////
class TemporalFrame : public BaseFrame
{
public:
    TemporalFrame(const QString& content, uint64_t initial)
        : BaseFrame(content),
          timestamp(initial)
    {
    }

    uint64_t timestamp;
};

////////////////////////////////////////////////////////////////////////
/// @class CursorFrame
////////////////////////////////////////////////////////////////////////
class CursorFrame : public TemporalFrame
{
public:
    CursorFrame()
        : TemporalFrame(QString(), 0),
          next(0, 0, EditMode::Insert)
    {
    }

    Editor next;
};

////////////////////////////////////////////////////////////////////////
/// @class SnapshotFrame
////////////////////////////////////////////////////////////////////////
class SnapshotFrame : public TemporalFrame
{
public:
    SnapshotFrame(const QString& content, uint64_t initial)
        : TemporalFrame(content, initial)
    {
    }
};

////////////////////////////////////////////////////////////////////////
/// @class Document
////////////////////////////////////////////////////////////////////////
class Document : public std::vector<std::shared_ptr<BaseFrame>>
{
public:
    Document() = default;

    ////////////////////////////////////////////////////////////////////
    /// @fn append
    ////////////////////////////////////////////////////////////////////
    template <class ... Args>
    void append(Args... args)
    {
        emplace_back(std::shared_ptr<BaseFrame>(args...));
    }

    ////////////////////////////////////////////////////////////////////
    /// @fn render
    ////////////////////////////////////////////////////////////////////
    QString render()
    {
        QString output;
        for (auto frame : *this)
        {
            if (frame)
            {
                output += frame->content;
            }
        }
        return output;
    }
};

////////////////////////////////////////////////////////////////////////
/// @fn generateExampleDocument
////////////////////////////////////////////////////////////////////////
Document generateExampleDocument()
{
    /// Conceptually, our document is composed of a series of frames.
    ///
    /// Within each frame, we can record changes to the current document in a context-driven way.
    /// Let's break it down.

    Document example;
    example.append(new SnapshotFrame("", 0));
    example.append(new CursorFrame());
    example.append(new Frame("Hello", 100));
    example.append(new Frame(" ", 100));
    example.append(new Frame("W", 120));
    example.append(new Frame("o", 118));
    example.append(new Frame("r", 125));
    example.append(new Frame("l", 132));
    example.append(new Frame("d", 150));
    example.append(new Frame("!", 150));

    return example;
}

////////////////////////////////////////////////////////////////////////
/// \brief main
/// \param argc
/// \param argv
/// \return
////////////////////////////////////////////////////////////////////////
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    qDebug() << "Core Thought Example";
    auto example = generateExampleDocument();
    auto output = example.render();
    qDebug() << output;

    return a.exec();
}
