#include <QtGui>
#include <poppler-qt5.h>
#include <algorithm>

#include "pdfviewer.h"

PdfViewer::PdfViewer(QWidget *parent): QLabel(parent)
{
    m_current_page = 0;
    m_doc = nullptr;
    m_scale_factor = 1.0;
    m_link_hovered = false;
    m_previous_pos = -1;
    m_skip_next_event = false;
    setAlignment(Qt::AlignCenter);
    setMouseTracking(true);
}

PdfViewer::~PdfViewer(){ delete m_doc; }
Poppler::Document *PdfViewer::document() { return m_doc; }
qreal PdfViewer::scale() { return m_scale_factor; }

bool PdfViewer::setDocument(const QString &filePath)
{
    Poppler::Document *oldDocument = m_doc;

    m_doc = Poppler::Document::load(filePath);

    if (!m_doc)
        return false;

    delete oldDocument;
    m_doc->setRenderHint(Poppler::Document::Antialiasing);
    m_doc->setRenderHint(Poppler::Document::TextAntialiasing);
    m_current_page = 0;
    render_pdf_as_image();
    init_links();

    emit firstPage();
    if (m_doc->numPages() == 1)
        emit lastPage();

    return true;
}

void PdfViewer::render_pdf_as_image()
{
    if (m_doc != nullptr)
        m_image = m_doc->page(m_current_page)->renderToImage(m_scale_factor * physicalDpiX(), m_scale_factor * physicalDpiY());
}

void PdfViewer::show()
{
    setPixmap(QPixmap::fromImage(m_image));
}

void PdfViewer::setPage(int page)
{
    if (page < 0 || page >= m_doc->numPages()) return;

    m_current_page = page;
    render_pdf_as_image();
    show();

    emit pageChanged();

    if (m_current_page == 0)
        emit firstPage();

    if (m_current_page == m_doc->numPages() - 1)
        emit lastPage();
}

void PdfViewer::nextPage()
{
    setPage(m_current_page+1);
}

void PdfViewer::previousPage()
{
    setPage(m_current_page-1);
}

void PdfViewer::setScale(qreal scale)
{
    if (scale < 2 && scale > 0.1)
    {
        m_scale_factor = scale;
        render_pdf_as_image();
        show();
    }
}

void PdfViewer::zoomIn()
{
    setScale(m_scale_factor + 0.1);
    emit scaleChanged((int) (m_scale_factor*100));
}

void PdfViewer::zoomOut()
{
    setScale(m_scale_factor - 0.1);
    emit scaleChanged((int) (m_scale_factor*100));
}

void PdfViewer::zoomFit()
{
    setScale(0.1);
    float ih = m_image.height();
    float wh = height();
    float dpi = physicalDpiY();
    float invariant = (m_scale_factor*dpi*dpi)/ih;
    float scale = (wh*invariant)/(dpi*dpi);
    setScale(scale);
}

void PdfViewer::wheelEvent(QWheelEvent* event)
{
    if (event->modifiers() & Qt::ControlModifier)
    {
        if (event->delta() >0)
            setScale(m_scale_factor + 0.05);
        else
            setScale(m_scale_factor - 0.05);

        emit scaleChanged((int) (m_scale_factor*100));
    }
    else
        QLabel::wheelEvent(event);
}

void PdfViewer::mousePressEvent(QMouseEvent* event)
{
    QPointF pos = to_pdf_relative(event->pos());

    for (int i = 0 ; i < m_links.size() ; i++)
    {
        QRectF link_bbox = m_links[i].bbox();
        bool cursor_in_link_area = link_bbox.contains(pos);

        // If the cursor is in the link and there is no hover yet
        // draw a rectangle
        if (cursor_in_link_area)
        {
            int line = m_links[i].code_line();
            m_link_hovered = true;
            m_skip_next_event = true;
            emit linkClicked((int) (line));
            break;
        }
    }
}

void PdfViewer::mouseMoveEvent(QMouseEvent* event)
{
    QPointF pos = to_pdf_relative(event->pos());

    for (int i = 0 ; i < m_links.size() ; i++)
    {
        if (m_links[i].page() != m_current_page)
            continue;

        QRectF link_bbox = m_links[i].bbox();
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

        // If the cursor is not in a link and there is a hover
        // delete the rectangle
        if (m_link_hovered && !cursor_in_link_area)
        {
            m_link_hovered = false;
            render_pdf_as_image();
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

    int xmin = rect.left()*isize.width();
    int xmax = rect.right()*isize.width();

    int ymin = rect.top()*isize.height();
    int ymax = rect.bottom()*isize.height();

    return QRect(QPoint(xmin, ymax), QPoint(xmax, ymin));
}

void PdfViewer::highlight_link_from_lines(QVector<int> lines)
{
    if (m_skip_next_event)
    {
        m_skip_next_event = false;
        return;
    }

    if (m_link_hovered)
        render_pdf_as_image();

    for (int line : lines)
    {
        for (int i = 0 ; i < m_links.size() ; i++)
        {
           if (line == m_links[i].code_line())
           {
               if (m_links[i].page() != m_current_page)
                  setPage(m_links[i].page());

               QPainter qPainter(&m_image);
               QRectF bb = m_links[i].bbox();
               qPainter.setBrush(Qt::NoBrush);
               qPainter.setPen(QPen(Qt::darkCyan, 2));
               qPainter.fillRect(to_img_absolute(bb), QColor(255, 255,0, 100));
               qPainter.end();
               m_link_hovered = true;
           }
        }
    }

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
        int line = (m_links[i].code_line());
        if (line < offset) // C'est une note et pas un accord
            j++;

        i++;
    }

    render_pdf_as_image();

    int page = m_links[i-1].page();
    if (page != m_current_page)
        setPage(page);

    QRect bb = to_img_absolute(m_links[i-1].bbox());
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


void PdfViewer::init_links()
{
    QSet<QPair<int, int>> registry;
    m_links.clear();

    for (int i = 0 ; i < m_doc->numPages() ; i++)
    {
        QList<Poppler::Link*> all_links = m_doc->page(i)->links();

        // Check each link to determine whether or not we record it
        for (int j = 0 ; j < all_links.size() ; j++)
        {
            // Keep only non null area links
            if (all_links.at(j)->linkArea().width() > 0.0001)
            {
                Poppler::Link* link = all_links.at(j);
                ClickableNote lk(link, i);

                // We keep a single link per row/colum in the code
                QPair<int, int> p(lk.code_line(), lk.code_column());
                if (!registry.contains(p))
                {
                    registry.insert(p);

                    if (lk.code_line() != -1)
                        m_links.append(lk);
                }
            }
        }


        std::sort(m_links.begin(), m_links.end(), [](ClickableNote a, ClickableNote b)
        {
            if (a.page() != b.page()) return a.page() < b.page();
            if (a.code_line() != b.code_line()) return a.code_line() < b.code_line();
            return a.code_column() < b.code_column();
        });
    }
}

ClickableNote::ClickableNote() {}

ClickableNote::ClickableNote(Poppler::Link* link, int page)
{
    m_link = link;
    m_page = page;
    m_line = -1;
    m_column = -1;
    m_bbox = QRectF();

    QString url = static_cast<Poppler::LinkBrowse*>(link)->url();
    QStringList parsed_url = url.split(":");
    QRectF link_bbox = link->linkArea();

    if (parsed_url.size() > 2)
        m_line = parsed_url.at(2).toInt();

    if (parsed_url.size() > 3)
        m_column = parsed_url.at(3).toInt();

    // Buffer the bbox for better feeling
    QPointF p1 = link_bbox.topLeft();
    QPointF p2 = link_bbox.bottomRight();
    p1.setX(p1.x() - 0.002);
    p1.setY(p1.y() + 0.002);
    p2.setX(p2.x() + 0.002);
    p2.setY(p2.y() - 0.002);
    link_bbox.setTopLeft(p1);
    link_bbox.setBottomRight(p2);

    m_bbox = link_bbox;
}

int ClickableNote::page() { return m_page; }
int ClickableNote::code_line() { return m_line; }
int ClickableNote::code_column() { return m_column; }
QRectF ClickableNote::bbox() { return m_bbox; }


