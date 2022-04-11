#include <QtGui>
#include <poppler-qt5.h>
#include <algorithm>

#include "pdfviewer.h"

PdfViewer::PdfViewer(QWidget *parent): QLabel(parent)
{
    m_current_page = -1;
    m_doc = nullptr;
    m_scale_factor = 1.0;
    m_link_hovered = false;
    m_previous_pos = -1;
    m_skip_next_event = false;
    setAlignment(Qt::AlignCenter);
    setMouseTracking(true);
}

PdfViewer::~PdfViewer()
{
    delete m_doc;
}

Poppler::Document *PdfViewer::document()
{
    return m_doc;
}

qreal PdfViewer::scale() const
{
    return m_scale_factor;
}

void PdfViewer::show(int page)
{
    if (page != -1 && page != m_current_page + 1)
    {
        m_current_page = page - 1;
        emit pageChanged(page);
    }

    setPixmap(QPixmap::fromImage(m_image));
}

bool PdfViewer::setDocument(const QString &filePath)
{
    Poppler::Document *oldDocument = m_doc;

    m_doc = Poppler::Document::load(filePath);

    if (m_doc)
    {
        delete oldDocument;
        m_doc->setRenderHint(Poppler::Document::Antialiasing);
        m_doc->setRenderHint(Poppler::Document::TextAntialiasing);
        m_current_page = -1;
        setPage(1);

        // Keep only non null area links
        QList<Poppler::Link*> all_links = m_doc->page(m_current_page)->links();
        m_links.clear();
        for (int i = 0 ; i < all_links.size() ; i++)
        {
            if (all_links.at(i)->linkArea().width() > 0.0001)
            {
                m_links.append(all_links.at(i));
            }
        }

        std::sort(m_links.begin(), m_links.end(), [](Poppler::Link* a, Poppler::Link* b)
        {
            if (PdfViewer::get_line(a) != PdfViewer::get_line(b)) return PdfViewer::get_line(a) < PdfViewer::get_line(b);
            return PdfViewer::get_column(a) < PdfViewer::get_column(b);
        });
        clean_pdf();
    }

    return m_doc != 0;
}

void PdfViewer::setPage(int page)
{
    if (page != m_current_page + 1)
    {
        show(page);
    }
}

void PdfViewer::setScale(qreal scale)
{
    if (m_scale_factor != scale)
    {
        m_scale_factor = scale;
        clean_pdf();
        show();
    }
}

void PdfViewer::wheelEvent ( QWheelEvent * event )
{
    if (event->modifiers() & Qt::ControlModifier)
    {
        if (event->delta() >0)
            setScale(m_scale_factor + 0.025);
        else
            setScale(m_scale_factor - 0.025);

        emit scaleChanged((int) (m_scale_factor*100));
    }
    else
        QLabel::wheelEvent(event);
}

void PdfViewer::mousePressEvent(QMouseEvent * event )
{
    QPointF pos = to_pdf_relative(event->pos());

    for (int i = 0 ; i < m_links.size() ; i++)
    {
        QRectF link_bbox = bbox(m_links.at(i));
        bool cursor_in_link_area = link_bbox.contains(pos);

        // If the cursor is in the link and there is no hover yet
        // draw a rectangle
        if (cursor_in_link_area)
        {
            int line = get_line(m_links.at(i));
            m_link_hovered = true;
            m_skip_next_event = true;
            emit linkClicked((int) (line));
            break;
        }
    }
}

void PdfViewer::mouseMoveEvent(QMouseEvent * event )
{
    QPointF pos = to_pdf_relative(event->pos());

    for (int i = 0 ; i < m_links.size() ; i++)
    {
        QRectF link_bbox = bbox(m_links.at(i));
        bool cursor_in_link_area =link_bbox.contains(pos);

        // If the cursor is in the link and there is not hover yet
        // draw a rectangle
        if (!m_link_hovered && cursor_in_link_area)
        {
            QPainter qPainter(&m_image);
            qPainter.setBrush(Qt::NoBrush);
            qPainter.setPen(QPen(Qt::darkCyan, 2));
            qPainter.drawRoundRect(to_img_absolute(link_bbox), 50, 50);
            qPainter.end();
            show();
            m_link_hovered = true;
            break;
        }

        // If the cursor is no in a link and there is a hover
        // delete the rectangle
        if (m_link_hovered && !cursor_in_link_area)
        {
            m_link_hovered = false;
            clean_pdf();
            show();
        }
    }
}

QPointF PdfViewer::to_pdf_relative(QPoint pos)
{
    QSize isize = m_image.size();
    QSize wsize = this->size();
    int hmargin = (wsize.width() - isize.width())/2;
    int vmargin = (wsize.height() - isize.height())/2;
    int imgx = pos.x() - hmargin;
    int imgy = pos.y() - vmargin;
    float rx = (float)imgx/isize.width();
    float ry = (float)imgy/isize.height();

    if (imgx > 0 && imgx <= isize.width() && imgy > 0 && imgy <= isize.height())
        return QPointF(rx, ry);
    else
        return QPointF();
}

QRect PdfViewer::to_img_absolute(QRectF rect)
{
    QSize isize = m_image.size();
    QSize wsize = this->size();

    int xmin = rect.left()*isize.width();
    int xmax = rect.right()*isize.width();

    int ymin = rect.top()*isize.height();
    int ymax = rect.bottom()*isize.height();

    return QRect(QPoint(xmin, ymax), QPoint(xmax, ymin));
}

int PdfViewer::get_line(Poppler::Link* link)
{
    QString url = static_cast<Poppler::LinkBrowse*>(link)->url();
    QStringList parsed_url = url.split(":");
    int line = parsed_url.at(2).toInt();
    //int column_start = parsed_url.at(3).toInt();
    //int column_end = parsed_url.at(4).toInt();
    return line;
}

int PdfViewer::get_column(Poppler::Link* link)
{
    QString url = static_cast<Poppler::LinkBrowse*>(link)->url();
    QStringList parsed_url = url.split(":");
    //int line = parsed_url.at(2).toInt();
    int column_start = parsed_url.at(3).toInt();
    //int column_end = parsed_url.at(4).toInt();
    return column_start;
}

QRectF PdfViewer::bbox(Poppler::Link* link)
{
    QRectF link_bbox = link->linkArea();

    // Buffer the bbox for better feeling
    QPointF p1 = link_bbox.topLeft();
    QPointF p2 = link_bbox.bottomRight();
    p1.setX(p1.x() - 0.002);
    p1.setY(p1.y() + 0.002);
    p2.setX(p2.x() + 0.002);
    p2.setY(p2.y() - 0.002);
    link_bbox.setTopLeft(p1);
    link_bbox.setBottomRight(p2);

    return link_bbox;
}

void PdfViewer::highlight_link_from_lines(QVector<int> lines)
{
    if (m_skip_next_event)
    {
        m_skip_next_event = false;
        return;
    }

    if (m_link_hovered)
        clean_pdf();

    QPainter qPainter(&m_image);
    for (int line : lines)
    {
        for (int i = 0 ; i < m_links.size() ; i++)
        {
           if (line == get_line(m_links.at(i)))
           {
               QRectF bb = bbox(m_links.at(i));
               qPainter.setBrush(Qt::NoBrush);
               qPainter.setPen(QPen(Qt::darkCyan, 2));
               qPainter.fillRect(to_img_absolute(bb), QColor(255, 255,0, 100));
               m_link_hovered = true;
           }
        }
    }

    qPainter.end();
    show();
}

void PdfViewer::highlight_note(int pos, int offset)
{
    if (pos == m_previous_pos)
        return;

    m_previous_pos = pos;

    int j = -1;
    int i = 0;
    while (j != pos && i < m_links.size())
    {
        int line = get_line(m_links.at(i));
        if (line < offset) // C'est une note et pas un accord
            j++;

        i++;
    }

    clean_pdf();

    QRect bb = to_img_absolute(bbox(m_links.at(i-1)));
    QPoint p0 = bb.bottomLeft();
    QPoint p1 = bb.topLeft();
    QPoint p2 = bb.topRight();
    QPoint end((p1.x() + p2.x())/2, p2.y());
    QPoint start((p1.x() + p2.x())/2, p2.y() + 3 * (p2.y() - p0.y()));
    QLine arrow(start, end);

    QPainter qPainter(&m_image);
    qPainter.setPen(QPen(Qt::red, 3));
    qPainter.drawLine(arrow);
    qPainter.end();
    show();
}

void PdfViewer::clean_pdf()
{
    if (m_doc != nullptr)
        m_image = m_doc->page(m_current_page)->renderToImage(m_scale_factor * physicalDpiX(), m_scale_factor * physicalDpiY());
}



