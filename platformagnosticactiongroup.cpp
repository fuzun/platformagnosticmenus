/*
 * MIT License
 *
 * Copyright (c) 2023 Fatih Uzunoglu <fuzun54@outlook.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */
#include "platformagnosticactiongroup.hpp"

#include <QActionGroup>
#include <QQuickItem>
#include <QQmlComponent>
#include <QQmlEngine>

#include "platformagnosticmenu.hpp"

#define QQUICKCONTROLS2_ACTION_GROUP_PATH "qrc:///util/ActionGroupExt.qml"

PlatformAgnosticActionGroup::PlatformAgnosticActionGroup(QObject *parent)
    : QObject{parent}
{

}

template<>
PlatformAgnosticActionGroup* PlatformAgnosticActionGroup::fromActionGroup(QActionGroup * actionGroup)
{
    assert(actionGroup);

    PlatformAgnosticActionGroup* const widgetsActionGroup = new WidgetsActionGroup;

    widgetsActionGroup->setActionGroup(actionGroup);

    connect(actionGroup, &QActionGroup::triggered, widgetsActionGroup, &PlatformAgnosticActionGroup::triggered);
    actionGroup->setProperty("platformAgnosticActionGroup", QVariant::fromValue(widgetsActionGroup));

    return widgetsActionGroup;
}

template<>
PlatformAgnosticActionGroup* PlatformAgnosticActionGroup::fromActionGroup(PlatformAgnosticActionGroup * actionGroup)
{
    assert(actionGroup);
    return actionGroup;
}

PlatformAgnosticActionGroup* PlatformAgnosticActionGroup::createActionGroup(QObject *parent)
{
    if (!parent)
    {
        return new WidgetsActionGroup;
    }
    else
    {
        if (const auto widgetsMenuParent = qobject_cast<WidgetsMenu*>(parent))
            return new WidgetsActionGroup(widgetsMenuParent);
        else if (const auto quickControls2MenuParent = qobject_cast<QuickControls2Menu*>(parent))
            return new QuickControls2ActionGroup((*quickControls2MenuParent)(), quickControls2MenuParent);
        else if (const auto quickItemParent = qobject_cast<QQuickItem*>(parent))
            return new QuickControls2ActionGroup(quickItemParent, quickItemParent);
        else
            Q_UNREACHABLE();
    }
}

void PlatformAgnosticActionGroup::setEnabled(const bool enabled)
{
    assert(actionGroup());
    actionGroup()->setProperty("enabled", enabled);
}

void PlatformAgnosticActionGroup::setExclusive(const bool exclusive)
{
    assert(actionGroup());
    actionGroup()->setProperty("exclusive", exclusive);
}

WidgetsActionGroup::WidgetsActionGroup(QObject *parent)
    : PlatformAgnosticActionGroup{parent}
{
    m_actionGroup = new QActionGroup(parent);
    connect(m_actionGroup.data(), &QActionGroup::triggered, this, &PlatformAgnosticActionGroup::triggered);

    m_actionGroup->setProperty("agnosticActionGroup", QVariant::fromValue(this));
}

void WidgetsActionGroup::addAction(PlatformAgnosticAction *action)
{
    assert(qobject_cast<WidgetsAction*>(action));
    assert(m_actionGroup);

    m_actionGroup.data()->addAction(static_cast<WidgetsAction*>(action)->m_action.data());
}

void WidgetsActionGroup::removeAction(PlatformAgnosticAction *action)
{
    assert(qobject_cast<WidgetsAction*>(action));
    assert(m_actionGroup);

    m_actionGroup.data()->removeAction(static_cast<WidgetsAction*>(action)->m_action.data());
}

QObject* WidgetsActionGroup::actionGroup() const
{
    return m_actionGroup.data();
}

void WidgetsActionGroup::setActionGroup(QObject *actionGroup)
{
    assert(actionGroup);
    assert(qobject_cast<QActionGroup*>(actionGroup));

    m_actionGroup = static_cast<QActionGroup*>(actionGroup);
}

QObject* QuickControls2ActionGroup::actionGroup() const
{
    return m_actionGroup.data();
}

void QuickControls2ActionGroup::setActionGroup(QObject *actionGroup)
{
    assert(actionGroup);
    assert(actionGroup->inherits("QQuickActionGroup"));

    m_actionGroup = actionGroup;
}

QuickControls2ActionGroup::QuickControls2ActionGroup(QObject *quickParent, QObject *parent)
    : PlatformAgnosticActionGroup{parent}
{
    assert(quickParent);

    const auto engine = qmlEngine(quickParent);
    assert(engine);

    m_actionGroupComponent = new QQmlComponent(engine, QUrl(QStringLiteral(QQUICKCONTROLS2_ACTION_GROUP_PATH)), this);

    m_actionGroup = m_actionGroupComponent->create(qmlContext(quickParent));
    assert(m_actionGroup);
    assert(m_actionGroup->inherits("QQuickActionGroup"));

    QQmlEngine::setObjectOwnership(m_actionGroup, QQmlEngine::CppOwnership);

    connect(m_actionGroup, SIGNAL(_triggered(QObject*)), this, SIGNAL(triggered(QObject*)));

    m_actionGroup->setParent(this);

    m_actionGroup->setProperty("platformAgnosticActionGroup", QVariant::fromValue(this));
}

QuickControls2ActionGroup::QuickControls2ActionGroup(QObject *parent)
    : QuickControls2ActionGroup{parent, parent}
{

}

void QuickControls2ActionGroup::addAction(PlatformAgnosticAction *action)
{
    assert(qobject_cast<QuickControls2Action*>(action));
    assert(m_actionGroup);

    QMetaObject::invokeMethod(m_actionGroup.data(),
                              "_addAction",
                              Q_ARG(QVariant,
                                    QVariant::fromValue(static_cast<QuickControls2Action*>(action)->m_action.data())));
}

void QuickControls2ActionGroup::removeAction(PlatformAgnosticAction *action)
{
    assert(qobject_cast<QuickControls2Action*>(action));
    assert(m_actionGroup);

    QMetaObject::invokeMethod(m_actionGroup.data(),
                              "_removeAction",
                              Q_ARG(QVariant,
                                    QVariant::fromValue(static_cast<QuickControls2Action*>(action)->m_action.data())));
}
