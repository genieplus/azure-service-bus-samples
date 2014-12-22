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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "proton/message.h"
#include "proton/messenger.h"
#include "proton/error.h"
#ifndef PN_VERSION_MAJOR
#include "proton/version.h"
#endif

#include "common.h"

#define VERBOSE
#define EXTRAVERBOSE

int receive(char *sbnamespace, char *entity, char *issuerName, char *issuerKey)
{
    char address[500];
    SNPRINTF(address, sizeof address,
        "amqps://%s:%s@%s." SERVICEBUS_DOMAIN "/%s",
        issuerName, issuerKey, sbnamespace, entity);

    pn_message_t *message = pn_message();

    printf("CALL pn_messenger... ");
    pn_messenger_t *messenger = pn_messenger(NULL);
    printf("RETURNED\n");

    printf("CALL pn_messenger_set_timeout... ");
    /* Arbitrarily set timeout to 10 seconds */
    int err = pn_messenger_set_timeout(messenger, 10000);
    printf("RETURNED %d\n", err);
    protonError(err, "pn_messenger_set_timeout", messenger);
    if (err != 0)
    {
        return -1;
    }

#if (PN_VERSION_MINOR == 4)
    /*
    ** This API, and the idea of accept modes, only exists in Proton-C 0.4
    ** In more recent versions this idea has been replaced by the incoming
    ** window.
    */
    printf("CALL pn_messenger_set_accept_mode... ");
    err = pn_messenger_set_accept_mode(messenger, PN_ACCEPT_MODE_MANUAL);
    printf("RETURNED %d\n", err);
    protonError(err, "pn_messenger_set_accept_mode", messenger);
    if (err != 0)
    {
        return -1;
    }
#endif

#if (PN_VERSION_MINOR > 7)
    /*
    ** IMPORTANT!
    ** Introduced in 0.8. Setting these modes is vital for reliable
    ** messaging because otherwise the broker will presettle messages
    ** (receive and delete mode) and they will be gone from the
    ** broker. Have to set modes for both directions on the link in
    ** order to make everything at the protocol level work as expected.
    */
    printf("CALL pn_messenger_set_snd_settle_mode...");
    err = pn_messenger_set_snd_settle_mode(messenger, PN_SND_UNSETTLED);
    printf("RETURNED %d\n", err);
    protonError(err, "pn_messenger_set_snd_settle_mode", messenger);
    if (err != 0)
    {
        return -1;
    }

    printf("CALL pn_messenger_set_rcv_settle_mode...");
    err = pn_messenger_set_rcv_settle_mode(messenger, PN_RCV_SECOND);
    printf("RETURNED %d\n", err);
    protonError(err, "pn_messenger_set_rcv_settle_mode", messenger);
    if (err != 0)
    {
        return -1;
    }
#endif

#if (PN_VERSION_MINOR > 4)
    /*
    ** The size of the incoming window determines how many messages retrieved
    ** with pn_messenger_get() are tracked by the messenger. If you _get() more
    ** messages than the window holds, the oldest message falls out, and if it
    ** was not explicitly accepted or rejected, it is automatically accepted.
    ** So setting the window size to 1 provides behavior similar to 0.4's
    ** PN_ACCEPT_MODE_AUTO, with the important difference that the auto
    ** accept will not occur until the next call to _get().
    ** This sample does explicit accepts, so the incoming window size doesn't
    ** really matter.
    **
    ** IMPORTANT: Setting the incoming window to nonzero changes Proton-C's
    ** messaging mode to one in which it requests that the broker retain
    ** messages until they are explicitly accepted. This is vital for
    ** reliable messaging!
    */
    printf("CALL pn_messenger_set_incoming_window... ");
    err = pn_messenger_set_incoming_window(messenger, 1);
    printf("RETURNED %d\n", err);
    protonError(err, "pn_messenger_set_incoming_window", messenger);
    if (err != 0)
    {
        return -1;
    }
#endif

    printf("CALL pn_messenger_subscribe... ");
    pn_subscription_t *subscription =
        pn_messenger_subscribe(messenger, address);
    printf("RETURNED\n");
    if (NULL == subscription)
    {
        printf("pn_messenger_subscribe returned NULL\n");
        printf("%s", pn_error_text(pn_messenger_error(messenger)));
        return -1;
    }

    printf("CALL pn_messenger_start... ");
    err = pn_messenger_start(messenger);
    printf("RETURNED %d\n", err);
    protonError(err, "pn_messenger_start", messenger);
    if (err != 0)
    {
        return -1;
    }

    while (true)
    {
        printf("CALL pn_messenger_recv... ");
        err = pn_messenger_recv(messenger, 1);
        printf("RETURNED %d\n", err);
        protonError(err, "pn_messenger_recv", messenger);
        if (PN_TIMEOUT == err)
        {
            printf("Timeout, breaking out of the receive loop\n");
            break;
        }
        else if (err != 0)
        {
            printf("Breaking out of the receive loop\n");
            break;
        }
        else
        {
            while (pn_messenger_incoming(messenger))
            {
                printf("CALL pn_messenger_get... ");
                err = pn_messenger_get(messenger, message);
                printf("RETURNED %d\n", err);
                protonError(err, "pn_messenger_get", messenger);
                pn_tracker_t tracker = pn_messenger_incoming_tracker(messenger);

#ifdef VERBOSE
                printf("########## Begin message ############\n");
                printf("Address: %s\n", pn_message_get_address(message));
                printf("Content type: %s\n",
                    pn_message_get_content_type(message));
#endif
                pn_atom_t correlation_id =
                    pn_message_get_correlation_id(message);
                if (correlation_id.type != PN_UUID)
                {
                    printf("Correlation id is not a UUID\n");
                }
                else
                {
#ifdef VERBOSE
                    printf("Correlation id:\n");
                    outputUuid(&correlation_id.u.as_uuid);
#endif
                }
                pn_atom_t message_id = pn_message_get_id(message);
                if (message_id.type != PN_UUID)
                {
                    printf("Message id is not a UUID\n");
                }
                else
                {
#ifdef VERBOSE
                    printf("Message id:\n");
                    outputUuid(&message_id.u.as_uuid);
#endif
                }
#ifdef VERBOSE
                printf("Reply to: %s\n", pn_message_get_reply_to(message));
                printf("Reply to group id: %s\n",
                        pn_message_get_reply_to_group_id(message));
                printf("Group id: %s\n", pn_message_get_group_id(message));
                pn_bytes_t user_id = pn_message_get_user_id(message);
                printf("User id: ");
                size_t j;
                for (j = 0; j < user_id.size; j++)
                {
                    printf("%c", user_id.start[j]);
                }
                printf("\nTTL: %d\n", pn_message_get_ttl(message));
#endif

                pn_data_t *body = pn_message_body(message);
                char buffer[1024];
                size_t buffsize = sizeof(buffer);
                pn_data_format(body, buffer, &buffsize);
                printf("Content: %s\n", buffer);

                pn_data_t *properties = pn_message_properties(message);
#ifdef EXTRAVERBOSE
#if (PN_VERSION_MINOR < 6) || (PN_VERSION_MINOR > 7)
                /* There's a bug in pn_data_dump in versions 0.6 and 0.7 */
                pn_data_dump(properties);
                pn_data_next(properties);
#endif
#endif
#ifdef VERBOSE
                printf("########## End message ############\n");
#endif

                printf("CALL pn_messenger_accept... ");
                err = pn_messenger_accept(messenger, tracker, PN_CUMULATIVE);
                printf("RETURNED %d\n", err);
                protonError(err, "pn_messenger_accept", messenger);
            }
        }
    }

    printf("CALL pn_messenger_stop... ");
    pn_messenger_stop(messenger);
    printf("RETURNED\n");
    pn_messenger_free(messenger);

    pn_message_free(message);

    return 0;
}


int main(int argc, char **argv)
{
    if (argc != 5)
    {
        printf("Usage: %s namespace entity issuer-name issuer-key\n", argv[0]);
        return 1;
    }

    // For Proton-C versions 0.4-0.6, the key MUST NOT be URL-encoded.
    // For Proton-C versions 0.7+, the key MUST be URL-encoded.
#if (PN_VERSION_MINOR >= 7)
    char *key = urlEncodeKey(argv[4]);
#else
    char *key = argv[4];
#endif
    receive(argv[1], argv[2], argv[3], key);
    return 0;
}
