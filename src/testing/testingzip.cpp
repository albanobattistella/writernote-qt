#include "testingzip.h"
#include <QVector>
#include "utils/random/random.h"
#include "utils/WCommonScript.h"
#include "utils/path/get_path.h"
#include "core/WDir.h"
#include "core/WZipWriterSingle.h"
#include "core/WZipReaderSingle.h"

#if (defined(DEBUG_CORE) || defined(DEBUGINFO))

static not_used void gen_num(QVector<int> &data, int min, int max)
{
    int i;
    const int gen = Random::random(min, max);

    data.prepend(gen);

    for(i = 0; i < gen; i++){
        const int res = Random::random(0, INT_MAX);
        data.append(res);
    }
}

static not_used void gen_str(QByteArray &arr, int min_char, int max_char)
{
    constexpr int min = static_cast<int>('a');
    constexpr int max = static_cast<int>('z');
    const int gen = Random::random(min_char, max_char);
    int i;

    for(i = 0; i < gen; i++){
        const char c = static_cast<char>(Random::random(min, max));
        W_ASSERT(c <= 'z' and c >= 'a');
        arr.append(c);
    }
}

static not_used void test_write_zip(QVector<int> &data, const QByteArray &path)
{
    WZipWriterSingle writer;

    writer.init(nullptr, 0, sizeof(int) * data.length());

    for(const auto ref: qAsConst(data)){
        W_ASSERT(sizeof(ref) == sizeof(int));
        writer.write_object(ref);
    }

    writer.commit_change(path, "test");
}

static not_used void test_read_from_zip(QVector<int> &res, const QByteArray &path, int len)
{
    int i;
    bool ok;
    WZip zip(path, ok);
    W_ASSERT(zip.openFileInZip("test") == true);
    W_ASSERT(ok == true);
    WZipReaderSingle reader(&zip, 0);

    for(i = 0; i < len; i++){
        int tmp;
        W_ASSERT(reader.read_object(tmp) == 0);
        res.append(tmp);
    }
}

static not_used void test_check_equal(const QVector<int> &d1, const QVector<int> &d2)
{
    int tmp1, tmp2;
    int i;

    W_ASSERT(d1.length() == d2.length());

    for(i = 0; i < d1.length(); i++){
        tmp1 = d1.at(i);
        tmp2 = d2.at(i);
        W_ASSERT(tmp1 == tmp2);
    }
}

static not_used void run_test_1()
{
    QVector<int> data;
    QVector<int> res;
    QByteArray to = "${HOME}/tmp_writernote_test/";

    WDir::createDir(to);

    gen_num(data, 10000, 1000000);

    gen_str(to, 5, 10);

    test_write_zip(data, to);
    test_read_from_zip(res, to, data.length());

    /* check if data is equal */
    test_check_equal(data, res);
}

#endif

#if defined(DEBUG_CORE)
void __init__ testZip()
{
    constexpr auto max = 10;
    for(int i = 0; i < max; i++){
        WDebug(true, qstr("test : zip : %1 of %2").arg(i).arg(max))
        run_test_1();
    }

}
#endif


