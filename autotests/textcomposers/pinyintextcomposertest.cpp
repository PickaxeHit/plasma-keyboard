/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "pinyin/pinyintextcomposer.h"
#include "textcomposertestutils.h"

#include <QStandardPaths>
#include <QTest>

using namespace Qt::StringLiterals;

class PinyinTextComposerTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase()
    {
        QStandardPaths::setTestModeEnabled(true);
    }

    void testBasic()
    {
        auto *backend = new TextComposerTestBackend;
        InputEngine engine(backend);
        engine.setTextComposer(new PinyinTextComposer);

        // Type pinyin
        QVERIFY(sendText(engine, u"nihao"));

        // Select 你好 candidate
        QCOMPARE(engine.preeditText(), u"nihao"_s);
        int candidateIndex = engine.candidates().indexOf(u"你好"_s);
        QVERIFY(candidateIndex >= 0);
        QVERIFY(engine.selectCandidate(candidateIndex));
        QCOMPARE(backend->commitRequests, QStringList{u"你好"_s});
        QVERIFY(engine.preeditText().isEmpty());
        QVERIFY(engine.candidates().isEmpty());
    }

    void testBackspace()
    {
        auto *backend = new TextComposerTestBackend;
        InputEngine engine(backend);
        engine.setTextComposer(new PinyinTextComposer);

        QVERIFY(sendText(engine, u"ni"));

        QVERIFY(engine.sendTextComposerKey(Qt::Key_Backspace, QString()));
        QCOMPARE(engine.preeditText(), u"n"_s);

        QVERIFY(engine.sendTextComposerKey(Qt::Key_Escape, QString()));
        QVERIFY(engine.preeditText().isEmpty());
        QVERIFY(engine.candidates().isEmpty());
    }

    void testPartialSelection()
    {
        auto *backend = new TextComposerTestBackend;
        InputEngine engine(backend);
        engine.setTextComposer(new PinyinTextComposer);

        // Matching 你 (ni) for "nihao" should leave "hao" in the preedit
        QVERIFY(sendText(engine, u"nihao"));
        QVERIFY(engine.selectCandidate(engine.candidates().indexOf(u"你"_s)));
        QCOMPARE(backend->commitRequests, QStringList{u"你"_s});
        QCOMPARE(engine.preeditText(), u"hao"_s);

        QVERIFY(engine.selectCandidate(engine.candidates().indexOf(u"好"_s)));
        QCOMPARE(backend->commitRequests, (QStringList{u"你"_s, u"好"_s}));
        QVERIFY(engine.preeditText().isEmpty());
    }

    void testSwitchInputMode()
    {
        auto *backend = new TextComposerTestBackend;
        InputEngine engine(backend);
        auto *composer = new PinyinTextComposer;
        engine.setTextComposer(composer);

        QVERIFY(sendText(engine, u"nihao"));
        composer->setInputMode(u"latin"_s);
        QCOMPARE(backend->commitRequests, QStringList{u"你好"_s});
        QVERIFY(engine.preeditText().isEmpty());
        QVERIFY(engine.candidates().isEmpty());
    }

    void testLatinMode()
    {
        auto *backend = new TextComposerTestBackend;
        InputEngine engine(backend);
        auto *composer = new PinyinTextComposer;
        composer->setInputMode(u"latin"_s);
        engine.setTextComposer(composer);

        QVERIFY(sendText(engine, u"a"));
        engine.setShiftActive(true);
        QVERIFY(sendText(engine, u"a"));
        engine.setCapsLockActive(true);
        QVERIFY(sendText(engine, u"a"));
        QCOMPARE(backend->commitRequests, (QStringList{u"a"_s, u"A"_s, u"A"_s}));
        QVERIFY(engine.preeditText().isEmpty());
        QVERIFY(engine.candidates().isEmpty());
    }

    void testDoublePinyin_data()
    {
        QTest::addColumn<QString>("scheme");
        QTest::addColumn<QString>("input");

        // "nihao" in each scheme (only the key for the final "ao" differs)
        QTest::newRow("ziranma") << u"zrm"_s << u"nihk"_s;
        QTest::newRow("microsoft") << u"ms"_s << u"nihk"_s;
        QTest::newRow("ziguang") << u"ziguang"_s << u"nihq"_s;
        QTest::newRow("smart-abc") << u"abc"_s << u"nihk"_s;
        QTest::newRow("pinyin-plus-plus") << u"pyjj"_s << u"nihd"_s;
        QTest::newRow("xiaohe") << u"xhe"_s << u"nihc"_s;
    }

    void testDoublePinyin()
    {
        QFETCH(QString, scheme);
        QFETCH(QString, input);

        auto *backend = new TextComposerTestBackend;
        InputEngine engine(backend);
        auto *composer = new PinyinTextComposer;
        composer->setPinyinScheme(scheme);
        QCOMPARE(composer->pinyinScheme(), scheme);
        QVERIFY(composer->doublePinyin());
        engine.setTextComposer(composer);

        QVERIFY(sendText(engine, input));
        QCOMPARE(engine.preeditText(), input);
        int candidateIndex = engine.candidates().indexOf(u"你好"_s);
        QVERIFY(candidateIndex >= 0);
        QVERIFY(engine.selectCandidate(candidateIndex));
        QCOMPARE(backend->commitRequests, QStringList{u"你好"_s});
        QVERIFY(engine.preeditText().isEmpty());
        QVERIFY(engine.candidates().isEmpty());
    }

    void testDoublePinyinPartialSelection()
    {
        auto *backend = new TextComposerTestBackend;
        InputEngine engine(backend);
        auto *composer = new PinyinTextComposer;
        composer->setPinyinScheme(u"zrm"_s);
        engine.setTextComposer(composer);

        // Matching 你 (two keys, "ni") for "nihk" should leave "hk" in the preedit
        QVERIFY(sendText(engine, u"nihk"));
        QVERIFY(engine.selectCandidate(engine.candidates().indexOf(u"你"_s)));
        QCOMPARE(backend->commitRequests, QStringList{u"你"_s});
        QCOMPARE(engine.preeditText(), u"hk"_s);

        QVERIFY(engine.selectCandidate(engine.candidates().indexOf(u"好"_s)));
        QCOMPARE(backend->commitRequests, (QStringList{u"你"_s, u"好"_s}));
        QVERIFY(engine.preeditText().isEmpty());
    }

    void testDoublePinyinSemicolon_data()
    {
        QTest::addColumn<QString>("scheme");

        // The final "ing" is ';' in these schemes
        QTest::newRow("microsoft") << u"ms"_s;
        QTest::newRow("ziguang") << u"ziguang"_s;
    }

    void testDoublePinyinSemicolon()
    {
        QFETCH(QString, scheme);

        auto *backend = new TextComposerTestBackend;
        InputEngine engine(backend);
        auto *composer = new PinyinTextComposer;
        composer->setPinyinScheme(scheme);
        QVERIFY(composer->semicolonKeyRequired());
        engine.setTextComposer(composer);

        // "d;" is "ding"
        QVERIFY(sendText(engine, u"d;"));
        QCOMPARE(engine.preeditText(), u"d;"_s);
        QVERIFY(engine.candidates().indexOf(u"定"_s) >= 0);
    }

    void testSemicolonNotRequiredInOtherSchemes()
    {
        PinyinTextComposer composer;
        QVERIFY(!composer.semicolonKeyRequired());
        for (const QString &scheme : {u"full"_s, u"zrm"_s, u"abc"_s, u"pyjj"_s, u"xhe"_s}) {
            composer.setPinyinScheme(scheme);
            QCOMPARE(composer.pinyinScheme(), scheme);
            QVERIFY2(!composer.semicolonKeyRequired(), qPrintable(scheme));
        }
    }

    void testUnknownSchemeIgnored()
    {
        PinyinTextComposer composer;
        QCOMPARE(composer.pinyinScheme(), u"full"_s);
        QVERIFY(!composer.doublePinyin());

        composer.setPinyinScheme(u"zrm"_s);
        composer.setPinyinScheme(u"not-a-scheme"_s);
        QCOMPARE(composer.pinyinScheme(), u"zrm"_s);

        composer.setPinyinScheme(u"full"_s);
        QVERIFY(!composer.doublePinyin());
    }

    void testSwitchSchemeCommitsComposition()
    {
        auto *backend = new TextComposerTestBackend;
        InputEngine engine(backend);
        auto *composer = new PinyinTextComposer;
        engine.setTextComposer(composer);

        QVERIFY(sendText(engine, u"nihao"));
        composer->setPinyinScheme(u"zrm"_s);
        QCOMPARE(backend->commitRequests, QStringList{u"你好"_s});
        QVERIFY(engine.preeditText().isEmpty());
        QVERIFY(engine.candidates().isEmpty());
    }
};

QTEST_GUILESS_MAIN(PinyinTextComposerTest)

#include "pinyintextcomposertest.moc"
