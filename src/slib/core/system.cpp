#include "../../../inc/slib/core/system.h"
#include "../../../inc/slib/core/file.h"
#include "../../../inc/slib/core/log.h"
#include "../../../inc/slib/core/list.h"

SLIB_NAMESPACE_BEGIN

sl_bool System::isMobilePlatform()
{
#if defined(SLIB_PLATFORM_IS_MOBILE)
	return sl_true;
#else
	return sl_false;
#endif
}

String System::getApplicationDirectory()
{
	String path = getApplicationPath();
#if !defined(SLIB_PLATFORM_IS_ANDROID)
	path = File::getParentDirectoryPath(path);
#endif
	return path;
}

#if !defined(SLIB_PLATFORM_IS_APPLE)
String System::getApplicationHomeDirectory()
{
	return getApplicationDirectory();
}

String System::getCachesDirectory()
{
	String dir = getApplicationDirectory() + "/caches";
	File::createDirectory(dir);
	return dir;
}
#endif

#if defined(SLIB_PLATFORM_IS_MOBILE)
static CList<String>& _System_getGlobalUniqueInstances()
{
	SLIB_SAFE_STATIC(CList<String>, lst);
	return lst;
}

struct _SYS_GLOBAL_UNIQUE_INSTANCE
{
	String name;
};

void* System::createGlobalUniqueInstance(const String& _name)
{
	String name = _name;
	if (name.isEmpty()) {
		return sl_null;
	}
	name = File::makeSafeFileName(name);
	CList<String>& lst = _System_getGlobalUniqueInstances();
	if (lst.indexOf(name) >= 0) {
		return sl_null;
	}
	_SYS_GLOBAL_UNIQUE_INSTANCE* instance = new _SYS_GLOBAL_UNIQUE_INSTANCE();
	instance->name = name;
	lst.add(name);
	return instance;
}

void System::freeGlobalUniqueInstance(void* instance)
{
	if (instance) {
		_SYS_GLOBAL_UNIQUE_INSTANCE* l = (_SYS_GLOBAL_UNIQUE_INSTANCE*)(instance);
		if (l) {
			CList<String>& lst = _System_getGlobalUniqueInstances();
			lst.removeValue(l->name);
			delete l;
		}
	}
}
#endif

#if defined(SLIB_PLATFORM_IS_MOBILE)
sl_bool System::createProcess(const String& pathExecutable, const String* commandLines, sl_uint32 nCommandLines)
{
	SLIB_STATIC_STRING(n, "System::createProcess");
	SLIB_STATIC_STRING(v, "Not supported");
	SLIB_LOG_ERROR(n, v);
	return sl_false;
}
#endif

void System::yield(sl_uint32 ellapsed)
{
	if (ellapsed < 16) {
		return;
	}
	if (ellapsed < 32) {
		System::yield();
	} else {
		System::sleep(1);
	}
}

void Console::println(const String& s)
{
	SLIB_STATIC_STRING(l, "\n");
	print(s + l);
}

#if defined(SLIB_PLATFORM_IS_MOBILE)
String Console::readLine()
{
	return String::null();
}
#endif

SLIB_NAMESPACE_END