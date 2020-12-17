/**
 * Copyright (c) 2015-2018, Martin Roth (mhroth@gmail.com)
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef _TINY_OSC_
#define _TINY_OSC_

#include <stdbool.h>
#include <stdint.h>

#define TINYOSC_TIMETAG_IMMEDIATELY 1L

#ifdef __cplusplus
extern "C" {
#endif

typedef struct tosc_message {
  char *format;  // a pointer to the format field
  char *marker;  // the current read head
  char *buffer;  // the original message data (also points to the address)
  uint32_t len;  // length of the buffer data
} tosc_message;

/**
 * Returns a point to the address block of the OSC buffer.
 * This is also the start of the buffer.
 */
char *tosc_getAddress(tosc_message *o);

/**
 * Returns a pointer to the format block of the OSC buffer.
 */
char *tosc_getFormat(tosc_message *o);

/**
 * Returns the length in bytes of this message.
 */
uint32_t tosc_getLength(tosc_message *o);

/**
 * Returns the next 32-bit float. Does not check buffer bounds.
 */
float tosc_getNextFloat(tosc_message *o);

/**
 * Resets the read head to the first element.
 *
 * @return  The same tosc_message pointer.
 */
tosc_message *tosc_reset(tosc_message *o);

/**
 * Parse a buffer containing an OSC message.
 * The contents of the buffer are NOT copied.
 * The tosc_message struct only points at relevant parts of the original buffer.
 * Returns 0 if there is no error. An error code (a negative number) otherwise.
 */
int tosc_parseMessage(tosc_message *o, char *buffer, const int len);



/**
 * A convenience function to (non-destructively) print a pre-parsed OSC message
 * to stdout.
 */
void tosc_printMessage(tosc_message *o);

#ifdef __cplusplus
}
#endif

#endif // _TINY_OSC_
