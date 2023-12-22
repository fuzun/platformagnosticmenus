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
#include "platformagnosticaction.hpp"

#include <QAction>
#include <QQmlComponent>
#include <QQmlProperty>
#include <QQuickItem>
#include <QQmlEngine>
#include <QQmlFile>
#include <QIcon>

#include "platformagnosticactiongroup.hpp"
#include "platformagnosticmenu.hpp"

#define QQUICKCONTROLS2_ACTION_PATH "qrc:///util/ActionExt.qml"

PlatformAgnosticAction::PlatformAgnosticAction(QObject *parent)
    : QObject{parent}
{

}

template<>
PlatformAgnosticAction* PlatformAgnosticAction::fromAction(QAction *action)
{
    assert(action);

    PlatformAgnosticAction* const widgetsAction = new WidgetsAction;

    widgetsAction->setAction(action);

    connect(action, &QAction::toggled, widgetsAction, &PlatformAgnosticAction::toggled);
    connect(action, &QAction::triggered, widgetsAction, &PlatformAgnosticAction::triggered);

    action->setProperty("platformAgnosticAction", QVariant::fromValue(widgetsAction));

    return widgetsAction;
}

template<>
PlatformAgnosticAction* PlatformAgnosticAction::fromAction(PlatformAgnosticAction *action)
{
    assert(action);
    return action;
}

PlatformAgnosticAction *PlatformAgnosticAction::createAction(QObject *parent)
{
    if (!parent)
    {
        return new WidgetsAction;
    }
    else
    {
        if (const auto widgetsMenuParent = qobject_cast<WidgetsMenu*>(parent))
            return new WidgetsAction(widgetsMenuParent);
        else if (const auto quickControls2MenuParent = qobject_cast<QuickControls2Menu*>(parent))
            return new QuickControls2Action((*quickControls2MenuParent)(), quickControls2MenuParent);
        else if (const auto widgetsActionGroupParent = qobject_cast<WidgetsActionGroup*>(parent))
            return new WidgetsAction(widgetsActionGroupParent);
        else if (const auto quickControls2ActionGroupParent = qobject_cast<QuickControls2ActionGroup*>(parent))
            return new QuickControls2Action((*quickControls2ActionGroupParent)(), quickControls2ActionGroupParent);
        else if (const auto quickItemParent = qobject_cast<QQuickItem*>(parent))
            return new QuickControls2Action(quickItemParent, quickItemParent);
        else
            Q_UNREACHABLE();
    }
}

PlatformAgnosticAction *PlatformAgnosticAction::createAction(const QString& text, QObject *parent)
{
    PlatformAgnosticAction * const action = createAction(parent);
    action->setText(text);
    return action;
}

void PlatformAgnosticAction::setVisible(const bool _visible)
{
    if (isVisible() == _visible)
        return;

    if (_visible)
    {
        const auto text = m_text;
        m_text.clear();
        setText(text);
    }
    else
    {
        m_text = text();
        setText(QLatin1String(""));
    }
}

bool PlatformAgnosticAction::isVisible() const
{
    return m_text.isEmpty();
}

QString PlatformAgnosticAction::text() const
{
    assert(action());
    return action()->property("text").value<QString>();
}

void PlatformAgnosticAction::setText(const QString &text)
{
    assert(action());
    if (isVisible())
        action()->setProperty("text", text);
    else
        m_text = text;
}

void PlatformAgnosticAction::setEnabled(bool enabled)
{
    assert(action());
    action()->setProperty("enabled", enabled);
}

void PlatformAgnosticAction::setChecked(bool checked)
{
    assert(action());
    action()->setProperty("checked", checked);
}

void PlatformAgnosticAction::setCheckable(bool checkable)
{
    assert(action());
    action()->setProperty("checkable", checkable);
}

void PlatformAgnosticAction::setData(const QVariant &data)
{
    if (m_data != data)
        m_data = data;
}

QVariant PlatformAgnosticAction::data() const
{
    return m_data;
}

WidgetsAction::WidgetsAction(QObject *parent)
    : PlatformAgnosticAction{parent}
{
    const auto action = new QAction(parent);
    m_action = action;

    connect(action, &QAction::changed, action, [action]() {
        if (action->text().isEmpty())
            action->setVisible(false);
        else
            action->setVisible(true);
    });

    connect(action, &QAction::toggled, this, &PlatformAgnosticAction::toggled);
    connect(action, &QAction::triggered, this, &PlatformAgnosticAction::triggered);

    m_action->setProperty("platformAgnosticAction", QVariant::fromValue(this));
}

void WidgetsAction::setShortcut(const QKeySequence &shortcut)
{
    assert(m_action);
    m_action->setShortcut(shortcut);
}

void WidgetsAction::setActionGroup(PlatformAgnosticActionGroup *actionGroup)
{
    assert(actionGroup ? !!qobject_cast<WidgetsActionGroup*>(actionGroup) : true);
    assert(m_action);

    m_action->setActionGroup(static_cast<WidgetsActionGroup*>(actionGroup)->m_actionGroup);
}

void WidgetsAction::setIcon(const QString &iconSourceOrName, const bool isSource)
{
    assert(m_action);

    QIcon icon;
    if (isSource)
    {
        icon = QIcon(iconSourceOrName);
    }
    else
    {
        assert(QIcon::hasThemeIcon(iconSourceOrName));
        icon = QIcon::fromTheme(iconSourceOrName);
    }
    assert(!icon.isNull());

    m_action->setIcon(icon);
}

QObject *WidgetsAction::action() const
{
    return m_action.data();
}

void WidgetsAction::setAction(QObject *action)
{
    assert(action);
    assert(qobject_cast<QAction*>(action));

    m_action = static_cast<QAction*>(action);
}

QuickControls2Action::QuickControls2Action(QObject *quickParent, QObject *parent)
    : PlatformAgnosticAction{parent}
{
    assert(quickParent);

    const auto engine = qmlEngine(quickParent);
    assert(engine);

    m_actionComponent = new QQmlComponent(engine, QUrl(QStringLiteral(QQUICKCONTROLS2_ACTION_PATH)), this);

    m_action = m_actionComponent->create(qmlContext(quickParent));
    assert(m_action);
    assert(m_action->inherits("QQuickAction"));

    QQmlEngine::setObjectOwnership(m_action, QQmlEngine::CppOwnership);

    m_action->setParent(this);

    connect(m_action, SIGNAL(toggled(QObject*)), this, SLOT(onToggled(QObject*)));
    connect(m_action, SIGNAL(triggered(QObject*)), this, SLOT(onTriggered(QObject*)));

    m_action->setProperty("platformAgnosticAction", QVariant::fromValue(this));
}

QuickControls2Action::QuickControls2Action(QObject *parent)
    : QuickControls2Action{parent, parent}
{

}

void QuickControls2Action::setShortcut(const QKeySequence &shortcut)
{
    assert(m_action);

    const bool ret = QQmlProperty::write(m_action.data(),
                                         QStringLiteral("shortcut"),
                                         shortcut);
    assert(ret);
}

void QuickControls2Action::setActionGroup(PlatformAgnosticActionGroup *actionGroup)
{
    assert(actionGroup ? !!qobject_cast<QuickControls2ActionGroup*>(actionGroup) : true);
    assert(m_action);

    const bool ret = QQmlProperty::write(m_action.data(),
                                         QStringLiteral("ActionGroup.group"),
                                         QVariant::fromValue(actionGroup ? static_cast<QuickControls2ActionGroup*>(actionGroup)->m_actionGroup.data()
                                                                         : nullptr),
                                         qmlContext(m_action.data()));
    assert(ret);
}

void QuickControls2Action::setIcon(const QString &_iconSourceOrName, const bool isSource)
{
    assert(m_action);

    QString iconSourceOrName = _iconSourceOrName;

    const QLatin1String suffixSource{"source"};
    const QLatin1String suffixName{"name"};
    const QLatin1String *suffix;

    if (isSource)
    {
        const auto qrc = QLatin1String{"qrc"};
        if (!iconSourceOrName.startsWith(qrc))
            iconSourceOrName.prepend(qrc);

        suffix = &suffixSource;
    }
    else
    {
        suffix = &suffixName;
    }

    auto property = QQmlProperty(m_action.data(), "icon." + *suffix, qmlContext(m_action.data()));
    assert(property.isValid() && property.isWritable());
    const bool ret = property.write(iconSourceOrName);
    assert(ret);
}

QObject *QuickControls2Action::action() const
{
    return m_action.data();
}

void QuickControls2Action::setAction(QObject *action)
{
    assert(action);
    assert(action->inherits("QQuickAction"));

    m_action = action;
}

void QuickControls2Action::onTriggered(QObject *source)
{
    Q_UNUSED(source);
    emit triggered(m_action->property("checked").toBool());
}

void QuickControls2Action::onToggled(QObject *source)
{
    Q_UNUSED(source);
    emit toggled(m_action->property("toggled").toBool());
}
