#include <QStylePainter>
#include <QStyleOption>
#include <QTransform>

#include "widget/wknobcomposed.h"

WKnobComposed::WKnobComposed(QWidget* pParent)
        : WWidget(pParent),
          m_dCurrentAngle(140.0),
          m_dMinAngle(-230.0),
          m_dMaxAngle(50.0),
          m_dKnobCenterXOffset(0),
          m_dKnobCenterYOffset(0) {
}

WKnobComposed::~WKnobComposed() {
}

void WKnobComposed::setup(QDomNode node, const SkinContext& context) {
    clear();

    // Set background pixmap if available
    if (context.hasNode(node, "BackPath")) {
        QDomElement backPathElement = context.selectElement(node, "BackPath");
        setPixmapBackground(context.getPixmapSource(backPathElement),
//                            context.selectScaleMode(backPathElement, Paintable::FIXED));
                            context.selectScaleMode(backPathElement, Paintable::STRETCH));
    }

    // Set knob pixmap if available
    if (context.hasNode(node, "Knob")) {
        QDomElement knobNode = context.selectElement(node, "Knob");
        setPixmapKnob(context.getPixmapSource(knobNode),
//                      context.selectScaleMode(knobNode, Paintable::FIXED));
                      context.selectScaleMode(knobNode, Paintable::STRETCH));
    }

    if (context.hasNode(node, "MinAngle")) {
        m_dMinAngle = context.selectDouble(node, "MinAngle");
    }

    if (context.hasNode(node, "MaxAngle")) {
        m_dMaxAngle = context.selectDouble(node, "MaxAngle");
    }

    if (context.hasNode(node, "KnobCenterXOffset")) {
        m_dKnobCenterXOffset = context.selectDouble(node, "KnobCenterXOffset");
    }

    if (context.hasNode(node, "KnobCenterYOffset")) {
        m_dKnobCenterYOffset = context.selectDouble(node, "KnobCenterYOffset");
    }
}

void WKnobComposed::clear() {
    m_pPixmapBack.clear();
    m_pKnob.clear();
}

void WKnobComposed::setPixmapBackground(PixmapSource source,
                                        Paintable::DrawMode mode) {
    m_pPixmapBack = WPixmapStore::getPaintable(source, mode);
    if (m_pPixmapBack.isNull() || m_pPixmapBack->isNull()) {
        qDebug() << metaObject()->className()
                 << "Error loading background pixmap:" << source.getPath();
    }
    m_contentsSize = m_pPixmapBack->size();
}

void WKnobComposed::setPixmapKnob(PixmapSource source,
                                  Paintable::DrawMode mode) {
    m_pKnob = WPixmapStore::getPaintable(source, mode);
    if (m_pKnob.isNull() || m_pKnob->isNull()) {
        qDebug() << metaObject()->className()
                 << "Error loading knob pixmap:" << source.getPath();
    }
}

void WKnobComposed::onConnectedControlChanged(double dParameter, double dValue) {
    Q_UNUSED(dValue);
    // dParameter is in the range [0, 1].
    double angle = m_dMinAngle + (m_dMaxAngle - m_dMinAngle) * dParameter;

    // TODO(rryan): What's a good epsilon? Should it be dependent on the min/max
    // angle range? Right now it's just 1/100th of a degree.
    if (fabs(angle - m_dCurrentAngle) > 0.01) {
        // paintEvent updates m_dCurrentAngle
        update();
    }
}

void WKnobComposed::paintEvent(QPaintEvent* e) {
    Q_UNUSED(e);
    QStylePainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.setRenderHint(QPainter::SmoothPixmapTransform);
    QRect contentsRect = getContentsRect();

    if (m_pPixmapBack) {
//        m_pPixmapBack->draw(contentsRect, &p, m_pPixmapBack->rect());
        m_pPixmapBack->draw(contentsRect, &p);
    }

    QTransform transform;
    if (!m_pKnob.isNull() && !m_pKnob->isNull()) {
        qreal tx = contentsRect.x() + m_dKnobCenterXOffset + contentsRect.width() / 2.0;
        qreal ty = contentsRect.y() + m_dKnobCenterYOffset + contentsRect.height() / 2.0;
        transform.translate(-tx, -ty);
        p.translate(tx, ty);

        // We update m_dCurrentAngle since onConnectedControlChanged uses it for
        // no-op detection.
        m_dCurrentAngle = m_dMinAngle + (m_dMaxAngle - m_dMinAngle) * getControlParameterDisplay();
        p.rotate(m_dCurrentAngle);

        // Need to convert from QRect to a QRectF to avoid losing precison.
//        m_pKnob->drawCentered(transform.mapRect(contentsRect), &p,
        m_pKnob->drawCentered(contentsRect, &p,
                              m_pKnob->rect());
    }
}

void WKnobComposed::mouseMoveEvent(QMouseEvent* e) {
    m_handler.mouseMoveEvent(this, e);
}

void WKnobComposed::mousePressEvent(QMouseEvent* e) {
    m_handler.mousePressEvent(this, e);
}

void WKnobComposed::mouseReleaseEvent(QMouseEvent* e) {
    m_handler.mouseReleaseEvent(this, e);
}

void WKnobComposed::wheelEvent(QWheelEvent* e) {
    m_handler.wheelEvent(this, e);
}
