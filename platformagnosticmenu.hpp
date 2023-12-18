/* MIT License
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
 */
#ifndef PLATFORMAGNOSTICMENU_HPP
#define PLATFORMAGNOSTICMENU_HPP

#include <QObject>
#include <QPointer>
#include <QList>
#include <QKeySequence>

#include "platformagnosticaction.hpp"

// Common denominator for QMenu and QQuickMenu
class PlatformAgnosticMenu : public QObject
{
    Q_OBJECT

    friend class PlatformAgnosticAction;
    friend class PlatformAgnosticActionGroup;

public:
    explicit PlatformAgnosticMenu(QObject *parent);
    virtual ~PlatformAgnosticMenu() = default;

    virtual void installEventFilter(QObject* object);
    virtual void removeEventFilter(QObject* object);

    template<class Menu>
    static PlatformAgnosticMenu* fromMenu(Menu menu);

    static PlatformAgnosticMenu* createMenu(QObject * parent = nullptr);
    static PlatformAgnosticMenu* createMenu(const QString& text, QObject * parent);

    virtual PlatformAgnosticMenu *addMenu(const QString &title);
    virtual void addMenu(PlatformAgnosticMenu *menu) = 0;

    virtual void setTitle(const QString& title);
    virtual void setEnabled(bool enabled);
    virtual void popup(const QPoint& pos) = 0;
    virtual void close() = 0;
    virtual void addSeparator() = 0;

    virtual void addAction(PlatformAgnosticAction *action) = 0;
    virtual PlatformAgnosticAction* addAction(const QString& text);
    virtual PlatformAgnosticAction* addAction(const QString& iconSource, const QString& text);
    virtual void removeAction(PlatformAgnosticAction *action) = 0;

    virtual QList<PlatformAgnosticAction*> actions() const = 0;

    virtual QSize sizeHint() const = 0;

    template<typename Functor>
    PlatformAgnosticAction* addAction(const QString& text, Functor func, const QKeySequence &shortcut = 0)
    {
        const auto action = addAction(text);
        action->setShortcut(shortcut);
        connect(action, &PlatformAgnosticAction::triggered, func);
        return action;
    }

    template<class Object, typename Functor>
    PlatformAgnosticAction* addAction(const QString& text, Object context, Functor func, const QKeySequence &shortcut = 0)
    {
        const auto action = addAction(text);
        action->setShortcut(shortcut);
        connect(action, &PlatformAgnosticAction::triggered, context, func);
        return action;
    }

    template<typename Functor>
    PlatformAgnosticAction* addAction(const QString& iconSource, const QString& text, Functor func, const QKeySequence &shortcut = 0)
    {
        const auto action = addAction(text, func, shortcut);
        action->setIcon(iconSource);
        return action;
    }

    template<class Object, typename Functor>
    PlatformAgnosticAction* addAction(const QString& iconSource, const QString& text, Object context, Functor func, const QKeySequence &shortcut = 0)
    {
        const auto action = addAction(text, context, func, shortcut);
        action->setIcon(iconSource);
        return action;
    }

    template<typename Functor>
    PlatformAgnosticAction* addAction(const QString& iconSource, const QString& text)
    {
        const auto action = addAction(text);
        action->setIcon(iconSource);
        return action;
    }

signals:
    void aboutToShow();
    void aboutToHide();

protected:
    QObject* operator()() const { return menu(); };
    virtual QObject* menu() const = 0;
    virtual void setMenu(QObject* menu) = 0;
};

class WidgetsMenu : public PlatformAgnosticMenu
{
    Q_OBJECT

public:
    explicit WidgetsMenu(WidgetsMenu* parent = nullptr);
    virtual ~WidgetsMenu();

    void addAction(PlatformAgnosticAction *action) override;
    void removeAction(PlatformAgnosticAction *action) override;

    void addMenu(PlatformAgnosticMenu *menu) override;

    void popup(const QPoint& pos) override;

    QList<PlatformAgnosticAction*> actions() const override;

    void close() override;

    void addSeparator() override;

    QSize sizeHint() const override;

protected:
    QObject* menu() const override;
    void setMenu(QObject * menu) override;

private:
    QPointer<class QMenu> m_menu;
};

class QuickControls2Menu : public PlatformAgnosticMenu
{
    Q_OBJECT

public:
    QuickControls2Menu(QObject* quickParent, class QObject* parent = nullptr);
    virtual ~QuickControls2Menu() = default;

    explicit QuickControls2Menu(QuickControls2Menu *parent);
    explicit QuickControls2Menu(class QQuickWindow* parent);
    explicit QuickControls2Menu(class QQuickItem* parent);

    virtual void installEventFilter(QObject* object) override;
    virtual void removeEventFilter(QObject* object) override;

    void addAction(PlatformAgnosticAction *action) override;
    void removeAction(PlatformAgnosticAction *action) override;

    void addMenu(PlatformAgnosticMenu *menu) override;

    void popup(const QPoint& pos) override;

    QList<PlatformAgnosticAction*> actions() const override;

    void close() override;

    void addSeparator() override;

    QSize sizeHint() const override;

protected:
    QObject* menu() const override;
    void setMenu(QObject * menu) override;

private:
    QPointer<QObject> m_menu;

    QPointer<class QQmlComponent> m_menuComponent;
    QPointer<class QQmlComponent> m_menuSeparatorComponent;
};



#endif // PLATFORMAGNOSTICMENU_HPP
