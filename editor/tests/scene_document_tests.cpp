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
    void preservesCustomShapeAcrossSaveLoad();
    void preservesSpriteAssetPathAcrossSaveLoad();
    void rejectsInvalidObjectDimensions();
    void rejectsDuplicateObjectIds();
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
    player->width = 180.0;
    player->height = 72.5;
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
    QCOMPARE(restored->width, 180.0);
    QCOMPARE(restored->height, 72.5);
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

void SceneDocumentTests::rejectsInvalidObjectDimensions()
{
    QTemporaryDir temporaryDirectory;
    QVERIFY(temporaryDirectory.isValid());

    const QString path = temporaryDirectory.filePath(QStringLiteral("invalid-size.vscene"));
    QFile invalidScene(path);
    QVERIFY(invalidScene.open(QIODevice::WriteOnly));
    const QByteArray contents =
        R"({"format":"veyra-scene","formatVersion":1,"objects":[{"id":"object-1","name":"Bad","x":0,"y":0,"width":0,"height":40,"rotation":0,"scaleX":1,"scaleY":1,"visible":true}]})";
    QCOMPARE(invalidScene.write(contents), static_cast<qint64>(contents.size()));
    invalidScene.close();

    Veyra::SceneDocument document;
    const QString preservedId = document.objects().first().id;
    QString error;
    QVERIFY(!document.load(path, &error));
    QVERIFY(!error.isEmpty());
    QCOMPARE(document.objects().size(), 1);
    QCOMPARE(document.objects().first().id, preservedId);
}

void SceneDocumentTests::rejectsDuplicateObjectIds()
{
    QTemporaryDir temporaryDirectory;
    QVERIFY(temporaryDirectory.isValid());

    const QString path = temporaryDirectory.filePath(QStringLiteral("duplicate-ids.vscene"));
    QFile invalidScene(path);
    QVERIFY(invalidScene.open(QIODevice::WriteOnly));
    const QByteArray contents =
        R"({"format":"veyra-scene","formatVersion":1,"objects":[{"id":"same","name":"A","x":0,"y":0,"width":10,"height":10,"rotation":0,"scaleX":1,"scaleY":1},{"id":"same","name":"B","x":20,"y":0,"width":10,"height":10,"rotation":0,"scaleX":1,"scaleY":1}]})";
    QCOMPARE(invalidScene.write(contents), static_cast<qint64>(contents.size()));
    invalidScene.close();

    Veyra::SceneDocument document;
    QString error;
    QVERIFY(!document.load(path, &error));
    QVERIFY(!error.isEmpty());
    QCOMPARE(document.objects().size(), 1);
}

void SceneDocumentTests::preservesCustomShapeAcrossSaveLoad()
{
    QTemporaryDir temporaryDirectory;
    QVERIFY(temporaryDirectory.isValid());

    Veyra::SceneDocument source;
    Veyra::SceneObject& ship = source.addObject(QStringLiteral("Player Ship"));
    ship.shape = QStringLiteral("triangle");
    source.markModified();
    const QString path = temporaryDirectory.filePath(QStringLiteral("ship.vscene"));
    QString error;
    QVERIFY2(source.save(path, &error), qPrintable(error));

    Veyra::SceneDocument loaded;
    QVERIFY2(loaded.load(path, &error), qPrintable(error));
    const Veyra::SceneObject* restored = loaded.find(ship.id);
    QVERIFY(restored != nullptr);
    QCOMPARE(restored->shape, QStringLiteral("triangle"));
}

void SceneDocumentTests::preservesSpriteAssetPathAcrossSaveLoad()
{
    QTemporaryDir temporaryDirectory;
    QVERIFY(temporaryDirectory.isValid());

    Veyra::SceneDocument source;
    Veyra::SceneObject& sprite = source.addObject(QStringLiteral("Player Sprite"));
    sprite.shape = QStringLiteral("sprite");
    sprite.assetPath = QStringLiteral("Assets/player.png");
    source.markModified();

    const QString path = temporaryDirectory.filePath(QStringLiteral("sprite.vscene"));
    QString error;
    QVERIFY2(source.save(path, &error), qPrintable(error));

    Veyra::SceneDocument loaded;
    QVERIFY2(loaded.load(path, &error), qPrintable(error));
    const Veyra::SceneObject* restored = loaded.find(sprite.id);
    QVERIFY(restored != nullptr);
    QCOMPARE(restored->shape, QStringLiteral("sprite"));
    QCOMPARE(restored->assetPath, QStringLiteral("Assets/player.png"));
}

QTEST_GUILESS_MAIN(SceneDocumentTests)
#include "scene_document_tests.moc"
