/***************************************************************************
                          wwidget.cpp  -  description
                             -------------------
    begin                : Wed Jun 18 2003
    copyright            : (C) 2003 by Tue & Ken Haste Andersen
    email                : haste@diku.dk
***************************************************************************/

/***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/

#include <QtDebug>
#include <QTouchEvent>
#include <QStyle>
#include <QStyleOption>

#include "widget/wwidget.h"
#include "controlobjectslave.h"
#include "util/assert.h"

WWidget::WWidget(QWidget* parent, Qt::WindowFlags flags)
        : QFrame(parent, flags),
          WBaseWidget(this),
          m_activeTouchButton(Qt::NoButton) {
    m_pTouchShift = new ControlObjectSlave("[Controls]", "touch_shift");
    setAttribute(Qt::WA_StaticContents);
    setAttribute(Qt::WA_AcceptTouchEvents);
    setFocusPolicy(Qt::ClickFocus);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

WWidget::~WWidget() {
    delete m_pTouchShift;
}

bool WWidget::touchIsRightButton() {
    return (m_pTouchShift->get() != 0.0);
}

bool WWidget::event(QEvent* e) {
    if (e->type() == QEvent::ToolTip) {
        updateTooltip();
    } else if (isEnabled()) {
        switch(e->type()) {
        case QEvent::TouchBegin:
        case QEvent::TouchUpdate:
        case QEvent::TouchEnd:
        {
            QTouchEvent* touchEvent = static_cast<QTouchEvent*>(e);
            if (touchEvent->deviceType() !=  QTouchEvent::TouchScreen) {
                break;
            }

            // fake a mouse event!
            QEvent::Type eventType = QEvent::None;
            switch (touchEvent->type()) {
            case QEvent::TouchBegin:
                eventType = QEvent::MouseButtonPress;
                if (touchIsRightButton()) {
                    // touch is right click
                    m_activeTouchButton = Qt::RightButton;
                } else {
                    m_activeTouchButton = Qt::LeftButton;
                }
                break;
            case QEvent::TouchUpdate:
                eventType = QEvent::MouseMove;
                break;
            case QEvent::TouchEnd:
                eventType = QEvent::MouseButtonRelease;
                break;
            default:
                DEBUG_ASSERT(false);
                break;
            }

            const QTouchEvent::TouchPoint &touchPoint =
                    touchEvent->touchPoints().first();
            QMouseEvent mouseEvent(eventType,
                    touchPoint.pos().toPoint(),
                    touchPoint.screenPos().toPoint(),
                    m_activeTouchButton, // Button that causes the event
                    Qt::NoButton, // Not used, so no need to fake a proper value.
                    touchEvent->modifiers());

            return QWidget::event(&mouseEvent);
        }
        default:
            break;
        }
    }

    return QWidget::event(e);
}

//QSize WWidget::sizeHint() const {
QSize WWidget::minimumSizeHint() const {
    QSize widgetSize = QFrame::sizeHint();

    if (!m_contentsSize.isNull()) {
        // CT_Slider is the simplest sizeFromContents : the given size
        // with the box size if it has borders/box or geometry.
        // The reference doesn't provide any good explanation for
        // sizeFromContents so you would read QCommonStyle and QStyleSheetStyle
        // sources to understand what Qt does.
        QStyleOption option;
        option.initFrom(this);
        widgetSize = style()->sizeFromContents(QStyle::CT_Slider, &option,
                                               m_contentsSize, this);
    }

    return widgetSize;
}

QRect WWidget::getContentsRect() const {

    QStyleOption option;
    option.initFrom(this);
    QRect contentsRect = style()->subElementRect(QStyle::SE_FrameContents,
                                                 &option, this);
    if (contentsRect.isNull()) {
        if (m_contentsSize.isEmpty()){
            contentsRect = rect();
        } else {
            contentsRect = QRect(QPoint(0,0), m_contentsSize);
        }
    }

    return contentsRect;
}
