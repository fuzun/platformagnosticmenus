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
#ifndef PLATFORMAGNOSTICACTION_HPP
#define PLATFORMAGNOSTICACTION_HPP

#include <QObject>
#include <QPointer>

class PlatformAgnosticActionGroup;

// Common denominator for QAction and QQuickAction
class PlatformAgnosticAction : public QObject
{
    Q_OBJECT

    friend class PlatformAgnosticActionGroup;
    friend class PlatformAgnosticMenu;

public:
    explicit PlatformAgnosticAction(QObject *parent);
    virtual ~PlatformAgnosticAction() = default;

    template<class Action>
    static PlatformAgnosticAction* fromAction(Action action);

    static PlatformAgnosticAction* createAction(QObject * parent = nullptr);
    static PlatformAgnosticAction* createAction(const QString& text, QObject * parent = nullptr);

    virtual void setText(const QString &text);
    virtual void setCheckable(bool checkable);
    virtual void setShortcut(const QKeySequence &shortcut);
    virtual void setActionGroup(PlatformAgnosticActionGroup * actionGroup) = 0;
    virtual void setIcon(const QString& iconSource) = 0;

public slots:
    virtual void setEnabled(bool enabled);
    virtual void setChecked(bool checked);

signals:
    void toggled(bool);
    void triggered(bool);

protected:
    QObject* operator()() const { return action(); };
    virtual QObject* action() const = 0;
    virtual void setAction(QObject* action) = 0;
};

class WidgetsAction : public PlatformAgnosticAction
{
    Q_OBJECT

    friend class WidgetsActionGroup;
    friend class WidgetsMenu;

public:
    WidgetsAction(QObject *parent = nullptr);
    virtual ~WidgetsAction() = default;

    void setActionGroup(PlatformAgnosticActionGroup* actionGroup) override;
    void setIcon(const QString& iconSource) override;

protected:
    QObject* action() const override;
    void setAction(QObject* action) override;

private:
    QPointer<class QAction> m_action;
};

class QuickControls2Action : public PlatformAgnosticAction
{
    Q_OBJECT

    friend class QuickControls2ActionGroup;
    friend class QuickControls2Menu;

public:
    QuickControls2Action(QObject* quickParent, class QObject* parent = nullptr);

    explicit QuickControls2Action(QObject *parent);
    virtual ~QuickControls2Action() = default;

    void setActionGroup(PlatformAgnosticActionGroup* actionGroup) override;
    void setIcon(const QString& iconSource) override;

protected:
    QObject* action() const override;
    void setAction(QObject* action) override;

private:
    QPointer<QObject> m_action;
    QPointer<class QQmlComponent> m_actionComponent;

private slots:
    void onTriggered(QObject* source);
    void onToggled(QObject* source);
};

#endif // PLATFORMAGNOSTICACTION_HPP
