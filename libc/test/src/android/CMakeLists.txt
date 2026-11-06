add_libc_testsuite(libc-android-tests)

add_libc_test(
  android_get_application_target_sdk_version_test
  SUITE
    libc-android-tests
  SRCS
    android_get_application_target_sdk_version_test.cpp
  DEPENDS
    libc.hdr.android_api_level_macros
    libc.include.android_api_level
    libc.src.android.android_get_application_target_sdk_version
)
