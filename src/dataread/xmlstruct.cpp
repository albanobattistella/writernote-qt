#include "xmlstruct.h"
#include "core/WString.h"
#include "utils/WCommonScript.h"
#include "file/File.h"

xmlstruct::xmlstruct(const WByteArray *path_U, Document *currenttitle_U)
{
    setData(path_U, currenttitle_U);
}

void xmlstruct::setData(const WByteArray *path_U, Document *currenttitle_U)
{
    this->_path = path_U;
    this->_doc = currenttitle_U;
}

size_t xmlstruct::get_size_file(const WByteArray &path)
{
    size_t size;
    File file(path);

    if (!file.open(File::OpenMode::readOnly))
        return 0;

    size = file.size();

    file.close();

    return size;
}

xmlstruct::xmlstruct(const WByteArray &path_U, Document &currenttitle_U)
{
    setData(&path_U, &currenttitle_U);
}
