// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
// SPDX-FileCopyrightText: 2026 zigai

#include "navigationInput.h"

#include <QKeyEvent>
#include <QMouseEvent>
#include <QSignalSpy>
#include <QTest>

class NavigationInputTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void keyboardBack()
    {
        QWindow window;
        NavigationInput input;
        input.setWindow(&window);
        input.setProperty("enabled", true);
        QSignalSpy back(&input, &NavigationInput::backRequested);

        QKeyEvent shortcut(QEvent::ShortcutOverride, Qt::Key_Left, Qt::AltModifier);
        shortcut.ignore();
        QCoreApplication::sendEvent(&window, &shortcut);
        QVERIFY(shortcut.isAccepted());
        QCOMPARE(back.count(), 0);
        QKeyEvent press(QEvent::KeyPress, Qt::Key_Left, Qt::AltModifier);
        QCoreApplication::sendEvent(&window, &press);
        QCOMPARE(back.count(), 1);
        QKeyEvent repeat(QEvent::KeyPress, Qt::Key_Left, Qt::AltModifier, QString(), true, 2);
        QCoreApplication::sendEvent(&window, &repeat);
        QCOMPARE(back.count(), 1);
        QKeyEvent release(QEvent::KeyRelease, Qt::Key_Left, Qt::AltModifier);
        QCoreApplication::sendEvent(&window, &release);
        QCOMPARE(back.count(), 1);
        QKeyEvent native(QEvent::KeyPress, Qt::Key_Back, Qt::NoModifier);
        QCoreApplication::sendEvent(&window, &native);
        QCOMPARE(back.count(), 2);
    }

    void nativeMouseBack()
    {
        QWindow window;
        NavigationInput input;
        input.setWindow(&window);
        input.setProperty("enabled", true);
        QSignalSpy back(&input, &NavigationInput::backRequested);
        const QPointF position(10, 10);
        QMouseEvent press(QEvent::MouseButtonPress, position, position, Qt::BackButton, Qt::BackButton, Qt::NoModifier);
        QCoreApplication::sendEvent(&window, &press);
        QCOMPARE(back.count(), 0);
        QMouseEvent release(QEvent::MouseButtonRelease, position, position, Qt::BackButton, Qt::NoButton, Qt::NoModifier);
        QCoreApplication::sendEvent(&window, &release);
        QCOMPARE(back.count(), 1);

        QMouseEvent unrelated(QEvent::MouseButtonRelease, position, position, Qt::LeftButton, Qt::NoButton, Qt::NoModifier);
        QCoreApplication::sendEvent(&window, &unrelated);
        QCOMPARE(back.count(), 1);
    }

    void disabledOutsideMap()
    {
        QWindow window;
        NavigationInput input;
        input.setWindow(&window);
        QSignalSpy back(&input, &NavigationInput::backRequested);
        QKeyEvent key(QEvent::KeyPress, Qt::Key_Back, Qt::NoModifier);
        QCoreApplication::sendEvent(&window, &key);
        QCOMPARE(back.count(), 0);
        input.setProperty("enabled", true);
        QKeyEvent unrelated(QEvent::KeyPress, Qt::Key_C, Qt::ControlModifier);
        QCoreApplication::sendEvent(&window, &unrelated);
        QCOMPARE(back.count(), 0);
    }

    void replacingWindowRemovesFilter()
    {
        QWindow first;
        QWindow second;
        NavigationInput input;
        input.setWindow(&first);
        input.setProperty("enabled", true);
        input.setWindow(&second);
        QSignalSpy back(&input, &NavigationInput::backRequested);
        QKeyEvent oldKey(QEvent::KeyPress, Qt::Key_Back, Qt::NoModifier);
        QCoreApplication::sendEvent(&first, &oldKey);
        QCOMPARE(back.count(), 0);
        QKeyEvent newKey(QEvent::KeyPress, Qt::Key_Back, Qt::NoModifier);
        QCoreApplication::sendEvent(&second, &newKey);
        QCOMPARE(back.count(), 1);
        input.setWindow(nullptr);
        QCOMPARE(input.window(), nullptr);
    }
};

QTEST_MAIN(NavigationInputTest)

#include "navigationInputTest.moc"
