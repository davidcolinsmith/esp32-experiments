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

#include <stddef.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

#include "lwip/def.h"

#include "esp-tinyosc.h"

#define BUNDLE_ID 0x2362756E646C6500L // "#bundle"

// http://opensoundcontrol.org/spec-1_0
int tosc_parseMessage(tosc_message *o, char *buffer, const int len) {
  // NOTE(mhroth): if there's a comma in the address, that's weird
  int i = 0;
  while (buffer[i] != '\0') ++i; // find the null-terimated address
  while (buffer[i] != ',') ++i; // find the comma which starts the format string
  if (i >= len) return -1; // error while looking for format string
  // format string is null terminated
  o->format = buffer + i + 1; // format starts after comma

  while (i < len && buffer[i] != '\0') ++i;
  if (i == len) return -2; // format string not null terminated

  i = (i + 4) & ~0x3; // advance to the next multiple of 4 after trailing '\0'
  o->marker = buffer + i;

  o->buffer = buffer;
  o->len = len;

  return 0;
}

char *tosc_getAddress(tosc_message *o) {
  return o->buffer;
}

char *tosc_getFormat(tosc_message *o) {
  return o->format;
}

float tosc_getNextFloat(tosc_message *o) {
  // convert from big-endian (network btye order)
  const uint32_t i = lwip_ntohl(*((uint32_t *) o->marker));
  o->marker += 4;
  return *((float *) (&i));
}

void tosc_printMessage(tosc_message *osc) {
  printf("[%i bytes] %s %s",
      osc->len, // the number of bytes in the OSC message
      tosc_getAddress(osc), // the OSC address string, e.g. "/button1"
      tosc_getFormat(osc)); // the OSC format string, e.g. "f"

  for (int i = 0; osc->format[i] != '\0'; i++) {
    switch (osc->format[i]) {
      case 'f': printf(" %g", tosc_getNextFloat(osc)); break;
      default: printf(" Unknown format: '%c'", osc->format[i]); break;
    }
  }
  printf("\n");
}
