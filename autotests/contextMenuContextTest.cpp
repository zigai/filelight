// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
// SPDX-FileCopyrightText: 2026 zigai

#include "contextMenuContext.h"

#include <QClipboard>
#include <QGuiApplication>
#include <QMimeData>
#include <QTest>

class ContextMenuContextTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void copyClipboard_data()
    {
        QTest::addColumn<QUrl>("url");
        QTest::addColumn<QString>("text");

        QTest::newRow("file") << QUrl::fromLocalFile(QStringLiteral("/tmp/file.txt")) << QStringLiteral("/tmp/file.txt");
        QTest::newRow("folder") << QUrl::fromLocalFile(QStringLiteral("/tmp/folder/")) << QStringLiteral("/tmp/folder/");
        QTest::newRow("special-characters") << QUrl::fromLocalFile(QString::fromUtf8("/tmp/café #1 100%.txt")) << QString::fromUtf8("/tmp/café #1 100%.txt");
        const QUrl remote(QStringLiteral("sftp://example.org/home/user/a%20file.txt"));
        QTest::newRow("remote") << remote << remote.toDisplayString();
    }

    void copyClipboard()
    {
        QFETCH(QUrl, url);
        QFETCH(QString, text);

        Filelight::ContextMenuContext context;
        context.copyClipboard(url);

        const auto *mime = QGuiApplication::clipboard()->mimeData();
        QVERIFY(mime);
        QVERIFY(mime->formats().contains(QStringLiteral("text/plain")));
        QVERIFY(mime->hasText());
        QCOMPARE(mime->text(), text);
        QVERIFY(mime->hasUrls());
        QCOMPARE(mime->urls(), QList<QUrl>{url});
    }
};

QTEST_MAIN(ContextMenuContextTest)

#include "contextMenuContextTest.moc"
