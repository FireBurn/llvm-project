//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of macros from sys/system_properties.h
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_MACROS_SYSTEM_PROPERTIES_MACROS_H
#define LLVM_LIBC_MACROS_SYSTEM_PROPERTIES_MACROS_H

// The longest value, counting the terminator, that a property can have unless
// its name starts with "ro.".
#define PROP_VALUE_MAX 92
// The longest name, counting the terminator, that a property can have unless
// it starts with "ro.".
#define PROP_NAME_MAX 32
// The socket the properties are set through.
#define PROP_SERVICE_NAME "property_service"
// The socket that serves the properties privileged services may set.
#define PROP_SERVICE_FOR_SYSTEM_NAME "property_service_for_system"
// The directory the property area files are in.
#define PROP_DIRNAME "/dev/__properties__"

// Messages sent to the property service.
#define PROP_MSG_SETPROP 1
#define PROP_MSG_SETPROP2 0x00020001

// Status codes the property service answers with.
#define PROP_SUCCESS 0
#define PROP_ERROR_READ_CMD 0x0004
#define PROP_ERROR_READ_DATA 0x0008
#define PROP_ERROR_READ_ONLY_PROPERTY 0x000B
#define PROP_ERROR_INVALID_NAME 0x0010
#define PROP_ERROR_INVALID_VALUE 0x0014
#define PROP_ERROR_PERMISSION_DENIED 0x0018
#define PROP_ERROR_INVALID_CMD 0x001B
#define PROP_ERROR_HANDLE_CONTROL_MESSAGE 0x0020
#define PROP_ERROR_SET_FAILED 0x0024

#endif // LLVM_LIBC_MACROS_SYSTEM_PROPERTIES_MACROS_H
