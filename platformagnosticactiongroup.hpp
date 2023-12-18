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
#ifndef PLATFORMAGNOSTICACTIONGROUP_HPP
#define PLATFORMAGNOSTICACTIONGROUP_HPP

#include <QObject>
#include <QPointer>

class PlatformAgnosticAction;

// Common denominator for QActionGroup and QQuickActionGroup
class PlatformAgnosticActionGroup : public QObject
{
    Q_OBJECT

    friend class PlatformAgnosticAction;

public:
    explicit PlatformAgnosticActionGroup(QObject * parent = nullptr);
    virtual ~PlatformAgnosticActionGroup() = default;

    template<class ActionGroup>
    static PlatformAgnosticActionGroup* fromActionGroup(ActionGroup actionGroup);

    static PlatformAgnosticActionGroup* createActionGroup(QObject *parent = nullptr);

    virtual void addAction(PlatformAgnosticAction *action) = 0;
    virtual void removeAction(PlatformAgnosticAction *action) = 0;

public slots:
    virtual void setEnabled(bool enabled);
    virtual void setExclusive(bool exclusive);

signals:
    void triggered(QObject *action);

protected:
    QObject* operator()() const { return actionGroup(); };
    virtual QObject *actionGroup() const = 0;
    virtual void setActionGroup(QObject* actionGroup) = 0;
};

class WidgetsActionGroup : public PlatformAgnosticActionGroup
{
    Q_OBJECT

    friend class WidgetsAction;

public:
    explicit WidgetsActionGroup(QObject* parent = nullptr);
    virtual ~WidgetsActionGroup() = default;

    void addAction(PlatformAgnosticAction *action) override;
    void removeAction(PlatformAgnosticAction *action) override;

protected:
    QObject* actionGroup() const override;
    void setActionGroup(QObject* actionGroup) override;

private:
    QPointer<class QActionGroup> m_actionGroup;
};

class QuickControls2ActionGroup : public PlatformAgnosticActionGroup
{
    Q_OBJECT

    friend class QuickControls2Action;

public:
    QuickControls2ActionGroup(QObject* quickParent, class QObject* parent = nullptr);

    explicit QuickControls2ActionGroup(QObject* parent);
    virtual ~QuickControls2ActionGroup() = default;

    void addAction(PlatformAgnosticAction *action) override;
    void removeAction(PlatformAgnosticAction *action) override;

protected:
    QObject* actionGroup() const override;
    void setActionGroup(QObject* actionGroup) override;

private:
    QPointer<QObject> m_actionGroup;
    QPointer<class QQmlComponent> m_actionGroupComponent;
};

#endif // PLATFORMAGNOSTICACTIONGROUP_HPP
