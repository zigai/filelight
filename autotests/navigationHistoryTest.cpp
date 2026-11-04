// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
// SPDX-FileCopyrightText: 2026 zigai

#include "navigationHistory.h"
#include "Config.h"
#include "fileModel.h"
#include "fileTree.h"
#include "mainContext.h"
#include "scan.h"

#include <QDir>
#include <QFile>
#include <QSignalSpy>
#include <QStandardPaths>
#include <QTemporaryDir>
#include <QTest>

using Filelight::NavigationHistory;

class NavigationHistoryTest : public QObject
{
    Q_OBJECT

    static QUrl folder(const char *path)
    {
        return QUrl::fromLocalFile(QString::fromUtf8(path));
    }

    static bool scanFinished(const QUrl &expected)
    {
        return QTest::qWaitFor(
            [&] {
                return !Filelight::ScanManager::instance()->running()
                    && QDir::cleanPath(Filelight::FileModel::instance()->url().toLocalFile()) == QDir::cleanPath(expected.toLocalFile());
            },
            5000);
    }

private Q_SLOTS:
    void emptyHistory()
    {
        NavigationHistory history;
        QVERIFY(history.current().isEmpty());
        QVERIFY(!history.canGoBack());
        QVERIFY(!history.canGoForward());
        QVERIFY(!history.goBack());
        QVERIFY(!history.goForward());
        QVERIFY(!history.record({}));
        QVERIFY(!history.record(QUrl(QStringLiteral("relative/path"))));
    }

    void backForwardAndBranching()
    {
        NavigationHistory history;
        QVERIFY(history.record(folder("/a/")));
        QVERIFY(history.record(folder("/b/")));
        QVERIFY(history.record(folder("/c/")));
        QCOMPARE(history.back(), folder("/b/"));
        QVERIFY(history.goBack());
        QVERIFY(history.goBack());
        QCOMPARE(history.current(), folder("/a/"));
        QVERIFY(!history.canGoBack());
        QCOMPARE(history.forward(), folder("/b/"));
        QVERIFY(history.goForward());
        QVERIFY(history.record(folder("/new/")));
        QVERIFY(!history.canGoForward());
        QVERIFY(history.goBack());
        QCOMPARE(history.current(), folder("/b/"));
        QCOMPARE(history.forward(), folder("/new/"));
    }

    void duplicateRescanPreservesForward()
    {
        NavigationHistory history;
        history.record(folder("/a/"));
        history.record(folder("/b/"));
        history.goBack();
        QVERIFY(!history.record(folder("/a")));
        QVERIFY(!history.record(folder("/a/child/../")));
        QVERIFY(!history.record({}));
        QCOMPARE(history.forward(), folder("/b/"));
        QVERIFY(!history.canGoBack());
    }

    void initialBackUpCanBeReversed()
    {
        NavigationHistory history;
        history.record(folder("/a/b/"));
        QVERIFY(history.prependParent(folder("/a/")));
        QCOMPARE(history.current(), folder("/a/"));
        QVERIFY(history.prependParent(folder("/")));
        QCOMPARE(history.current(), folder("/"));
        QVERIFY(!history.prependParent(folder("/")));
        QVERIFY(history.goForward());
        QCOMPARE(history.current(), folder("/a/"));
        QVERIFY(history.goForward());
        QCOMPARE(history.current(), folder("/a/b/"));
        QVERIFY(!history.canGoForward());
    }

    void specialAndRemoteUrls()
    {
        NavigationHistory history;
        const auto local = QUrl::fromLocalFile(QString::fromUtf8("/tmp/café #1 100%/"));
        const QUrl remote(QStringLiteral("sftp://example.org/home/a%20folder/"));
        history.record(local);
        history.record(remote);
        QVERIFY(history.goBack());
        QCOMPARE(history.current(), local);
        QCOMPARE(history.forward(), remote);
    }

    void acceptedVisitsAndFailedReplay()
    {
        QStandardPaths::setTestModeEnabled(true);
        Config::instance()->read();
        qRegisterMetaType<std::shared_ptr<File>>("std::shared_ptr<File>");
        qRegisterMetaType<std::shared_ptr<Folder>>("std::shared_ptr<Folder>");
        QTemporaryDir temporary;
        QVERIFY(temporary.isValid());
        QDir root(temporary.path());
        QVERIFY(root.mkpath(QStringLiteral("a/nested")));
        QVERIFY(root.mkdir(QStringLiteral("b")));
        QFile sample(root.filePath(QStringLiteral("a/nested/sample.txt")));
        QVERIFY(sample.open(QIODevice::WriteOnly));
        sample.write("history test");
        sample.close();
        const auto parent = QUrl::fromLocalFile(temporary.path());
        const auto a = QUrl::fromLocalFile(root.filePath(QStringLiteral("a")));
        const auto b = QUrl::fromLocalFile(root.filePath(QStringLiteral("b")));
        std::unique_ptr<Filelight::MainContext> context(Filelight::MainContext::create(nullptr, nullptr));
        QSignalSpy errors(context.get(), &Filelight::MainContext::openUrlFailed);

        QVERIFY(context->openUrl(a));
        QVERIFY(scanFinished(a));
        QVERIFY(!context->forwardEnabled());
        QVERIFY(context->slotBack()); // Initially opened folder: Back falls back to Up.
        QVERIFY(scanFinished(parent));
        QVERIFY(context->forwardEnabled());
        QVERIFY(context->slotForward());
        QVERIFY(scanFinished(a));
        QVERIFY(!context->forwardEnabled());
        QVERIFY(context->slotBack());
        QVERIFY(scanFinished(parent));

        context->updateURL(b); // Same route used by chart/sidebar navigation.
        QVERIFY(context->openUrl(b));
        QVERIFY(scanFinished(b));
        QVERIFY(!context->forwardEnabled()); // New visit discarded the old A branch.
        QVERIFY(context->slotBack());
        QVERIFY(scanFinished(parent));
        QVERIFY(context->openUrl(parent)); // Rescan must preserve Forward.
        QVERIFY(scanFinished(parent));
        QVERIFY(context->forwardEnabled());

        QVERIFY(QDir(b.toLocalFile()).removeRecursively()); // Disposable fixture only.
        QVERIFY(!context->slotForward());
        QCOMPARE(errors.count(), 1);
        QVERIFY(scanFinished(parent));
        QVERIFY(context->forwardEnabled()); // Failed replay did not advance history.
        QVERIFY(root.mkdir(QStringLiteral("b")));
        QVERIFY(context->slotForward());
        QVERIFY(scanFinished(b));

        const auto missing = QUrl::fromLocalFile(root.filePath(QStringLiteral("missing")));
        context->updateURL(missing);
        QVERIFY(!context->openUrl(missing));
        QCOMPARE(QDir::cleanPath(context->url().toLocalFile()), QDir::cleanPath(b.toLocalFile()));
        QVERIFY(!context->forwardEnabled());
        context->slotUp(); // Up remains parent navigation, recorded as a normal visit.
        QVERIFY(scanFinished(parent));
        QVERIFY(context->slotBack());
        QVERIFY(scanFinished(b));
        QVERIFY(context->slotForward());
        QVERIFY(scanFinished(parent));
        QVERIFY(!context->slotForward());
    }
};

QTEST_MAIN(NavigationHistoryTest)

#include "navigationHistoryTest.moc"
