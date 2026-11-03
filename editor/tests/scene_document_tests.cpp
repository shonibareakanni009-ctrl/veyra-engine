/**
 * @file scene_document_tests.cpp
 * @brief Automated tests for scene editing and .vscene persistence.
 */
#include "scene_document.h"

#include <QFile>
#include <QTemporaryDir>
#include <QtTest>

class SceneDocumentTests final : public QObject
{
    Q_OBJECT

private slots:
    void savesAndLoadsScene();
    void rejectsUnsupportedVersionWithoutChangingOpenScene();
    void removingObjectMarksDocumentModified();
};

void SceneDocumentTests::savesAndLoadsScene()
{
    QTemporaryDir temporaryDirectory;
    QVERIFY(temporaryDirectory.isValid());

    Veyra::SceneDocument source;
    const QString playerId = source.addObject(QStringLiteral("Player")).id;
    Veyra::SceneObject* player = source.find(playerId);
    QVERIFY(player != nullptr);
    player->x = 123.5;
    player->y = -42.25;
    player->rotation = 37.0;
    player->scaleX = 1.5;
    player->scaleY = 0.75;
    source.markModified();

    const QString path = temporaryDirectory.filePath(QStringLiteral("round-trip.vscene"));
    QString error;
    QVERIFY2(source.save(path, &error), qPrintable(error));
    QVERIFY(!source.isModified());

    Veyra::SceneDocument loaded;
    QVERIFY2(loaded.load(path, &error), qPrintable(error));
    QVERIFY(!loaded.isModified());
    QCOMPARE(loaded.objects().size(), source.objects().size());

    const Veyra::SceneObject* restored = loaded.find(playerId);
    QVERIFY(restored != nullptr);
    QCOMPARE(restored->name, QStringLiteral("Player"));
    QCOMPARE(restored->x, 123.5);
    QCOMPARE(restored->y, -42.25);
    QCOMPARE(restored->rotation, 37.0);
    QCOMPARE(restored->scaleX, 1.5);
    QCOMPARE(restored->scaleY, 0.75);
}

void SceneDocumentTests::rejectsUnsupportedVersionWithoutChangingOpenScene()
{
    QTemporaryDir temporaryDirectory;
    QVERIFY(temporaryDirectory.isValid());

    const QString path = temporaryDirectory.filePath(QStringLiteral("future-version.vscene"));
    QFile invalidScene(path);
    QVERIFY(invalidScene.open(QIODevice::WriteOnly));
    const QByteArray contents =
        R"({"format":"veyra-scene","formatVersion":999,"objects":[]})";
    QCOMPARE(invalidScene.write(contents), static_cast<qint64>(contents.size()));
    invalidScene.close();

    Veyra::SceneDocument openScene;
    const QString preservedId = openScene.objects().first().id;
    const QString preservedName = openScene.objects().first().name;
    openScene.addObject(QStringLiteral("Unsaved Object"));
    const int originalCount = openScene.objects().size();
    QString error;

    QVERIFY(!openScene.load(path, &error));
    QVERIFY(!error.isEmpty());
    QCOMPARE(openScene.objects().size(), originalCount);
    QCOMPARE(openScene.objects().first().id, preservedId);
    QCOMPARE(openScene.objects().first().name, preservedName);
    QVERIFY(openScene.isModified());
    QVERIFY(openScene.filePath().isEmpty());
}

void SceneDocumentTests::removingObjectMarksDocumentModified()
{
    Veyra::SceneDocument document;
    const QString objectId = document.objects().first().id;
    QVERIFY(!document.isModified());
    QVERIFY(document.removeObject(objectId));
    QVERIFY(document.isModified());
    QVERIFY(document.objects().isEmpty());
    QVERIFY(!document.removeObject(objectId));
}

QTEST_GUILESS_MAIN(SceneDocumentTests)
#include "scene_document_tests.moc"
