/*
 *  Copyright 2014 Microsoft Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
*/

#ifndef __COMMON_H
#define __COMMON_H

#include "proton/messenger.h"

#ifdef _WIN32
#define SNPRINTF _snprintf
#else
#define SNPRINTF snprintf
#endif

#ifndef SERVICEBUS_DOMAIN
#define SERVICEBUS_DOMAIN	"servicebus.windows.net"
#endif

extern void protonError(int err, char *step, pn_messenger_t *messenger);
extern void generateUuid(pn_uuid_t *pGenerated);
extern void outputUuid(pn_uuid_t *pUuid);
extern char *urlEncodeKey(const char *key);

#endif /* __COMMON_H */
