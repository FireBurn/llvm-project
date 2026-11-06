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

#endif // LLVM_LIBC_MACROS_SYSTEM_PROPERTIES_MACROS_H
